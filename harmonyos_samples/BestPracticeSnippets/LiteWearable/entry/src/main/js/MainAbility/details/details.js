// [Start details_js]
// details.js
import router from '@ohos.router';

export default {
    clickAction() {
        this.getUIContext().getRouter().replaceUrl({
            uri: 'pages/index/index'
        });
    }
};
// [End details_js]