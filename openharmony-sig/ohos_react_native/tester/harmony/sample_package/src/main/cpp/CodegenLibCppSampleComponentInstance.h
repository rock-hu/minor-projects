#pragma once

#include "RNOH/arkui/StackNode.h"
#include "RNOH/arkui/TextNode.h"
#include "RNOH/generated/components/BaseCodegenLibCppSampleComponentComponentInstance.h"

namespace rnoh {
class CodegenLibCppSampleComponentInstance
    : public BaseCodegenLibCppSampleComponentComponentInstance {
  using super = BaseCodegenLibCppSampleComponentComponentInstance;

  StackNode m_stackNode;
  TextNode m_textNode;

 public:
  CodegenLibCppSampleComponentInstance(Context context)
      : super(std::move(context)) {
    m_stackNode.insertChild(m_textNode, 0);
  }

  ArkUINode& getLocalRootArkUINode() {
    return m_stackNode;
  };

 protected:
  void onPropsChanged(SharedConcreteProps const& props) override {
    super::onPropsChanged(props);
    m_textNode.setTextContent(props->text);
  };

  void onFinalizeUpdates() override {
    super::onFinalizeUpdates();
    m_eventEmitter->onMount({.text = m_props->text});
  }
};

} // namespace rnoh