/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <jsi/jsi.h>
#include <string>
#include "ReactCommon/TurboModule.h"

namespace rnoh {

class BlobCollector : public facebook::jsi::HostObject {
 public:
  BlobCollector(
      facebook::react::TurboModule& turboModule,
      const std::string& blobId);
  ~BlobCollector();

  static void install(
      facebook::jsi::Runtime& rt,
      facebook::react::TurboModule& turboModule);

 private:
  const std::string blobId_;
  facebook::react::TurboModule& turboModule_;
};

} // namespace rnoh