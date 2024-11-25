#include "RNOH/Package.h"
#include "RNOH/generated/BaseFabricComponentSamplePackagePackage.h"

namespace rnoh {
class FabricComponentSamplePackagePackage : public BaseFabricComponentSamplePackagePackage {
  using Super = BaseFabricComponentSamplePackagePackage;

 public:
  FabricComponentSamplePackagePackage(Package::Context ctx)
      : BaseFabricComponentSamplePackagePackage(ctx) {}

  ComponentInstance::Shared createComponentInstance(
      const ComponentInstance::Context& ctx) override;
};
} // namespace rnoh
