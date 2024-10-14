/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// DocsCode 7
import { rcp } from '@kit.RemoteCommunicationKit';
import { url } from '@kit.ArkTS';
import Logger from '../common/Logger';
import { NetworkQualityProvider } from './NetworkStateSimulator';

// Define the RequestUrlChangeInterceptor interceptor
export class RequestUrlChangeInterceptor implements rcp.Interceptor {
  private readonly networkQualityProvider: NetworkQualityProvider;

  constructor(networkQualityProvider: NetworkQualityProvider) {
    this.networkQualityProvider = networkQualityProvider
  }

  // Custom Request processing Logic
  async intercept(context: rcp.RequestContext, next: rcp.RequestHandler): Promise<rcp.Response> {
    if (context.request.method === 'GET' && !this.networkQualityProvider.isNetworkFast()) {
      Logger.info('[RequestUrlChangeInterceptor]: Slow network is detected');
      const parts = context.request.url.pathname.split('.');
      if (parts.length === 2) {
        const changed = url.URL.parseURL(context.request.url.href);
        changed.pathname = parts[0] + '_small.' + parts[1];
        Logger.info(`[RequestUrlChangeInterceptor]: Replace URL from "${context.request.url.href}" to "${changed}"`);
        AppStorage.setOrCreate('ReplacedInfo',`[RequestUrlChangeInterceptor]: Replace URL from "${context.request.url.href}" to "${changed}"`);
        context.request.url = changed;
      }
    } else {
      Logger.info('[RequestUrlChangeInterceptor]: Network is fast');
    }
    return next.handle(context);
  }
}

// Define the ResponseHeaderRemoveInterceptor interceptor
export class ResponseHeaderRemoveInterceptor implements rcp.Interceptor {
  // Custom Response processing Logic
  async intercept(context: rcp.RequestContext, next: rcp.RequestHandler): Promise<rcp.Response> {
    const response = await next.handle(context);
    const toReturn: rcp.Response = {
      request: response.request,
      statusCode: response.statusCode,
      httpVersion: response.httpVersion,
      headers: {
        'content-range': response.headers['content-range']
      },
      effectiveUrl: response.effectiveUrl,
      timeInfo: response.timeInfo,
      toJSON: () => null
    };
    Logger.info('[ResponseHeaderRemoveInterceptor]: Response was modified');
    return toReturn;
  }
}
// DocsCode 7