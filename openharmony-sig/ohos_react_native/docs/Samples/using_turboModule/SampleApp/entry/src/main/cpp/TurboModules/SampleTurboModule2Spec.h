// NOTE: This entire file should be codegen'ed.
#pragma once

#include <ReactCommon/TurboModule.h>
#include "RNOH/ArkTSTurboModule.h"
#include "RNOH/RNInstanceCAPI.h"

namespace rnoh {

    class JSI_EXPORT NativeSampleTurboModule2SpecJSI : public ArkTSTurboModule, public std::enable_shared_from_this<NativeSampleTurboModule2SpecJSI> {
    public:
        NativeSampleTurboModule2SpecJSI(
            const ArkTSTurboModule::Context ctx,
            const std::string name);
        void callJS();
        };

} // namespace rnoh
