import http from '@ohos.net.http'
import util from "@ohos.util";

import { TurboModule } from "../../../RNOH/TurboModule";
import { NetworkEventsDispatcher } from './NetworkEventDispatcher';
import ArrayList from '@ohos.util.ArrayList';
import { BlobMetadata } from '../Blob';
import {
  CancelRequestCallback,
  HttpErrorResponse,
  ReceivingProgress,
  SendingProgress
} from '../../../HttpClient/types';

type ResponseType =
| 'base64'
  | 'blob'
  | 'text';

interface Query {
  method: string,
  url: string,
  data: Object,
  headers: Object,
  responseType: ResponseType,
  incrementalUpdates: boolean,
  timeout: number,
  withCredentials: boolean,
}

export type UriHandler = {
  supports: (query: Query) => boolean;
  fetch: (query: Query) => Promise<Object>;
}

export type JsFormPart = {
  fieldName: string,
  headers: object,
  name: string,
  uri: string,
  string?: string,
}

export type JsFormData = {
  formData: JsFormPart[]
}

export type RequestBodyHandler = {
  supports: (data: Object) => boolean;
  handleRequest: (data: { blob: BlobMetadata }, contentType?: string) => {
    body: string | ArrayBuffer | Object,
    contentType?: string
  };
}

export type ResponseBodyHandler = {
  supports: (responseType: ResponseType) => boolean,
  handleResponse: (response: string | Object | ArrayBuffer) => BlobMetadata
}

/**
 * This is a TurboModule for handling Networking requests. It encodes the data it gets from JS
 * (a JSON object with different fields) into a Harmony specific format.
 * It also exposes handlers which can be used to customize how requests are handled.
 */

export class NetworkingTurboModule extends TurboModule {
  public static readonly NAME = 'Networking';
  private networkEventDispatcher: NetworkEventsDispatcher = new NetworkEventsDispatcher(this.ctx.rnInstance)
  private base64Helper: util.Base64Helper = new util.Base64Helper();
  private uriHandlers: ArrayList<UriHandler> = new ArrayList();
  private requestCancellersById: Map<number, CancelRequestCallback> = new Map();
  private nextId: number = 0
  private requestBodyHandlers: ArrayList<RequestBodyHandler> = new ArrayList();
  private responseBodyHandlers: ArrayList<ResponseBodyHandler> = new ArrayList();

  private REQUEST_METHOD_BY_NAME: Record<string, http.RequestMethod> = {
    OPTIONS: http.RequestMethod.OPTIONS,
    GET: http.RequestMethod.GET,
    HEAD: http.RequestMethod.HEAD,
    POST: http.RequestMethod.POST,
    PUT: http.RequestMethod.PUT,
    DELETE: http.RequestMethod.DELETE,
    TRACE: http.RequestMethod.TRACE,
    CONNECT: http.RequestMethod.CONNECT,
  }

  addUriHandler(handler: UriHandler) {
    this.uriHandlers.add(handler);
  }

  removeUriHandler(handler: UriHandler) {
    this.uriHandlers.remove(handler);
  }

  addRequestBodyHandler(handler: RequestBodyHandler) {
    this.requestBodyHandlers.add(handler)
  }

  removeRequestBodyHandler(handler: RequestBodyHandler) {
    this.requestBodyHandlers.remove(handler)
  }

  addResponseBodyHandler(handler: ResponseBodyHandler) {
    this.responseBodyHandlers.add(handler)
  }

  removeResponseBodyHandler(handler: ResponseBodyHandler) {
    this.responseBodyHandlers.remove(handler)
  }

  decodeBufferToText(buf: ArrayBuffer): string {
    const textDecoder = util.TextDecoder.create();
    const byteArray = new Uint8Array(buf);
    return textDecoder.decodeWithStream(byteArray, { stream: false });
  }

  async encodeResponse(response: string | Object | ArrayBuffer, responseType: ResponseType): Promise<string | Object> {
    if (responseType === 'text') {
      if (typeof response === 'string') {
        return response;
      } else if (response instanceof ArrayBuffer) {
        return this.decodeBufferToText(response);
      } else {
        // NOTE: Object responses have been long deprecated in Ark, we don't expect them here
        throw new Error("INTERNAL: unexpected Object http response");
      }
    } else if (responseType === 'base64') {
      let byteArray: Uint8Array;
      if (typeof response === 'string') {
        const textEncoder = new util.TextEncoder();
        byteArray = textEncoder.encodeInto(response);
      } else if (response instanceof ArrayBuffer) {
        byteArray = new Uint8Array(response);
      } else {
        throw new Error("INTERNAL: unexpected Object http response");
      }
      return this.base64Helper.encodeToString(byteArray);
    }

    throw new Error("Unsupported query response type");
  }

  private encodeBody(data: Object): string | ArrayBuffer | Object | undefined {
    if ('trackingName' in data) {
      delete data.trackingName;
    }
    if ('string' in data) {
      return data.string as string
    }
    if ('base64' in data) {
      const base64 = data.base64 as string;
      const textEncoder = new util.TextEncoder();
      const byteArray = textEncoder.encodeInto(base64);
      return byteArray.buffer;
    }
    if (!Object.keys(data).length) {
      return undefined;
    }
    return data;
  }

