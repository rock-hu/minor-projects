#include "FabricComponentSamplePackagePackage.h"
#include "RNOH/RNInstanceCAPI.h"
#include "RNOH/generated/BaseFabricComponentSamplePackagePackage.h"
#include "RNOHCorePackage/ComponentBinders/ViewComponentJSIBinder.h"
#include "RNOHCorePackage/ComponentBinders/ViewComponentNapiBinder.h"
#include "QDGestureFloatComponentInstance.h"

using namespace facebook;

namespace rnoh {
ComponentInstance::Shared FabricComponentSamplePackagePackage::createComponentInstance(
      const ComponentInstance::Context& ctx) {
  if (ctx.componentName == "QDGestureFloat") {
    return std::make_shared<QDGestureFloatComponentInstance>(std::move(ctx));
  }
  return nullptr;
};
} // namespace rnoh
