import uri from '@ohos.uri'
import HashMap from '@ohos.util.HashMap'
import { ObjectUtils } from '../utils/Index'
import { CordovaInterface } from './CordovaInterface'
import { CordovaPlugin } from './CordovaPlugin'
import { CordovaWebView } from './CordovaWebView'
import { PluginEntry } from './PluginEntry'
import { PluginResult } from './PluginResult'
import { MessageStatus } from './enumeration/MessageStatus'
import { CallbackContext } from './CallbackContext'
import { CordovaManager } from '../CordovaManager'

/**
 * CordovaPlugin插件管理器
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/25 14:28
 */
export class PluginManager {
  private readonly ctx: CordovaInterface
  private readonly app: CordovaWebView
  private readonly pluginMap: HashMap<string, CordovaPlugin | null> = new HashMap()
  private readonly entryMap: HashMap<string, PluginEntry> = new HashMap()
  private isInitialized: boolean = false

  constructor(cordovaWebView: CordovaWebView, cordova: CordovaInterface, pluginEntries: PluginEntry[]) {
    this.ctx = cordova
    this.app = cordovaWebView
    this.setPluginEntries(pluginEntries)
  }

  getPluginEntries(): PluginEntry[] {
    const entries: PluginEntry[] = []
    const iterator = this.entryMap.values()
    while (true) {
      const next = iterator.next()
      if (next.done) {
        break
      }
      entries.push(next.value)
    }
    return entries
  }

  setPluginEntries(pluginEntries: PluginEntry[]) {
    if (this.isInitialized) {
      this.onPageHide(false)
      this.onAboutToDisappear()
      this.pluginMap.clear()
      this.entryMap.clear()
    }

    for (const entry of pluginEntries) {
      this.addService(entry)
    }

    if (this.isInitialized) {
      this.startupPlugins()
    }

  }

  init() {
    this.isInitialized = true
    this.onPageHide(false)
    this.onAboutToDisappear()
    this.pluginMap.clear()
    this.startupPlugins()
  }

  private startupPlugins() {
    const iterator = this.entryMap.values()
    while (true) {
      const next = iterator.next()
      if (next.done) {
        break
      }
      const entry: PluginEntry = next.value
      if (entry.onload) {
        this.getPlugin(entry.service)
      } else {
        this.pluginMap.set(entry.service, null)
      }
    }
  }


  async exec(service: string, action: string, callbackId: string, rawArgs: string) {
    // 权限校验，校验不通过时，直接返错
    const webContainer = this.app.getWebContainer()
    const host = new uri.URI(webContainer.getUrl()).host
    let granted = CordovaManager.getWhiteDomains().some(it => it === host)
    if (!granted) {
      const bridges = webContainer.getBridgePermission()
      if (bridges && bridges.length > 0) {
        const bridge = bridges.find((value => value.name === service))
        granted = bridge?.methods === 'allAllow' || (bridge?.methods ?? '').split(',').some(it => it === action)
      }
    }
    if (!granted) {
      this.app.sendPluginResult(PluginResult.create(MessageStatus.ILLEGAL_ACCESS_EXCEPTION), callbackId)
      return
    }

    // 别名替换
    const aliasConfig = CordovaManager.getBridgeAlias(service, action)
    if (!ObjectUtils.isNull(aliasConfig)) {
      service = aliasConfig!.service
      action = aliasConfig!.action
    }
    const plugin = this.getPlugin(service)
    if (ObjectUtils.isNull(plugin)) {
      this.app.sendPluginResult(PluginResult.create(MessageStatus.CLASS_NOT_FOUND_EXCEPTION), callbackId)
    } else {
      const callbackContext: CallbackContext = new CallbackContext(callbackId, this.app)
      try {
        const wasValidAction = await plugin!.execute(action, JSON.parse(rawArgs), callbackContext)
        if (!wasValidAction) {
          callbackContext.sendPluginResult(PluginResult.create(MessageStatus.INVALID_ACTION))
        }
      } catch (e) {
        callbackContext.errorByString(e.message)
      }
    }
  }

  getPlugin(service: string): CordovaPlugin | null {
    let ret: CordovaPlugin | null = this.pluginMap.get(service)
    if (ObjectUtils.isNull(ret)) {
      const pe = this.entryMap.get(service)
      if (ObjectUtils.isNull(pe)) {
        return null
      }

      if (!ObjectUtils.isNull(pe.plugin)) {
        ret = pe.plugin!
      } else if (!ObjectUtils.isNull(pe.pluginFactory)) {
        ret = pe.pluginFactory!()
      } else {
        throw new Error('the CordovaPlugin is illegal, it should has one of plugin or pluginFactory prop')
      }
      ret!.privateInitialize(service, this.ctx, this.app)
      this.pluginMap.set(service, ret)
    }
    return ret
  }

  addService(entry: PluginEntry) {
    this.entryMap.set(entry.service, entry)
    if (!ObjectUtils.isNull(entry.plugin)) {
      entry.plugin!.privateInitialize(entry.service, this.ctx, this.app)
      this.pluginMap.set(entry.service, entry.plugin!)
    }
  }

  onPageShow(multitasking: boolean) {
    const iterator: IterableIterator<CordovaPlugin | null> = this.pluginMap.values()
    while (true) {
      const next = iterator.next()
      if (next.done) {
        break
      }
      next.value?.onPageShow(multitasking)
    }
  }

  onPageHide(multitasking: boolean) {
    const iterator: IterableIterator<CordovaPlugin | null> = this.pluginMap.values()
    while (true) {
      const next = iterator.next()
      if (next.done) {
        break
      }
      next.value?.onPageHide(multitasking)
    }
  }

  onAboutToDisappear() {
    const iterator: IterableIterator<CordovaPlugin | null> = this.pluginMap.values()
    while (true) {
      const next = iterator.next()
      if (next.done) {
        break
      }
      next.value?.onAboutToDisappear()
    }
  }

  postMessage(id: string, data: object | null): object | null {
    const iterator: IterableIterator<CordovaPlugin | null> = this.pluginMap.values()
    while (true) {
      const next = iterator.next()
      if (next.done) {
        break
      }
      if (ObjectUtils.isNull(next.value)) {
        continue
      }
      const obj = (next.value as CordovaPlugin).onMessage(id, data)
      if (!ObjectUtils.isNull(obj)) {
        return obj
      }
    }

    return this.ctx.onMessage(id, data)
  }

  shouldAllowNavigation(url: string): boolean {
    const iterator: IterableIterator<PluginEntry> = this.entryMap.values()
    while (true) {
      const next = iterator.next()
      if (next.done) {
        break
      }
      const plugin: CordovaPlugin | null = this.pluginMap.get((next.value as PluginEntry).service)
      if (!ObjectUtils.isNull(plugin)) {
        const result: boolean | null | undefined = plugin?.shouldAllowNavigation(url)
        if (!ObjectUtils.isNull(result)) {
          return result!
        }
      }
    }

    return url.startsWith('file://') || url.startsWith('about:blank')
  }

  shouldAllowBridgeAccess(url: string): boolean {
    const iterator: IterableIterator<PluginEntry> = this.entryMap.values()
    while (true) {
      const next = iterator.next()
      if (next.done) {
        break
      }
      const plugin: CordovaPlugin | null = this.pluginMap.get((next.value as PluginEntry).service)
      if (!ObjectUtils.isNull(plugin)) {
        const result: boolean | null | undefined = plugin?.shouldAllowBridgeAccess(url)
        if (!ObjectUtils.isNull(result)) {
          return result!
        }
      }
    }

    // todo 白名单
    // return url.startsWith('file://')
    return true
  }
}
