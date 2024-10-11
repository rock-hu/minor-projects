/*
 * The MIT License (MIT)
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 */
import axios, {
  AxiosError,
  AxiosResponse,
  AxiosProgressEvent,
  InternalAxiosRequestConfig,
  AxiosRequestConfig
} from '@ohos/axios'
import { FormData } from '@ohos/axios'
import fs from '@ohos.file.fs';
import { IdModel, InfoModel, UploadModel } from '../types/types'
import { DEMO_CONFIG, LOG } from '../common/Common'
import { writeFile, readFile } from '../common/fileFs'

axios.defaults.headers['authorization'] = 'customer-auto'
axios.defaults.baseURL = DEMO_CONFIG.baseUrl

const TAG = LOG.TAG

@Entry
@Component
struct Index {
  scroller: Scroller = new Scroller()
  @State status: string | number = ''
  @State message: string = ''
  @State performanceTiming: string = '';
  @State filename: string = 'blue.jpg'
  @State progress: string = ''
  @State downloadProgress: number = 0
  @State uploadProgress: number = 0
  @State showUrl: string = '';
  @State connectTimeout: number = 0;
  @State readTimeout: number = 0;
  @State startTime: number = 0;
  @State endTime: number = 0;
  @State maxBodyLength: number = -1;
  @State maxContentLength: number = -1;
  baseUrl: string = DEMO_CONFIG.baseUrl;
  getUrl: string = DEMO_CONFIG.getUrl;
  postUrl: string = DEMO_CONFIG.postUrl;
  putUrl: string = DEMO_CONFIG.putUrl;
  deleteUrl: string = DEMO_CONFIG.deleteUrl;
  downloadUrl: string = DEMO_CONFIG.downloadUrl;
  uploadUrl: string = DEMO_CONFIG.uploadUrl;
  clientCert_noPsw: string = DEMO_CONFIG.clientCert_noPsw;
  clientCert_hasPsw: string = DEMO_CONFIG.clientCert_hasPsw;
  proxyUrl: string = DEMO_CONFIG.proxyUrl;
  psw: string = DEMO_CONFIG.psw;
  host: string = DEMO_CONFIG.host;
  controller: TextInputController = new TextInputController()

