/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/components/view/ViewShadowNode.h>
#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/CustomNode.h"

namespace rnoh {
class CustomNodeComponentInstance
    : public CppComponentInstance<facebook::react::ViewShadowNode>,
      public CustomNodeDelegate {
 private:
  CustomNode m_customNode;
  std::unordered_map<facebook::react::Tag, bool> m_childrenClippedState;
  facebook::react::Point m_previousOffset;

  bool isViewClipped(
      const ComponentInstance::Shared& child,
      facebook::react::Point currentOffset,
      facebook::react::Rect parentBoundingBox);
 public:
  CustomNodeComponentInstance(Context context);

  void onChildInserted(
      ComponentInstance::Shared const& childComponentInstance,
      std::size_t index) override;
  void onChildRemoved(
      ComponentInstance::Shared const& childComponentInstance) override;

  void onPropsChanged(SharedConcreteProps const& props) override;

  void updateClippedSubviews(bool childrenChange = false);
  void restoreClippedSubviews();

  void onFinalizeUpdates() override;
  void onClick() override;
  void onHoverIn() override;
  void onHoverOut() override;
  CustomNode& getLocalRootArkUINode() override;
};
} // namespace rnoh