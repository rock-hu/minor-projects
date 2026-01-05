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

export interface IdModel {
  id?: number
}

interface UrlModel {
  url: string
}

export interface UploadModel {
  code: number,
  msg: string,
  ts: string,
  data: UrlModel
}

export interface InfoModel {
  id: number
  name: string
  briefDesc: string
  author: string
  licence: string
  gitUrl: string
  keyword: string
}

export interface ConfigModel {
  defaultUrl: string,
  path: string,
  getUrl: string,
  postUrl: string,
  downloadUrl: string,
  uploadUrl: string,
}

export interface NormalResultModel {
  code: number,
  data: Object,
  msg: string,
  pageNum: number,
  pageSize: number,
  totalNum: number,
  totalPage: number
}

export interface PostDataModel {
  pageNum: number,
  pageSize: number,
  newDate: number
}

export interface DownLoadResultModel {
  data: string
}

export interface UploadResultModel {
  code: number,
  msg: string,
  ts: string,
  data: UrlModel
}

export interface Log {
  TAG: string,
  DOMAIN: number,
}

export interface Xts_config {
  path_oneWayAuth: string,
  path_mutualAuth_noPsw: string,
  path_mutualAuth_hasPsw: string,
  path_certificate_pin: string,
  defaultUrl: string,
  path: string,
  getUrl: string,
  postUrl: string,
  downloadUrl: string,
  uploadUrl: string,
  url: string,
  baseURL: string,
  host: string,
  proxyHttps: string,
  proxyHttp: string,
  psw: string
}

export interface Demo_config {
  baseUrl: string,
  getUrl: string,
  postUrl: string,
  putUrl: string,
  deleteUrl: string,
  downloadUrl: string,
  uploadUrl: string,
  clientCert_noPsw: string,
  clientCert_hasPsw: string,
  proxyUrl: string,
  host: string,
  psw: string,
}