  build() {
    Scroll(this.scroller) {
      Column() {
        //axios标题
        Text('axios')
          .fontSize(28)
          .fontWeight(700)
          .fontColor('#000000')
          .margin({ top: 20, bottom: 25 })

        //请求按钮
        Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Start, wrap: FlexWrap.Wrap }) {
          Button('Creat')
            .width(100)
            .margin({
              bottom: 10,
              right: 10
            }).onClick((e) => {
            this.creat()
          })
          Button('Request')
            .width(100)
            .margin({
              bottom: 10,
              right: 10
            }).onClick((e) => {
            this.request()
          })
          Button('Get')
            .width(100)
            .margin({
              bottom: 10,
              right: 10
            }).onClick((e) => {
            this.get()
          })

          Button('Post').width(100).margin({
            bottom: 10,
            right: 10
          }).onClick((e) => {
            this.post()
          })
          Button('Put').width(100).margin({
            bottom: 10,
            right: 10
          }).onClick((e) => {
            this.put()
          })
          Button('Delete').width(100).margin({
            bottom: 10,
            right: 10
          }).onClick((e) => {
            this.delete()
          })

          Button($r("app.string.Index_interceptors")).width(100).margin({
            bottom: 10,
            right: 10
          }).onClick((e) => {
            this.interceptors()
          })

          Button($r("app.string.Index_defaults")).width(100).margin({
            bottom: 10,
            right: 10
          }).onClick((e) => {
            this.defaultSetting()
          })

          Button($r("app.string.Index_download")).width(100).margin({
            bottom: 10,
            right: 10
          }).onClick((e) => {
            this.download()
          })

          Button($r("app.string.Index_upload")).width(100).margin({
            bottom: 10,
            right: 10
          }).onClick((e) => {
            this.upload()
          })

          Button($r("app.string.Index_authenticate_no_password")).width(100).margin({
            bottom: 10,
            right: 10
          }).onClick((e) => {
            this.settingClientCert()
          })

          Button($r("app.string.Index_authenticate")).width(100).margin({
            bottom: 10,
            right: 10
          }).onClick((e) => {
            this.settingClientCert_psw()
          })

          Button($r("app.string.Index_response_type")).width(100).margin({
            bottom: 10,
            right: 10
          }).onClick((e) => {
            this.settingResponseType()
          })

          Button($r("app.string.Index_proxy")).width(100).margin({
            bottom: 10,
            right: 10
          }).onClick((e) => {
            this.settingProxy()
          })
        }

        //请求connectTimeout
        Column() {
          Text('connectTimeout')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ bottom: 10 })
          TextInput({
            text: String(this.connectTimeout),
            placeholder: 'input your connectTimeout',
            controller: this.controller
          })
            .placeholderColor(Color.Grey)
            .placeholderFont({ size: 14, weight: 400 })
            .caretColor(Color.Blue)
            .width(400)
            .height(40)
            .fontSize(14)
            .fontColor(Color.Black)
            .onChange((value: string) => {
              this.connectTimeout = Number(value) ? Number(value) : 0
            })
        }
        .margin({ top: 10, bottom: 20 })
        .alignItems(HorizontalAlign.Start)

        //请求readTimeout
        Column() {
          Text('readTimeout')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ bottom: 10 })
          TextInput({
            text: String(this.readTimeout),
            placeholder: 'input your readTimeout',
            controller: this.controller
          })
            .placeholderColor(Color.Grey)
            .placeholderFont({ size: 14, weight: 400 })
            .caretColor(Color.Blue)
            .width(400)
            .height(40)
            .fontSize(14)
            .fontColor(Color.Black)
            .onChange((value: string) => {
              this.readTimeout = Number(value) ? Number(value) : 0
            })
        }
        .margin({ top: 10, bottom: 20 })
        .alignItems(HorizontalAlign.Start)

        //请求maxBodyLength
        Column() {
          Text('maxBodyLength')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ bottom: 10 })
          TextInput({
            text: String(this.maxBodyLength),
            placeholder: 'input your maxBodyLength',
            controller: this.controller
          })
            .placeholderColor(Color.Grey)
            .placeholderFont({ size: 14, weight: 400 })
            .caretColor(Color.Blue)
            .width(400)
            .height(40)
            .fontSize(14)
            .fontColor(Color.Black)
            .onChange((value: string) => {
              this.maxBodyLength = Number(value) ? Number(value) : -1;
            })
        }
        .margin({ top: 10, bottom: 20 })
        .alignItems(HorizontalAlign.Start)

        //请求maxContentLength
        Column() {
          Text('MaxContentLength')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ bottom: 10 })
          TextInput({
            text: String(this.maxContentLength),
            placeholder: 'input your maxContentLength',
            controller: this.controller
          })
            .placeholderColor(Color.Grey)
            .placeholderFont({ size: 14, weight: 400 })
            .caretColor(Color.Blue)
            .width(400)
            .height(40)
            .fontSize(14)
            .fontColor(Color.Black)
            .onChange((value: string) => {
              this.maxContentLength = Number(value) ? Number(value) : -1;
            })
        }
        .margin({ top: 10, bottom: 20 })
        .alignItems(HorizontalAlign.Start)


        //请求地址
        Column() {
          Text("Request URL")
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ bottom: 10 })
          Text(this.showUrl.indexOf('http') > -1 ? this.showUrl : (this.baseUrl + this.showUrl))
            .width('100%')
            .height(50)
            .fontSize(14)
            .fontWeight(700)
            .fontColor('#000000')
            .borderStyle(BorderStyle.Solid)
            .borderWidth(1)
            .borderColor('#E6E7E8')
        }
        .margin({ top: 10, bottom: 20 })
        .alignItems(HorizontalAlign.Start)

        //请求结果
        Column() {
          Text("Request Result")
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ bottom: 10 })
            .textAlign(TextAlign.Start)

          //进度条
          if (this.showUrl === this.uploadUrl) {
            Text($r("app.string.Index_upload_progress"))
            Progress({ value: this.uploadProgress, type: ProgressType.Linear })
              .color('#009BE8').width('100%')
              .margin({ top: 8, right: 10 })
              .style({ strokeWidth: 10 })
          }
          if (this.showUrl === this.downloadUrl) {
            Text($r("app.string.Index_download_progress"))
            Progress({ value: this.downloadProgress, type: ProgressType.Linear })
              .color('#009BE8').width('100%')
              .margin({ top: 8, right: 10 })
              .style({ strokeWidth: 10 })
          }
          // 展示请求内容
          Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start, }) {
            Text("Request start time: " + this.startTime)
              .width('100%')
              .fontSize(16)
              .fontWeight(700)
              .fontColor('#000000')
            Text("Request end time: " + this.endTime)
              .width('100%')
              .fontSize(16)
              .fontWeight(700)
              .fontColor('#000000')
            Text("Request response time:  " + (this.endTime - this.startTime))
              .width('100%')
              .fontSize(16)
              .fontWeight(700)
              .fontColor('#000000')
            Text("Status: " + this.status)
              .width('100%')
              .fontSize(16)
              .fontWeight(700)
              .fontColor('#000000')
            Text("performanceTiming: " + this.performanceTiming)
              .width('100%')
              .fontSize(16)
              .fontWeight(700)
              .fontColor('#000000')
            Text("Data: " + this.message)
              .width('100%')
              .fontSize(16)
              .fontWeight(700)
              .fontColor('#000000')
          }
          .width('100%')
          .padding({ top: 20, bottom: 20, right: 15, left: 10 })
          .margin({ right: 10 })
          .borderStyle(BorderStyle.Solid)
          .borderWidth(1)
          .borderColor('#E6E7E8')
        }
        .margin({ top: 10, bottom: 20 })
        .alignItems(HorizontalAlign.Start)

        Button($r("app.string.Index_clear")).width(120).onClick((e) => {
          this.clear()
        })
      }
    }
    .scrollable(ScrollDirection.Vertical) // 滚动方向纵向
    .scrollBar(BarState.On) // 滚动条常驻显示
    .margin({ left: 10, right: 10 })
  }

  // Creat请求
  creat() {
    this.clear()
    this.showUrl = this.getUrl
    const instance = axios.create();
    this.startTime = new Date().getTime();

    instance.get<string, AxiosResponse<string>, null>(this.getUrl, {
      connectTimeout: this.connectTimeout,
      readTimeout: this.readTimeout,
      maxBodyLength: this.maxBodyLength,
      maxContentLength: this.maxContentLength
    }).then((res: AxiosResponse) => {
      this.status = res ? res.status : '';
      this.message = res ? JSON.stringify(res.data) : '';
      this.endTime = new Date().getTime();
    }).catch((err: AxiosError) => {
      this.status = '';
      this.message = err.message;
      this.endTime = new Date().getTime();
    })
  }

  // request请求
  request() {
    this.clear()
    this.showUrl = this.getUrl;
    this.startTime = new Date().getTime();

    axios.request<InfoModel, AxiosResponse<InfoModel>, IdModel>({
      url: this.getUrl,
      method: 'get',
      connectTimeout: this.connectTimeout,
      readTimeout: this.readTimeout,
      maxBodyLength: this.maxBodyLength,
      maxContentLength: this.maxContentLength
    }).then((res: AxiosResponse<InfoModel>) => {
      this.status = res ? res.status : '';
      this.message = res ? JSON.stringify(res.data) : '';
      this.endTime = new Date().getTime();
    }).catch((err: AxiosError) => {
      this.status = '';
      this.message = err.message;
      this.endTime = new Date().getTime();
    })
  }

  // get请求
  get() {
    this.clear()
    this.showUrl = this.getUrl
    this.startTime = new Date().getTime();
    axios.get<string, AxiosResponse<string>, null>(this.getUrl, {
      connectTimeout: this.connectTimeout,
      readTimeout: this.readTimeout,
      maxBodyLength: this.maxBodyLength,
      maxContentLength: this.maxContentLength
    }).then((res: AxiosResponse) => {
      this.status = res ? res.status : '';
      this.message = res ? JSON.stringify(res.data) : '';
      this.performanceTiming = res ? JSON.stringify(res.performanceTiming) : '';
      this.endTime = new Date().getTime();
    }).catch((err: AxiosError) => {
      this.status = '';
      this.message = err.message;
      this.endTime = new Date().getTime();
    })
  }

  // post请求
  post() {
    this.clear()
    this.showUrl = this.postUrl
    this.startTime = new Date().getTime();

    axios<InfoModel, AxiosResponse<InfoModel>, IdModel>({
      url: this.postUrl,
      method: 'post',
      data: {
        id: 591
      },
      connectTimeout: this.connectTimeout,
      readTimeout: this.readTimeout,
      maxBodyLength: this.maxBodyLength,
      maxContentLength: this.maxContentLength
    }).then((res: AxiosResponse<InfoModel>) => {
      this.status = res ? res.status : '';
      this.message = res ? JSON.stringify(res.data) : '';
      this.performanceTiming = res ? JSON.stringify(res.performanceTiming) : '';
      this.endTime = new Date().getTime();
    }).catch((err: AxiosError) => {
      this.status = '';
      this.message = err.message;
      this.endTime = new Date().getTime();
    })
  }

  // put请求
  put() {
    this.clear()
    this.showUrl = this.putUrl;
    this.startTime = new Date().getTime();

    axios.put<string, AxiosResponse<string>, IdModel>(this.putUrl, { id: 590 }, {
      connectTimeout: this.connectTimeout,
      readTimeout: this.readTimeout,
      maxBodyLength: this.maxBodyLength,
      maxContentLength: this.maxContentLength
    }).then((res: AxiosResponse) => {
      this.status = res ? res.status : '';
      this.message = res ? JSON.stringify(res.data) : '';
      this.endTime = new Date().getTime();
    }).catch((err: AxiosError) => {
      this.status = '';
      this.message = err.message;
      this.endTime = new Date().getTime();
    })
  }

  // delete请求
  delete() {
    this.clear()
    this.showUrl = this.deleteUrl;
    this.startTime = new Date().getTime();

    axios.delete<string, AxiosResponse<string>, IdModel>(this.deleteUrl, {
      connectTimeout: this.connectTimeout,
      readTimeout: this.readTimeout,
      maxBodyLength: this.maxBodyLength,
      maxContentLength: this.maxContentLength,
      data: {
        id: 591
      }
    }).then((res: AxiosResponse) => {
      this.status = res ? res.status : '';
      this.message = res ? JSON.stringify(res.data) : '';
      this.endTime = new Date().getTime();
    }).catch((err: AxiosError) => {
      this.status = '';
      this.message = err.message;
      this.endTime = new Date().getTime();
    })
  }

  // 拦截器
  interceptors() {
    this.clear()
    this.showUrl = this.getUrl
    this.startTime = new Date().getTime();

    const myInterceptor = axios.interceptors.response.use((response: AxiosResponse) => {
      // 对响应数据做点什么
      response.data = '在拦截器中，内容被更改了'
      return response;
    }, (error: AxiosError) => {
      // 对响应错误做点什么
      return Promise.reject(error);
    });

    axios<InfoModel[], AxiosResponse<InfoModel[]>, null>({
      url: this.getUrl,
      method: 'get',
      connectTimeout: this.connectTimeout,
      readTimeout: this.readTimeout,
      maxBodyLength: this.maxBodyLength,
      maxContentLength: this.maxContentLength
    }).then((res: AxiosResponse<InfoModel[]>) => {
      this.status = res ? res.status : '';
      this.message = res ? JSON.stringify(res.data) : '';
      // 移除拦截器
      axios.interceptors.response.eject(myInterceptor);
      this.endTime = new Date().getTime();
    }).catch((err: AxiosError) => {
      this.status = '';
      this.message = err.message;
      // 移除拦截器
      axios.interceptors.response.eject(myInterceptor);
      this.endTime = new Date().getTime();
    })
  }

  // 默认设置
  defaultSetting() {
    this.clear()
    this.showUrl = this.postUrl
    this.startTime = new Date().getTime();

    axios.defaults.headers['customer-header'] = 'customer-value'
    axios.defaults.method = 'post'
    axios<InfoModel, AxiosResponse<InfoModel>, IdModel>({
      url: this.postUrl,
      data: {
        id: 590
      },
      connectTimeout: this.connectTimeout,
      readTimeout: this.readTimeout,
      maxBodyLength: this.maxBodyLength,
      maxContentLength: this.maxContentLength
    }).then((res: AxiosResponse<InfoModel>) => {
      this.status = res.status;
      this.message = JSON.stringify(res.data);
      axios.defaults.method = '';
      axios.defaults.headers['customer-header'] = null;
      this.endTime = new Date().getTime();
    }).catch((err: AxiosError) => {
      this.status = '';
      this.message = err.message;
      axios.defaults.method = '';
      axios.defaults.headers['customer-header'] = null
      this.endTime = new Date().getTime();
    })
  }

  // 下载
  download() {
    this.clear()
    this.showUrl = this.downloadUrl
    this.startTime = new Date().getTime();

    let filePath = getContext(this).cacheDir + '/blue.jpg'
    // 下载。如果文件已存在，则先删除文件。
    try {
      fs.accessSync(filePath);
      fs.unlinkSync(filePath);
    } catch (err) {
    }
    axios<string, AxiosResponse<string>, null>({
      url: this.downloadUrl,
      method: 'get',
      context: getContext(this),
      filePath: filePath,
      connectTimeout: this.connectTimeout,
      readTimeout: this.readTimeout,
      maxBodyLength: this.maxBodyLength,
      maxContentLength: this.maxContentLength,
      onDownloadProgress: (progressEvent: AxiosProgressEvent): void => {
        this.downloadProgress = progressEvent && progressEvent.loaded && progressEvent.total ? Math.ceil(progressEvent.loaded / progressEvent.total * 100) : 0;
      }
    }).then((res: AxiosResponse<string>) => {
      this.status = res ? res.status : '';
      this.message = res ? JSON.stringify(res.data) : '';
      this.endTime = new Date().getTime();
    }).catch((err: AxiosError) => {
      this.status = '';
      this.message = err.message;
      this.endTime = new Date().getTime();
    })
  }

  // 上传
  upload() {
    this.clear()
    this.showUrl = this.uploadUrl
    this.startTime = new Date().getTime();

    let context: Context = getContext(this);
    let ca: Uint8Array = context.resourceManager.getRawFileContentSync("hello.txt");
    let cacheDir: string = context.cacheDir;
    let buffer: ArrayBuffer = new ArrayBuffer(1024);
    try {
      writeFile(cacheDir, 'hello.txt', ca.buffer);
      // 读取
      buffer = readFile(cacheDir + '/hello.txt');
    } catch (err) {
      console.error(TAG, JSON.stringify(err))
    }

    let formData = new FormData();
    formData.append('file', buffer, { filename: 'text.txt', type: 'text/plain' });

    axios.post<UploadModel, AxiosResponse<UploadModel>, FormData>(this.uploadUrl, formData, {
      headers: { 'Content-Type': 'multipart/form-data' },
      context: getContext(this),
      onUploadProgress: (progressEvent: AxiosProgressEvent): void => {
        this.uploadProgress = progressEvent && progressEvent.loaded && progressEvent.total ? Math.ceil(progressEvent.loaded / progressEvent.total * 100) : 0;
      },
      connectTimeout: this.connectTimeout,
      readTimeout: this.readTimeout,
      maxContentLength: this.maxContentLength,
      maxBodyLength: this.maxBodyLength
    }).then((res: AxiosResponse<UploadModel>) => {
      this.status = res ? res.status : '';
      this.message = res ? JSON.stringify(res.data) : '';
      console.info(TAG, JSON.stringify(res.headers));
      this.endTime = new Date().getTime();
    }).catch((err: AxiosError) => {
      this.status = '';
      this.message = err.message;
      this.endTime = new Date().getTime();
    })
  }

  // 设置双向证书校验(无密码）
  settingClientCert() {
    this.clear()
    this.showUrl = this.clientCert_noPsw
    this.startTime = new Date().getTime();

    let path_ca = ''; // 根证书路径
    let path_client = ''; // 客户端证书路径
    let path_key = ''; // 客户端密码路径

    // 获取根证书、客户端证书、客户端密码沙箱路径
    try {
      let context: Context = getContext(this);
      let ca: Uint8Array = context.resourceManager.getRawFileContentSync("oneWayAuth/ca.p12");
      let client: Uint8Array = context.resourceManager.getRawFileContentSync("mutualAuth_noPsw/p12/client.p12");
      let key: Uint8Array = context.resourceManager.getRawFileContentSync("mutualAuth_noPsw/p12/client.key");
      let cacheDir: string = context.cacheDir;

      if (ca != null) {
        path_ca = cacheDir + "/ca.crt";
        writeFile(cacheDir, 'ca.crt', ca.buffer)
      }
      if (client != null) {
        path_client = cacheDir + "/client.p12";
        writeFile(cacheDir, 'client.p12', client.buffer)
      }
      if (key != null) {
        path_key = cacheDir + "/client.key";
        writeFile(cacheDir, 'client.key', key.buffer)
      }
    } catch (err) {
      console.info(TAG, JSON.stringify(err));
    }

    axios<InfoModel, AxiosResponse<InfoModel>, null>({
      url: this.clientCert_noPsw,
      method: 'get',
      caPath: path_ca,
      clientCert: {
        certPath: path_client,
        certType: 'p12',
        keyPath: path_key,
      },
      connectTimeout: this.connectTimeout,
      readTimeout: this.readTimeout,
      maxBodyLength: this.maxBodyLength,
      maxContentLength: this.maxContentLength
    }).then((res: AxiosResponse<InfoModel>) => {
      this.status = res ? res.status : '';
      this.message = res ? JSON.stringify(res.data) : '';
      this.endTime = new Date().getTime();
    }).catch((err: AxiosError) => {
      this.status = '';
      this.message = err.message;
      this.endTime = new Date().getTime();
    })
  }

  // 设置双向证书校验(有密码）
  settingClientCert_psw() {
    this.clear()
    this.showUrl = this.clientCert_hasPsw
    this.startTime = new Date().getTime();

    let path_ca = ''; // 根证书路径
    let path_client = ''; // 客户端证书路径
    let path_key = ''; // 客户端密码路径

    // 获取根证书、客户端证书、客户端密码沙箱路径
    try {
      let context: Context = getContext(this);
      let ca: Uint8Array = context.resourceManager.getRawFileContentSync("mutualAuth_hasPsw/ca.crt");
      let client: Uint8Array = context.resourceManager.getRawFileContentSync("mutualAuth_hasPsw/pem/client.pem");
      let key: Uint8Array = context.resourceManager.getRawFileContentSync("mutualAuth_hasPsw/pem/client.key");
      let cacheDir: string = context.cacheDir;

      if (ca != null) {
        path_ca = cacheDir + "/ca.crt";
        writeFile(cacheDir, 'ca.crt', ca.buffer)
      }
      if (client != null) {
        path_client = cacheDir + "/client.pem";
        writeFile(cacheDir, 'client.pem', client.buffer)
      }
      if (key != null) {
        path_key = cacheDir + "/client.key";
        writeFile(cacheDir, 'client.key', key.buffer)
      }
    } catch (err) {
      console.info(TAG, JSON.stringify(err));
    }

    axios<InfoModel, AxiosResponse<InfoModel>, null>({
      url: this.clientCert_hasPsw,
      method: 'get',
      caPath: path_ca,
      clientCert: {
        certPath: path_client,
        certType: 'pem',
        keyPath: path_key,
        keyPasswd: this.psw
      },
      connectTimeout: this.connectTimeout,
      readTimeout: this.readTimeout,
      maxBodyLength: this.maxBodyLength,
      maxContentLength: this.maxContentLength
    }).then((res: AxiosResponse<InfoModel>) => {
      this.status = res ? res.status : '';
      this.message = res ? JSON.stringify(res.data) : '';
      this.endTime = new Date().getTime();
    }).catch((err: AxiosError) => {
      this.status = '';
      this.message = err.message;
      this.endTime = new Date().getTime();
    })
  }

  // 设置响应类型
  settingResponseType() {
    this.clear()
    this.showUrl = this.getUrl
    this.startTime = new Date().getTime();

    axios<string, AxiosResponse<string>, null>({
      url: this.getUrl,
      method: 'get',
      responseType: 'array_buffer',
      connectTimeout: this.connectTimeout,
      readTimeout: this.readTimeout,
      maxBodyLength: this.maxBodyLength,
      maxContentLength: this.maxContentLength
    }).then((res: AxiosResponse) => {
      this.status = res ? res.status : '';
      if (res.data instanceof ArrayBuffer) {
        this.message = res ? 'responseType设置成功' : '';
      }
      this.endTime = new Date().getTime();
    }).catch((err: AxiosError) => {
      this.status = '';
      this.message = err.message;
      this.endTime = new Date().getTime();
    })
  }

  // 设置代理
  settingProxy() {
    this.clear()
    this.showUrl = this.proxyUrl
    this.startTime = new Date().getTime();

    axios<string, AxiosResponse<string>, null>({
      url: this.proxyUrl,
      method: 'get',
      proxy: {
        host: this.host,
        port: 6443,
        exclusionList: []
      },
      connectTimeout: this.connectTimeout,
      readTimeout: this.readTimeout,
      maxBodyLength: this.maxBodyLength,
      maxContentLength: this.maxContentLength
    }).then((res: AxiosResponse) => {
      this.status = res ? res.status : '';
      this.message = res ? JSON.stringify(res.data) : '';
      this.endTime = new Date().getTime();
    }).catch((err: AxiosError) => {
      this.status = '';
      this.message = err.message;
      this.endTime = new Date().getTime();
    })
  }

  clear() {
    this.performanceTiming = '';
    this.uploadProgress = 0;
    this.downloadProgress = 0
    this.message = '';
    this.status = '';
    this.startTime = 0;
    this.endTime = 0;
  }
}