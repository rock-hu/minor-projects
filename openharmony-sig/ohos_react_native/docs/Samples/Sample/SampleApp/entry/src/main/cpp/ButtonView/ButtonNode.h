#pragma once
#include "RNOH/arkui/ArkUINode.h"
#include "RNOH/arkui/NativeNodeApi.h"

class ButtonNodeDelegate {
 public:
  virtual ~ButtonNodeDelegate() = default;
  virtual void onButtonClick(){};
};

namespace rnoh {
class ButtonNode : public ArkUINode {
 protected:
  ButtonNodeDelegate* m_buttonNodeDelegate;

 public:
  ButtonNode();
  ~ButtonNode();

  void insertChild(ArkUINode& child, std::size_t index);
  void removeChild(ArkUINode& child);
  void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs);
  ButtonNode& setLabel(const std::string &src);
  ButtonNode& setButtonNodeDelegate(ButtonNodeDelegate* buttonNodeDelegate);
};
} // namespace rnoh