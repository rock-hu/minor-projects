/**
 * Web组件使用的Model定义
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/27 11:10
 */

export interface AlertEvent {
  url: string;
  message: string;
  result: JsResult
}

export interface ConfirmEvent {
  url: string;
  message: string;
  result: JsResult
}

export interface PromptEvent {
  url: string
  message: string
  value: string
  result: JsResult
}

export interface ErrorEvent {
  request: WebResourceRequest,
  error: WebResourceError
}

export interface SslErrorEvent {
  handler: SslErrorHandler
  error: SslError
}

export interface OnTitleReceiveEvent {
  title: string
}

export interface LoadInterceptEvent {
  data: WebResourceRequest
}

export interface HttpErrorEvent {
  request: WebResourceRequest,
  response: WebResourceResponse
}

export interface ShowFileSelectorEvent {
  result: FileSelectorResult,
  fileSelector: FileSelectorParam
}

export interface OnInterceptRequestEvent {
  request: WebResourceRequest
}