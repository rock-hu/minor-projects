const fs = require("fs");
const path = require("path");
const { resolve } = require("path");
const PUBLIC_PATH = path.resolve(__dirname, "../public");
const zlib = require("zlib");

var multer = require("multer");
var storage = multer.diskStorage({

  //保存路径
  destination: function (req, file, cb) {
    cb(null, "./public/upload");
  },
  //保存在 destination 中的文件名
  filename: function (req, file, cb) {
    const originalName = file.originalname ? file.originalname : Date.now() + ''
    const filename = req.body.name ?  req.body.name : originalName;
    cb(null, filename)
  }
});
var uploads = multer({ storage: storage });
var upload_middle = uploads.array("file", 3);
const SUCCESS_CODE = 0;

/**
 * delete tpc item.
 *
 * @param req request
 * @param res response
 */
const deleteFile = (req, res) => {
  let file = req.query.path;
  //删除 static 目录下的该文件
  let uploadPath = resolve(`./public/upload/${file}`);
  try {
    fs.unlinkSync(uploadPath);
    res.send({
      code: SUCCESS_CODE,
      msg: "删除文件成功",
      data: {
        url: `${__dirname}/${uploadPath}`,
      },
    });
  } catch (err) {
    res.json(500, { error: "删除失败" });
  }
};

/**
 * upload file.
 *
 * @param req request
 * @param res response
 */
const uploadFile = (req, res) => {
  res.send({
    code: SUCCESS_CODE,
    msg: "success",
    data: {
      url: `http://139.9.199.99:4000/${req.body.name}`,
    },
  });
};

/**
 * upload mul file.
 *
 * @param req request
 * @param res response
 */
const uploadMulFile = (req, res) => {
  upload_middle(req, res, function (err) {
    if (err instanceof multer.MulterError) {
      // 发生错误
      res.json(500, { error: "上传失败，文件最多上传三个" });
    } else {
      res.send({
        code: SUCCESS_CODE,
        msg: "success",
        data: {
          url: `http://139.9.199.99:4000/${req.body.name}`,
        },
      });
    }
  });
};

/**
 * uploadBlob Blob file.
 *
 * @param req request
 * @param res response
 */
const uploadFileBlob = (req, res) => {
  let headersOption = req.headers
  let fileName = JSON.parse(headersOption['Dropbox-API-Arg'] || headersOption['dropbox-api-arg']).path
  const writeStream = fs.createWriteStream('./public/upload/' + fileName)
  writeStream.on("error", (err)=>{
    console.info('写入流出错', err)
  })

  writeStream.on("open", ()=>{
    writeStream.write(Buffer.from(req.body));
    writeStream.end()
  })

  writeStream.on("finish", ()=>{
    console.info('图像写入成功')
    res.send({
      code: SUCCESS_CODE,
      msg: "success",
      data: {
        url: `http://139.9.199.99:3000/${'./public/upload' + fileName}`,
      }
    });
  });
};


/**
 * uploadBlobs Blob file.
 *
 * @param req request
 * @param res response
 */
const uploadFileBlobs = (req, res) => {
  res.send({
    code: SUCCESS_CODE,
    msg: "success",
    data: {
      url: `http://139.9.199.99:3000/${'./public/upload' + fileName}`,
    }
  });
};


/**
 * download file.
 *
 * @param req request
 * @param res response
 */
const downloadFile = (req, res) => {
  // 获取参数
  let filePath = req.params.path;
  // 拼接文件的绝对路径
  let downloadPath = path.resolve(PUBLIC_PATH, filePath);

  // 得到文件名
  let { base: fileName } = path.parse(downloadPath);

  // 文件的信息
  let stat;
  try {
    // 通过fs模块获取文件的信息，报错就代表着不存在该文件
    stat = fs.statSync(downloadPath);
    res.setHeader("content-length", stat.size);
  } catch (error) {
    // 不存在该文件
    return res.json(500, { error: "文件不存在，下载失败" });
  }

  // 获取文件的大小
  let { size: total } = stat;
  // 设置响应头信息，指示内容应该被下载
  res.setHeader("Access-Control-Expose-Headers", "Content-Disposition");
  res.setHeader(
    "Content-Disposition",
    "attachment; filename=" + encodeURIComponent(fileName)
  );

  // 说明了实体主体内对象的媒体类型，这是一个二进制文件类型
  res.setHeader("Content-Type", "application/octet-stream");

  // 响应状态为200
  res.status(200);
  res.send({
    code: SUCCESS_CODE,
    msg: req.originalUrl + "下载完成",
  });

  // 通过fs模块创建二进制流，并将其二进制流推送到响应主体中
  fs.createReadStream(downloadPath).pipe(res);
};

/**
 * home page.
 *
 * @param req request
 * @param res response
 */
const getHomePage = (req, res) => {
  res.setHeader("Access-Control-Allow-Origin", "*");
  res.setHeader(
    "Access-Control-Allow-Methods",
    "POST, GET, OPTIONS, DELETE, PUT"
  );
  // 允许跨域请求header携带哪些东西
  res.header(
    "Access-Control-Allow-Headers",
    "Origin, X-Requested-With, Content-Type, Accept, If-Modified-Since, Access-Token"
  );
  res.render("index", { title: "Express" });
};

/**
 * Get a list of uploaded files.
 *
 * @param req request
 * @param res response
 */
const getFileList = (req, res) => {
  let uploadPath = resolve("./public/upload/");
  let files = fs.readdirSync(uploadPath);
  let result = files.map((file) => {
    let fullPath = path.join(uploadPath, "" + file);
    let stat = fs.lstatSync(fullPath);
    var atimeMs = timestampToTime(stat.atimeMs);
    return {
      name: file,
      size: stat.size,
      uploadTime: atimeMs,
      link: `${file}`,
    };
  });
  res.send(result);
};

/**
 * Timestamps are converted to time.
 *
 * @param timestamp Timestamps
 */
const timestampToTime = (timestamp) => {
  timestamp = timestamp ? timestamp : null;
  let date = new Date(timestamp); //时间戳为10位需*1000，时间戳为13位的话不需乘1000
  let Y = date.getFullYear() + "-";
  let M =
    (date.getMonth() + 1 < 10
      ? "0" + (date.getMonth() + 1)
      : date.getMonth() + 1) + "-";
  let D = (date.getDate() < 10 ? "0" + date.getDate() : date.getDate()) + " ";
  let h =
    (date.getHours() < 10 ? "0" + date.getHours() : date.getHours()) + ":";
  let m =
    (date.getMinutes() < 10 ? "0" + date.getMinutes() : date.getMinutes()) +
    ":";
  let s = date.getSeconds() < 10 ? "0" + date.getSeconds() : date.getSeconds();
  return Y + M + D + h + m + s;
};
/**
 * upload file.
 *
 * @param req request
 * @param res response
 */
const gzipHeader = (req, res) => {
  let data = {
    url: "hello world",
    name: "上传文件",
  };
  let re = zlib.gzip(data);
  res.send({
    code: SUCCESS_CODE,
    msg: "上传文件成功",
    data: re,
  });
};

module.exports = {
  deleteFile,
  uploadFile,
  uploadMulFile,
  uploadFileBlob,
  uploadFileBlobs,
  downloadFile,
  getHomePage,
  getFileList,
  gzipHeader,
};
