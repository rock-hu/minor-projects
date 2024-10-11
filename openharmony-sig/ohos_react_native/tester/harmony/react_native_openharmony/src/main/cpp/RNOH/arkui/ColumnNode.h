#pragma once

#include "ArkUINode.h"

namespace rnoh {
class ColumnNodeDelegate {
 public:
  virtual ~ColumnNodeDelegate() = default;
  virtual void onClick(){};
};

class ColumnNode : public ArkUINode {
 protected:
  ColumnNodeDelegate* m_columnNodeDelegate;

 public:
  ColumnNode();
  ~ColumnNode() override;

  ArkUI_NodeHandle getChildAt(std::size_t index);
  void insertChild(ArkUINode& child, std::size_t index);
  void removeChild(ArkUINode& child);
  void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs)
      override;
  void onClick();
  void setColumnNodeDelegate(ColumnNodeDelegate* columnNodeDelegate);
};

} // namespace rnoh
