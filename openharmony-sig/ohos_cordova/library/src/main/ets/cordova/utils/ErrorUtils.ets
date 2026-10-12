import HashMap from '@ohos.util.HashMap'

/**
 * Cordova Error工具类
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/27 14:40
 */

export enum CordovaErrorType {
  /**
   * 参数不存在
   */
  PARAM_NO_EXIST = 'NO_EXIST',
  /**
   * 参数不合法
   */
  PARAM_ILLEGAL = 'PARAM_ILLEGAL',
  /**
   * 参数解析错误
   */
  PARAM_PARSING_ERROR = 'PARSING_ERROR',
  /**
   * 用户取消
   */
  USER_CANCEL = 'USER_CANCEL',
  /**
   * 不支持此方法
   */
  METHOD_NOT_AVAILABLE = 'METHOD_NOT_AVAILABLE',
  /**
   * 添加快捷方式失败
   */
  SHORTCUT_ERROR = 'SHORTCUT_ERROR',
  /**
   * 运行时异常
   */
  RUNTIME_ERROR = 'RUNTIME_ERROR',
  /**
   * 报文加解密失败
   */
  ENCRYPT_ERROR = 'ENCDEC_ERROR',
  /**
   * 获取本地图片异常
   */
  GET_LOCAL_IMG_ERROR = 'GETLOCALIMG_ERROR',
  /**
   * 权限不足
   */
  NO_PERMISSION_ERROR = 'NOPERMISSION_ERROR',
  /**
   * 请您安装微信客户端
   */
  WX_NOT_INSTALLED_ERROR = 'WXNOTINSTALLED_ERROR',
  /**
   * 文件下载失败，请稍后重试
   */
  DOWNLOAD_FILE_ERROR = 'DOWNLOADFILE_ERROR',
  /**
   * 定位失败
   */
  LOCATION_ERROR = 'LBS_ERROR'
}

interface CordovaError {
  code: string
  message: string
}

export function createCordovaError(code: string, message: string): CordovaError {
  return { code, message }
}

const GENERAL_ERROR_DESC: HashMap<CordovaErrorType, CordovaError> = new HashMap()
GENERAL_ERROR_DESC.set(CordovaErrorType.PARAM_NO_EXIST, createCordovaError(CordovaErrorType.PARAM_NO_EXIST, '参数不存在'))
GENERAL_ERROR_DESC.set(CordovaErrorType.PARAM_ILLEGAL, createCordovaError(CordovaErrorType.PARAM_ILLEGAL, '参数不合法'))
GENERAL_ERROR_DESC.set(CordovaErrorType.PARAM_PARSING_ERROR, createCordovaError(CordovaErrorType.PARAM_PARSING_ERROR, '参数解析错误'))
GENERAL_ERROR_DESC.set(CordovaErrorType.USER_CANCEL, createCordovaError(CordovaErrorType.USER_CANCEL, '用户取消'))
GENERAL_ERROR_DESC.set(CordovaErrorType.METHOD_NOT_AVAILABLE, createCordovaError(CordovaErrorType.METHOD_NOT_AVAILABLE, '不支持此方法'))
GENERAL_ERROR_DESC.set(CordovaErrorType.SHORTCUT_ERROR, createCordovaError(CordovaErrorType.SHORTCUT_ERROR, '添加快捷方式失败'))
GENERAL_ERROR_DESC.set(CordovaErrorType.RUNTIME_ERROR, createCordovaError(CordovaErrorType.RUNTIME_ERROR, '运行时异常'))
GENERAL_ERROR_DESC.set(CordovaErrorType.ENCRYPT_ERROR, createCordovaError(CordovaErrorType.ENCRYPT_ERROR, '报文加解密失败'))
GENERAL_ERROR_DESC.set(CordovaErrorType.GET_LOCAL_IMG_ERROR, createCordovaError(CordovaErrorType.GET_LOCAL_IMG_ERROR, '获取本地图片异常'))
GENERAL_ERROR_DESC.set(CordovaErrorType.NO_PERMISSION_ERROR, createCordovaError(CordovaErrorType.NO_PERMISSION_ERROR, '权限不足'))
GENERAL_ERROR_DESC.set(CordovaErrorType.WX_NOT_INSTALLED_ERROR, createCordovaError(CordovaErrorType.WX_NOT_INSTALLED_ERROR, '请您安装微信客户端'))
GENERAL_ERROR_DESC.set(CordovaErrorType.DOWNLOAD_FILE_ERROR, createCordovaError(CordovaErrorType.DOWNLOAD_FILE_ERROR, '文件下载失败，请稍后重试'))
GENERAL_ERROR_DESC.set(CordovaErrorType.LOCATION_ERROR, createCordovaError(CordovaErrorType.LOCATION_ERROR, '定位失败'))


/**
 * 获取一个通用的Cordova Error描述对象
 *
 * @param type 通用错误类型
 * @returns 生成好的通用错误对象
 */
export function getGeneralCordovaError(type: CordovaErrorType): CordovaError {
  return GENERAL_ERROR_DESC.get(type) || createCordovaError(CordovaErrorType.RUNTIME_ERROR, '运行时异常')
}
