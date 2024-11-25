#pragma once
#include "RNOH/generated/BaseReactNativeHarmonySamplePackage2Package.h"

namespace rnoh {

class CodegenLibSamplePackage
    : public BaseReactNativeHarmonySamplePackage2Package {
  using Super = BaseReactNativeHarmonySamplePackage2Package;

 public:
  CodegenLibSamplePackage(Package::Context ctx) : Super(ctx) {}

  ComponentInstance::Shared createComponentInstance(
      const ComponentInstance::Context& ctx) {
    return nullptr;
  };
};
} // namespace rnoh
