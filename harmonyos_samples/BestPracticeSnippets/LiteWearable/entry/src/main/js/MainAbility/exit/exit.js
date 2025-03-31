// [Start 1]
// index.js
import router from '@ohos.router';
// 导入app模块  
import app from '@system.app';

export default {
    data: {
        title: 'World'
    },
    clickAction() {
        router.replaceUrl({
            uri: 'pages/details/details'
        });
    },
    touchMove(e) { // swipe处理事件
        if (e.direction == "right") { // 右滑退出
            app.terminate();
        }
    }
};
// [End 1]