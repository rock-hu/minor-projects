// [Start detail_js]
// index.js
export default {
    clickAction() {
        this.getUIContext().getRouter().replaceUrl({
            uri: 'pages/details/details'
        });
    }
};
// [End detail_js]