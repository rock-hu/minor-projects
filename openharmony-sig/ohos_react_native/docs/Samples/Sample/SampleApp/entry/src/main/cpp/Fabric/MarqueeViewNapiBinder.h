
#pragma once
//#include "MarqueeViewComponentDescriptor.h"
#include "RNOHCorePackage/ComponentBinders/ViewComponentNapiBinder.h"
#include "Props.h"

namespace rnoh {

    class MarqueeViewNapiBinder : public ViewComponentNapiBinder {
    public:
        napi_value createProps(napi_env env, facebook::react::ShadowView const shadowView) override {
            napi_value napiViewProps = ViewComponentNapiBinder::createProps(env, shadowView);
            if (auto props = std::dynamic_pointer_cast<const facebook::react::MarqueeViewProps>(shadowView.props)) {
                return ArkJS(env)
                    .getObjectBuilder(napiViewProps)
                    .addProperty("src", props->src)  // descriptor.props 添加键值
                    .build();
            }
            return napiViewProps;
        };
    };

} 