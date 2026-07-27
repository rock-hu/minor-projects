const DataModel = require("../model/DataModel");
const SUCCESS_CODE = 0;

/**
 * basCross-domain settings
 *
 * @param req request
 * @param res response
 */
const baseSetting = (req, res, next) => {
  var token = req.headers["authorization"];
  //设置允许跨域的域名，*代表允许任意域名跨域
  res.header("Access-Control-Allow-Origin", "*");
  //允许的header类型
  res.header("Access-Control-Allow-Headers", "*");
  //跨域允许的请求方式
  res.header(
    "Access-Control-Allow-Methods",
    "DELETE,PUT,POST,GET,OPTIONS,PATCH"
  );
  //让options请求快速返回
  if (req.method === "OPTIONS") res.send(200);
  // else if(req.originalUrl.indexOf('/tpc/') != -1){
  //   token ? next() : res.json(401, { error: 'token认证失败！' })
  // }
  else {
    next();
  }
};

/**
 * Get tpc list.
 *
 * @param req request
 * @param res response
 */
const getTpcList = (req, res) => {
  let result = DataModel.tpcList.map((item) => {
    let obj = {
      id: item["id"],
      name: item["name"],
      gitUrl: item["gitUrl"],
    };
    return obj;
  });
  res.send({ code: SUCCESS_CODE, data: result, msg: "查询成功" });
};

/**
 * patch tpc info.
 *
 * @param req request
 * @param res response
 */
const patchTpcInfo = (req, res) => {
  let tpcList = JSON.parse(JSON.stringify(DataModel.tpcList));
  let requestBody = req.body;

  if (!(requestBody instanceof Object) || !requestBody.id) {
    res.json(500, { error: "参数错误！" });
  }

  let result = tpcList.filter((item) => {
    return item.id == requestBody.id;
  });

  delete requestBody["id"];
  let bodyKeys = Object.keys(requestBody);

  if (result.length > 0 && bodyKeys.length > 0) {
    let obj = result[0];
    bodyKeys.forEach((item) => {
      if (obj[item]) {
        obj[item] = requestBody[item];
      }
    });
    res.send({ code: SUCCESS_CODE, data: obj, msg: "修改成功" });
  } else {
    res.send({ code: SUCCESS_CODE, data: [], msg: "修改成功" });
  }
};

/**
 * search tpc detail.
 *
 * @param req request
 * @param res response
 */
const postTpcDetail = (req, res) => {
  if (req.body.id) {
    let result = DataModel.tpcList.filter((item) => {
      return item.id == req.body.id;
    });
    res.send({ code: SUCCESS_CODE, data: result, msg: "提交成功" });
  } else {
    res.json(500, { error: "参数错误！" });
  }
};

/**
 * put tpc list.
 *
 * @param req request
 * @param res response
 */
const putTpcInfo = (req, res) => {
  let tpcList = JSON.parse(JSON.stringify(DataModel.tpcList));
  let requestBody = req.body;

  if (!(requestBody instanceof Object) || !requestBody.id) {
    res.json(500, { error: "参数错误！" });
  }

  let result = tpcList.filter((item) => {
    return item.id == requestBody.id;
  });

  delete requestBody["id"];
  let bodyKeys = Object.keys(requestBody);

  if (result.length > 0 && bodyKeys.length > 0) {
    let obj = result[0];
    bodyKeys.forEach((item) => {
      if (obj[item]) {
        obj[item] = requestBody[item];
      }
    });
    res.send({ code: SUCCESS_CODE, data: obj, msg: "修改成功" });
  } else {
    res.send({ code: SUCCESS_CODE, data: [], msg: "修改成功" });
  }
};

/**
 * delete tpc item.
 *
 * @param req request
 * @param res response
 */
const deleteTpcInfo = (req, res) => {
  if (req.body.id) {
    res.send({
      code: SUCCESS_CODE,
      data: `id为${req.body.id}的数据删除成功！`,
      msg: "操作成功",
    });
  } else {
    res.json(500, { error: "参数错误！" });
  }
};

/**
 * A request is returned after a delay of 10s.
 *
 * @param req request
 * @param res response
 */
const getDelay = (req, res) => {
  setTimeout(() => {
    res.send({
      code: SUCCESS_CODE,
      data: `id为${req.body.id}的数据删除成功！`,
      msg: "延迟10s后返回的数据。",
    });
  }, 10000);
};

/**
 * A redirect request.
 *
 * @param req request
 * @param res response
 */
const getRedirect = (req, res) => {
  res.writeHead(302, { Location: "/tpc/redirect2" });
  res.end();
};

/**
 * Second redirect.
 *
 * @param req request
 * @param res response
 */
const getRedirect2 = (req, res) => {
  res.writeHead(302, {
    Location: "https://www.openharmony.cn",
  });
  res.end();
};

module.exports = {
  baseSetting,
  getTpcList,
  patchTpcInfo,
  postTpcDetail,
  putTpcInfo,
  deleteTpcInfo,
  getDelay,
  getRedirect,
  getRedirect2,
};