  private encodeFormData(jsFormData: JsFormData): Array<http.MultiFormData> {
    const formData = jsFormData.formData.map((jsFormPart: JsFormPart): http.MultiFormData => {
      const contentType = jsFormPart.headers['content-type'] ?? 'text/plain'
      return {
        name: jsFormPart.fieldName,
        contentType: contentType,
        data: jsFormPart.string,
        filePath: jsFormPart.uri,
        remoteFileName: jsFormPart.name,
      }
    })
    return formData;
  }

  async clearCookies(callback: (didDeleteAnyCookies: boolean) => void) {
    const didDeleteAnyCookies = await this.ctx.rnInstance.httpClient.clearCookies();
    callback(didDeleteAnyCookies);
  }

  private isEncodedURI(str: string): boolean {
    try {
      const decodedStr = decodeURI(str);
      return decodedStr !== str;
    } catch (e) {
      return false;
    }
  }

  private getEncodedURI(str: string): string {
    if (this.isEncodedURI(str)) {
      return str;
    }
    return encodeURI(str);
  }
  

  async sendRequest(query: Query, callback: (requestId: number) => void) {
    const httpClient = this.ctx.rnInstance.httpClient;
    const requestId = this.createId();
    for (const handler of this.uriHandlers) {
      if (handler.supports(query)) {
        const response = handler.fetch(query);
        this.networkEventDispatcher.dispatchDidReceiveNetworkData(requestId, response);
        this.networkEventDispatcher.dispatchDidCompleteNetworkResponse(requestId);
        return;
      }
    }

    let requestBodyHandler: RequestBodyHandler = null;
    if (query.data) {
      for (const handler of this.requestBodyHandlers) {
        if (handler.supports(query.data)) {
          requestBodyHandler = handler;
          break;
        }
      }
    }

    let extraData = null;
    let headers: any = query.headers;
    let multiFormDataList: Array<http.MultiFormData> = [];

    if (requestBodyHandler != null) {
      const requestData = requestBodyHandler.handleRequest(query.data as { blob: BlobMetadata }, query.responseType);
      extraData = requestData.body;
      headers['Content-Type'] = requestData.contentType;
    } else if ('formData' in query.data) {
      headers['Content-Type'] = 'multipart/form-data'
      multiFormDataList = this.encodeFormData(query.data as JsFormData);
    } else {
      extraData = this.encodeBody(query.data);
    }
    let onReceiveProgress: (receiveProgress: ReceivingProgress) => void | null = null;

    if (query.incrementalUpdates) {
      if (query.responseType === 'text') {
        const textDecoder = util.TextDecoder.create('utf-8');
        onReceiveProgress = (receiveProgress: ReceivingProgress) => {
          const uintArray = new Uint8Array(receiveProgress.bitsReceived);
          const decodedStr = textDecoder.decodeWithStream(uintArray,
            { stream: true }); //we want to carry over bytes from incomplete characters
          this.networkEventDispatcher.dispatchDidReceiveNetworkIncrementalData(requestId, decodedStr,
            receiveProgress.lengthReceived, receiveProgress.totalLength)
        }
      } else {
        onReceiveProgress = (partialProgress: ReceivingProgress) => {
          this.networkEventDispatcher.dispatchDidReceiveNetworkDataProgress(requestId, partialProgress.lengthReceived,
            partialProgress.totalLength)
        }
      }
    }

    let onSendProgress = (sendProgress: SendingProgress) => {
      this.networkEventDispatcher.dispatchDidSendNetworkData(requestId, sendProgress.lengthSent,
        sendProgress.totalLength)
    };


    const { cancel, promise } = httpClient.sendRequest(this.getEncodedURI(query.url),
      {
        method: this.REQUEST_METHOD_BY_NAME[query.method],
        header: query.headers,
        extraData: extraData,
        timeout: query.timeout,
        multiFormDataList: multiFormDataList,
        onReceiveProgress: onReceiveProgress,
        onSendProgress: onSendProgress,
        handleCookies: query.withCredentials,
      },)
    this.requestCancellersById.set(requestId, cancel);

    promise.then(async (httpResponse) => {
      this.networkEventDispatcher.dispatchDidReceiveNetworkResponse(requestId, httpResponse.statusCode,
        httpResponse.headers, query.url);
      if (this.requestCancellersById[requestId]) {
        this.requestCancellersById.delete(requestId);
      }
      for (const handler of this.responseBodyHandlers) {
        if (handler.supports(query.responseType)) {
          this.networkEventDispatcher.dispatchDidReceiveNetworkData(requestId,
            handler.handleResponse(httpResponse.body));
          this.networkEventDispatcher.dispatchDidCompleteNetworkResponse(requestId);
          return;
        }
      }
      this.networkEventDispatcher.dispatchDidReceiveNetworkData(requestId,
        await this.encodeResponse(httpResponse.body, query.responseType));
      this.networkEventDispatcher.dispatchDidCompleteNetworkResponse(requestId);
    }).catch((errorResponse: HttpErrorResponse) => {
      this.networkEventDispatcher.dispatchDidReceiveNetworkResponse(requestId, errorResponse.statusCode || 0, {},
        query.url)
      this.networkEventDispatcher.dispatchDidCompleteNetworkResponseWithError(requestId,
        errorResponse.error.toString(), errorResponse.timeout ?? false);
      if (this.requestCancellersById[requestId]) {
        this.requestCancellersById.delete(requestId);
      }
    });


    callback(requestId)
  }

  abortRequest(requestId: number) {
    if (this.requestCancellersById[requestId]) {
      this.requestCancellersById[requestId]();
      this.requestCancellersById.delete(requestId);
    }
  }

  private createId(): number {
    return this.nextId++;
  }
}
