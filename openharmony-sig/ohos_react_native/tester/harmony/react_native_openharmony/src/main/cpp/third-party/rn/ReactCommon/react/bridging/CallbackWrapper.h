/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
// RNOH patch — apply fix from https://github.com/facebook/react-native/pull/43410/files

#pragma once

#include <jsi/jsi.h>
#include "LongLivedObject.h"
#include <ReactCommon/CallInvoker.h> // RNOH: patch

#include <memory>

namespace facebook {
namespace react {

// Helper for passing jsi::Function arg to other methods.
class CallbackWrapper : public LongLivedObject {
 private:
  CallbackWrapper(
      jsi::Function &&callback,
      jsi::Runtime &runtime,
      std::shared_ptr<CallInvoker> jsInvoker)
        : LongLivedObject(runtime),
        callback_(std::move(callback)),
        jsInvoker_(std::move(jsInvoker)) {}

  jsi::Function callback_;
  std::shared_ptr<CallInvoker> jsInvoker_;

 public:
  static std::weak_ptr<CallbackWrapper> createWeak(
      jsi::Function &&callback,
      jsi::Runtime &runtime,
      std::shared_ptr<CallInvoker> jsInvoker) {
    auto wrapper = std::shared_ptr<CallbackWrapper>(new CallbackWrapper(
        std::move(callback), runtime, std::move(jsInvoker)));
    LongLivedObjectCollection::get(runtime).add(wrapper);
    return wrapper;
  }

  // Delete the enclosed jsi::Function
  void destroy() {
    allowRelease();
  }

  jsi::Function &callback() {
    return callback_;
  }

  jsi::Runtime &runtime() {
    return runtime_;
  }

  CallInvoker &jsInvoker() {
    return *(jsInvoker_);
  }

  std::shared_ptr<CallInvoker> jsInvokerPtr() {
    return jsInvoker_;
  }
};

} // namespace react
} // namespace facebook
