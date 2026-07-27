/*
 * The MIT License (MIT)
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
import { describe, beforeAll, it, expect } from '@ohos/hypium'
import axios, { AxiosError, AxiosHeaders } from '@ohos/axios';
import { AxiosResponse, InternalAxiosRequestConfig } from '@ohos/axios'
import { NormalResultModel, PostDataModel, } from '../../../main/ets/types/types'
import hilog from '@ohos.hilog';
import { LOG, XTS_CONFIG } from '../../../main/ets/common/Common'

export default function DiskLruCacheTest() {

  describe('AxiosReadTimeoutTest', () => {
    const HTTP_COUNT = 2
    const TAG = LOG.TAG
    const DOMAIN = LOG.DOMAIN
    const config = XTS_CONFIG
    const readTimeout = 5000;

    // creat get method
    it('creat_get', 0, async (done: Function) => {
      let startTime = new Date().getTime()
      const instance = axios.create();
      let configUrlTest: (index: number) => void = async (index) => {
        instance.defaults.baseURL = config.baseURL
        instance.get<NormalResultModel, AxiosResponse<NormalResultModel>, null>(config.getUrl, {
          readTimeout: readTimeout
        }).then((res: AxiosResponse<NormalResultModel>) => {
          if (index < HTTP_COUNT) {
            configUrlTest(index + 1)
          } else {
            let endTime = new Date().getTime()
            let averageTime = (endTime - startTime) * 1000 / HTTP_COUNT
            hilog.info(DOMAIN, TAG, " get method averageTime: " + averageTime + ' μs');
            expect(res.data.msg).assertEqual('查询成功')
            done()
          }
        }).catch(() => {
          expect().assertFail()
        })
      }
      configUrlTest(0)
    })
    // request get method
    it('request_get', 1, async (done: Function) => {
      let startTime = new Date().getTime()
      let configUrlTest: (index: number) => void = async (index) => {
        axios.defaults.baseURL = config.baseURL;
        axios.request<NormalResultModel, AxiosResponse<NormalResultModel>, null>({
          url: config.getUrl,
          method: 'get',
          readTimeout: readTimeout
        }).then((res: AxiosResponse<NormalResultModel>) => {
          if (index < HTTP_COUNT) {
            configUrlTest(index + 1)
          } else {
            let endTime = new Date().getTime()
            let averageTime = (endTime - startTime) * 1000 / HTTP_COUNT
            hilog.info(DOMAIN, TAG, " get method averageTime: " + averageTime + ' μs');
            expect(res.data.msg).assertEqual('查询成功')
            done()
          }
        }).catch(() => {
          expect().assertFail()
        })
      }
      configUrlTest(0)
    })
    //get method
    it('get', 2, async (done: Function) => {
      let startTime = new Date().getTime()
      let configUrlTest: (index: number) => void = async (index) => {
        axios.defaults.baseURL = config.baseURL;
        axios.get<NormalResultModel, AxiosResponse<NormalResultModel>, null>(config.getUrl, {
          readTimeout: readTimeout
        }).then((res: AxiosResponse<NormalResultModel>) => {
          if (index < HTTP_COUNT) {
            configUrlTest(index + 1)
          } else {
            let endTime = new Date().getTime()
            let averageTime = (endTime - startTime) * 1000 / HTTP_COUNT
            hilog.info(DOMAIN, TAG, " get method averageTime: " + averageTime + ' μs');
            expect(res.data.msg).assertEqual('查询成功')
            done()
          }
        }).catch(() => {
          expect().assertFail()
        })
      }
      configUrlTest(0)
    })

    // post method
    it('post', 3, async (done: Function) => {
      let startTime = new Date().getTime()
      let data: PostDataModel = {
        pageNum: 1,
        pageSize: 4,
        newDate: 1
      }
      let configUrlTest: (index: number) => void = async (index) => {
        axios.defaults.baseURL = config.baseURL;
        axios.post<NormalResultModel, AxiosResponse<NormalResultModel>, PostDataModel>(config.postUrl, data, {
          readTimeout: readTimeout
        }).then((res: AxiosResponse<NormalResultModel>) => {
          if (index < HTTP_COUNT) {
            configUrlTest(index + 1)
          } else {
            let endTime = new Date().getTime()
            let averageTime = (endTime - startTime) * 1000 / HTTP_COUNT
            hilog.info(DOMAIN, TAG, " post method averageTime: " + averageTime + ' μs');
            expect(res.data.msg).assertEqual('成功')
            done()
          }
        }).catch(() => {
          expect().assertFail()
        })
      }
      configUrlTest(0)
    })

    // defaultSetting
    it('defaultSetting', 4, async (done: Function) => {
      axios.defaults.baseURL = config.baseURL;
      axios.defaults.headers['customer-header'] = 'customer-value'
      axios.defaults.method = 'post'
      let data: PostDataModel = {
        pageNum: 1,
        pageSize: 4,
        newDate: 1
      }
      let configUrlTest: (index: number) => void = async (index) => {
        axios<NormalResultModel, AxiosResponse<NormalResultModel>, PostDataModel>({
          url: config.path,
          data: data,
          readTimeout: readTimeout
        }).then((res: AxiosResponse<NormalResultModel>) => {
          if (index < HTTP_COUNT) {
            configUrlTest(index + 1)
          } else {
            let endTime = new Date().getTime()
            let averageTime = (endTime - startTime) * 1000 / HTTP_COUNT
            hilog.info(DOMAIN, TAG, " defaultSetting averageTime: " + averageTime + ' μs');
            expect(res.data.msg === '成功')
              .assertTrue()
            done()
          }
        }).catch(() => {
          expect().assertFail()
        })
      }
      let startTime = new Date().getTime()
      configUrlTest(0)
    })

    // interceptors
    it('interceptors', 5, async (done: Function) => {
      let startTime = new Date().getTime()
      const myRequestInterceptor = axios.interceptors.request.use((config: InternalAxiosRequestConfig<null>) => {
        expect(config.baseURL === config.baseURL).assertTrue()
        return config;
      }, (error: AxiosError) => {
        return Promise.reject(error);
      })
      // response interceptors
      const myResponseInterceptor = axios.interceptors.response.use((response: AxiosResponse) => {
        response.data = 'hello world'
        return response;
      }, (error: AxiosError) => {
        return Promise.reject(error);
      })

      let configUrlTest: (index: number) => void = async (index) => {
        axios.defaults.baseURL = config.baseURL;
        axios<string, AxiosResponse<string>, null>({
          method: 'get',
          url: config.getUrl,
          readTimeout: readTimeout
        }).then((res: AxiosResponse<string>) => {
          if (index < HTTP_COUNT) {
            configUrlTest(index + 1)
          } else {
            let endTime = new Date().getTime()
            let averageTime = (endTime - startTime) * 1000 / HTTP_COUNT
            hilog.info(DOMAIN, TAG, " interceptors averageTime: " + averageTime + ' μs');
            expect(res.data === 'hello world')
              .assertTrue()
            done()
            axios.interceptors.request.eject(myRequestInterceptor);
            axios.interceptors.response.eject(myResponseInterceptor);
          }
        }).catch(() => {
          expect().assertFail()
        })
      }
      configUrlTest(0)
    })

    // create instance
    it('formData', 6, async (done: Function) => {
      let formData = new Map([
        ['sign', 'tes']
      ])
      let startTime = new Date().getTime()
      axios.defaults.baseURL = config.baseURL;
      axios.post(config.postUrl, formData, {
        url: config.getUrl,
        headers: {
          'Content-Type': 'multipart/form-data'
        },
        readTimeout: readTimeout
      }).then((res: AxiosResponse<string>) => {
        let endTime = new Date().getTime()
        let averageTime = (endTime - startTime) * 1000 / HTTP_COUNT
        hilog.info(DOMAIN, TAG, " formData averageTime: " + averageTime + ' μs');
        expect(res.status === 200)
          .assertTrue()
        done()
      }).catch(() => {
        done()
      })
    })

  })
}
