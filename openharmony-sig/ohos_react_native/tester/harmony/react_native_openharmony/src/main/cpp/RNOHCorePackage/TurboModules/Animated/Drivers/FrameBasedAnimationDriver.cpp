/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "FrameBasedAnimationDriver.h"

#include <glog/logging.h>

namespace rnoh {

// 60 FPS is hardcoded in React Native
uint64_t FRAME_TIME_MILLIS = 1000.0 / 60.0;

FrameBasedAnimationDriver::FrameBasedAnimationDriver(
    facebook::react::Tag animationId,
    facebook::react::Tag animatedNode,
    AnimatedNodesManager& nodesManager,
    folly::dynamic const& config,
    AnimationEndCallback&& endCallback)
    : AnimationDriver(
          animationId,
          animatedNode,
          nodesManager,
          std::move(endCallback)) {
  resetConfig(config);
}

void FrameBasedAnimationDriver::resetConfig(folly::dynamic const& config) {
  auto frames = config["frames"];
  auto numberOfFrames = frames.size();
  m_frames.resize(numberOfFrames);
  for (auto i = 0; i < numberOfFrames; i++) {
    m_frames[i] = frames[i].asDouble();
  }
  m_toValue = config.getDefault("toValue", 0.0).asDouble();
  m_iterations = config.getDefault("iterations", 1).asInt();
  m_currentLoop = 1;
  m_startTimeNanos = -1;
  m_hasFinished = m_iterations == 0;
}

void FrameBasedAnimationDriver::runAnimationStep(uint64_t frameTimeNanos) {
  if (m_hasFinished) {
    return;
  }
  try {
    auto& animatedValue = getAnimatedValue();

    if (m_startTimeNanos < 0) {
      m_startTimeNanos = frameTimeNanos;
      if (m_currentLoop == 1) {
        m_fromValue = animatedValue.getValueAsDouble();
      }
    }

    auto timeFromStart = frameTimeNanos - m_startTimeNanos;
    uint64_t timeFromStartMillis = timeFromStart / 1e6;
    auto frameIndex =
        static_cast<uint64_t>((timeFromStartMillis) / FRAME_TIME_MILLIS);

    double nextValue;
    if (frameIndex >= m_frames.size() - 1) {
      nextValue = m_toValue;
      if (m_iterations == -1 || m_currentLoop < m_iterations) {
        m_startTimeNanos = -1;
        m_currentLoop++;
      } else {
        m_hasFinished = true;
      }
    } else {
      nextValue =
          m_fromValue + (m_toValue - m_fromValue) * m_frames[frameIndex];
    }
    animatedValue.setValue(nextValue);
  } catch (std::out_of_range& _e) {
    // if a node is not found we skip over it and proceed with the
    // animation to maintain consistency with other platforms
    m_hasFinished = true;
  }
}

} // namespace rnoh
