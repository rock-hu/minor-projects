// [Start detail_js]
// index.js
import router from '@ohos.router';

export default {
    clickAction() {
        this.getUIContext().getRouter().replaceUrl({
            uri: 'pages/details/details'
        });
    }
};
// [End detail_js]