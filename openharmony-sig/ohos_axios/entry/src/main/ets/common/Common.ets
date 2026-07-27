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
import { Log, Xts_config, Demo_config } from '../types/types'

const LOG: Log = {
  TAG: '[tpc].[axios]',
  DOMAIN: 0x0001
}
// xts请求服务器url
const XTS_CONFIG: Xts_config = {
  path_oneWayAuth: '', // 单向校验请求路径
  path_mutualAuth_noPsw: '', // 双向校验无密码请求路径
  path_mutualAuth_hasPsw: '', // 双向校验有密码请求路径
  path_certificate_pin: '', //证书锁定的请求路径
  defaultUrl: '',
  path: '',
  getUrl: '', // get请求地址
  postUrl: '', // post请求地址
  downloadUrl: '', // 下载地址
  uploadUrl: '', // 上传路径
  url: '',
  baseURL: '',
  host: '', //代理host
  proxyHttps: '', //https代理请求地址
  proxyHttp: '', //http代理请求地址
  psw: '', //证书校验密码
}
// demo请求服务器url
const DEMO_CONFIG: Demo_config = {
  baseUrl: 'http://localhost:4000', // 基础请求地址
  getUrl: '/tpc/get', // get请求地址
  postUrl: '/tpc/post', // post请求地址
  putUrl: '/tpc/put',// put请求地址
  deleteUrl: '/tpc/delete',// delete请求地址
  downloadUrl: '/tpc/download/blue.jpg', // 下载地址
  uploadUrl: '/tpc/upload', // 上传地址
  clientCert_noPsw: '', // 双向校验无密码请求地址
  clientCert_hasPsw: '', // 双向校验有密码请求地址
  proxyUrl: '', // https代理请求地址
  host: '', //代理host
  psw: '', //证书校验密码
}

export { LOG, XTS_CONFIG, DEMO_CONFIG }