# node service


## 运行示例

> 安装node

> 安装依赖

```bash
npm i
```

> 运行服务器

```bash
npm run start
```

## 访问主页
在chrome浏览器中输入`http://localhost:4000`访问主页。

> 注意：现已移除`blue.jpg`文件，请自行将任意文件拷贝至`public`文件夹下，进行测试。

> `http://localhost:4000/tpc/get`发送get请求。

> `http://localhost:4000/tpc/patch`发送patch请求。

> `http://localhost:4000/tpc/post`发送post请求。

> `http://localhost:4000/tpc/put`发送put请求。

> `http://localhost:4000/tpc/delete`发送delete请求。

> `http://localhost:4000/tpc/getDelay`发送一个8s后返回数据的请求。

> `http://localhost:4000/tpc/redirect`发送重定向的请求。

> `http://localhost:4000/tpc/upload`发送上传请求。

> `http://localhost:4000/tpc/download`发送下载请求。请将下载的文件放在upload目录下。
