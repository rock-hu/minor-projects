#include "CustomNode.h"

#include <glog/logging.h>
#include <memory>
#include "NativeNodeApi.h"

namespace rnoh {
CustomNode::CustomNode()
    : ArkUINode(NativeNodeApi::getInstance()->createNode(
          ArkUI_NodeType::ARKUI_NODE_CUSTOM)),
      m_customNodeDelegate(nullptr) {
  userCallback_ = new UserCallback();

  userCallback_->callback = [this](ArkUI_NodeCustomEvent *event) {
    auto type = OH_ArkUI_NodeCustomEvent_GetEventType(event);
      switch (type) {
        case ARKUI_NODE_CUSTOM_EVENT_ON_MEASURE:
          onMeasure(type);
          break;
        case ARKUI_NODE_CUSTOM_EVENT_ON_LAYOUT:
          onLayout();
        default:
          break;
        }
    };
  eventReceiver = [](ArkUI_NodeCustomEvent *event) {
    auto *userData = reinterpret_cast<UserCallback *>(OH_ArkUI_NodeCustomEvent_GetUserData(event));
    int32_t tagId = OH_ArkUI_NodeCustomEvent_GetEventTargetId(event);
    if (userData && (tagId == 89 || tagId == 90)) {
      userData->callback(event);
    }
  };
  maybeThrow(NativeNodeApi::getInstance()->addNodeCustomEventReceiver(m_nodeHandle, eventReceiver));

  maybeThrow(NativeNodeApi::getInstance()->registerNodeEvent(
      m_nodeHandle, NODE_ON_CLICK, 0, this));
  maybeThrow(NativeNodeApi::getInstance()->registerNodeEvent(
      m_nodeHandle, NODE_ON_HOVER, 0, this));
  maybeThrow(NativeNodeApi::getInstance()->registerNodeCustomEvent(
      m_nodeHandle, ARKUI_NODE_CUSTOM_EVENT_ON_MEASURE, 89, userCallback_));
  maybeThrow(NativeNodeApi::getInstance()->registerNodeCustomEvent(
      m_nodeHandle, ARKUI_NODE_CUSTOM_EVENT_ON_LAYOUT, 90, userCallback_));
}

void CustomNode::onMeasure(ArkUI_NodeCustomEventType eventType) {
    // auto rect = NativeNodeApi::getInstance()->getAttribute(m_nodeHandle, NODE_LAYOUT_RECT);
    // if (!rect) {
    //     LOG(ERROR)<<"Custom Node Get layout Rect para error";
    //     return;
    // }
    // int32_t width = rect->value[2].i32;
    // int32_t height = rect->value[3].i32;
    int32_t width = getSavedWidth();
    int32_t height = getSavedHeight();
    maybeThrow(NativeNodeApi::getInstance()->setMeasuredSize(m_nodeHandle, width, height));
}

void CustomNode::onLayout() {}

void CustomNode::insertChild(ArkUINode& child, std::size_t index) {
  maybeThrow(NativeNodeApi::getInstance()->insertChildAt(
      m_nodeHandle, child.getArkUINodeHandle(), static_cast<int32_t>(index)));
}

void CustomNode::addChild(ArkUINode &child){
  maybeThrow(NativeNodeApi::getInstance()->addChild(
      m_nodeHandle, child.getArkUINodeHandle()));
}
void CustomNode::removeChild(ArkUINode& child) {
  maybeThrow(NativeNodeApi::getInstance()->removeChild(
      m_nodeHandle, child.getArkUINodeHandle()));
}

void CustomNode::setCustomNodeDelegate(CustomNodeDelegate* customNodeDelegate) {
  m_customNodeDelegate = customNodeDelegate;
}

void CustomNode::onNodeEvent(
    ArkUI_NodeEventType eventType,
    EventArgs& eventArgs) {
  if (eventType == ArkUI_NodeEventType::NODE_ON_CLICK &&
      eventArgs[3].i32 != 2) {
    onClick();
  }
  if (eventType == ArkUI_NodeEventType::NODE_ON_HOVER) {
    if (m_customNodeDelegate != nullptr) {
      if (eventArgs[0].i32) {
        m_customNodeDelegate->onHoverIn();
      } else {
        m_customNodeDelegate->onHoverOut();
      }
    }
  }
}

void CustomNode::onClick() {
  if (m_customNodeDelegate != nullptr) {
    m_customNodeDelegate->onClick();
  }
}

CustomNode::~CustomNode() {
  NativeNodeApi::getInstance()->unregisterNodeEvent(
      m_nodeHandle, NODE_ON_CLICK);
  NativeNodeApi::getInstance()->unregisterNodeEvent(
      m_nodeHandle, NODE_ON_HOVER);
  NativeNodeApi::getInstance()->unregisterNodeCustomEvent(
      m_nodeHandle, ARKUI_NODE_CUSTOM_EVENT_ON_MEASURE);
  NativeNodeApi::getInstance()->unregisterNodeCustomEvent(
      m_nodeHandle, ARKUI_NODE_CUSTOM_EVENT_ON_LAYOUT);
  NativeNodeApi::getInstance()->removeNodeCustomEventReceiver(m_nodeHandle, eventReceiver);
  delete userCallback_;
}

CustomNode& CustomNode::setAlign(int32_t align) {
  ArkUI_NumberValue value[] = {{.i32 = align}};
  ArkUI_AttributeItem item = {.value = value, .size = 1};
  maybeThrow(NativeNodeApi::getInstance()->setAttribute(
      m_nodeHandle, NODE_STACK_ALIGN_CONTENT, &item));
  return *this;
}
} // namespace rnoh
