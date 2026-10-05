/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOH/BlobCollector.h"
#include "RNOH/GlobalJSIBinder.h"
#include "RNOH/TurboModuleProvider.h"

namespace rnoh {
class BlobCollectorJSIBinder : public GlobalJSIBinder {
 public:
  BlobCollectorJSIBinder(GlobalJSIBinder::Context ctx) : GlobalJSIBinder(ctx) {}
  void createBindings(
      facebook::jsi::Runtime& rt,
      std::shared_ptr<TurboModuleProvider> provider) override {
    BlobCollector::install(rt, *(provider->getTurboModule("BlobModule")));
  }
};
} // namespace rnoh