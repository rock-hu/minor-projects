// [Start details_js]
// details.js
export default {
    clickAction() {
        this.getUIContext().getRouter().replaceUrl({
            uri: 'pages/index/index'
        });
    }
};
// [End details_js]