#include "RNOH/Package.h"

namespace rnoh {
class SampleTurboModulePackage : public Package {
    public:
        SampleTurboModulePackage(Package::Context ctx) : Package(ctx) {}
    
        std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate() override;

    };
} // namespace rnoh
