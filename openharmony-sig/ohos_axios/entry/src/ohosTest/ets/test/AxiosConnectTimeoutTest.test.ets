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
import { describe, beforeAll, it, expect } from '@ohos/hypium'
import axios, { AxiosError, AxiosHeaders } from '@ohos/axios';
import { AxiosResponse, InternalAxiosRequestConfig } from '@ohos/axios'
import {
  NormalResultModel,
  PostDataModel,
} from '../../../main/ets/types/types'
import hilog from '@ohos.hilog';
import { LOG, XTS_CONFIG } from '../../../main/ets/common/Common'

export default function DiskLruCacheTest() {

  describe('AxiosConnectTimeoutTest', () => {
    const BASE_COUNT = 200
    const HTTP_COUNT = 2
    const BASELINE_REQUEST = 2500
    const TAG = LOG.TAG
    const DOMAIN = LOG.DOMAIN
    const config = XTS_CONFIG
    const connectTimeout = 5000;

    // creat get method
    it('creat_get', 0, async (done: Function) => {
      let startTime = new Date().getTime()
      const instance = axios.create();
      let configUrlTest: (index: number) => void = async (index) => {
        instance.get<NormalResultModel, AxiosResponse<NormalResultModel>, null>(config.getUrl, {
          connectTimeout: connectTimeout
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
    it('request_get', 0, async (done: Function) => {
      let startTime = new Date().getTime()
      let configUrlTest: (index: number) => void = async (index) => {
        axios.request<NormalResultModel, AxiosResponse<NormalResultModel>, null>({
          url:config.getUrl,
          method: 'get',
          connectTimeout: connectTimeout
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
    it('get', 0, async (done: Function) => {
      let startTime = new Date().getTime()
      let configUrlTest: (index: number) => void = async (index) => {
        axios.get<NormalResultModel, AxiosResponse<NormalResultModel>, null>(config.getUrl, {
          connectTimeout: connectTimeout
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
    it('post', 1, async (done: Function) => {
      let startTime = new Date().getTime()
      let data: PostDataModel = {
        pageNum: 1,
        pageSize: 4,
        newDate: 1
      }
      let configUrlTest: (index: number) => void = async (index) => {
        axios.post<NormalResultModel, AxiosResponse<NormalResultModel>, PostDataModel>(config.postUrl, data,{
          connectTimeout: connectTimeout
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
    it('defaultSetting', 5, async (done: Function) => {
      axios.defaults.baseURL = config.defaultUrl
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
          connectTimeout: connectTimeout
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
    it('interceptors', 6, async (done: Function) => {
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
        axios<string, AxiosResponse<string>, null>({
          method: 'get',
          url: config.getUrl,
          connectTimeout: connectTimeout
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
    it('create_100', 7, async (done: Function) => {
      let startTime = new Date().getTime()
      for (let i = 0; i < BASE_COUNT; i++) {
        axios.create({})
      }
      let endTime = new Date().getTime()
      let averageTime = (endTime - startTime) * 1000 / BASE_COUNT
      hilog.info(DOMAIN, TAG, " create instance averageTime: " + averageTime + ' μs');
      expect(averageTime < BASELINE_REQUEST)
        .assertTrue()
      done()
    })

    // create instance
    it('formData', 8, async (done: Function) => {
      let formData = new Map([
        ['sign', 'tes']
      ])
      let startTime = new Date().getTime()
      axios.defaults.baseURL = XTS_CONFIG.baseURL
      axios.post(config.postUrl, formData, {
        url: config.getUrl,
        headers: {
          'Content-Type': 'multipart/form-data'
        },
        connectTimeout: connectTimeout
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

    // request header with number
    it('header_number', 9, async (done: Function) => {
      let header = new AxiosHeaders()
      let uuid = 'FDHSAJKHFSAJHFDSKAL'
      let val = 'value'
      let startTime = new Date().getTime()
      header.set('X-B3', uuid)
      header.set('hello', val)
      let headerObj = AxiosHeaders.from(header)
      let endTime = new Date().getTime()
      let averageTime = (endTime - startTime) * 1000 / HTTP_COUNT
      hilog.info(DOMAIN, TAG, " header_number averageTime: " + averageTime + ' μs');
      expect(headerObj['X-B3']).assertEqual(uuid)
      expect(headerObj['hello']).assertEqual(val)
      done()
    })
  })
}
