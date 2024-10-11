#pragma once
#include "RNOH/CppComponentInstance.h"
#include "ButtonNode.h"
#include "ButtonViewComponentDescriptor.h"

namespace rnoh {

class ButtonViewComponentInstance
    : public CppComponentInstance<facebook::react::ButtonViewShadowNode>,
      public ButtonNodeDelegate {
 private:
  ButtonNode m_buttonNode;

 public:
  ButtonViewComponentInstance(Context context);

  ButtonNode& getLocalRootArkUINode() override;

  void onChildInserted(
      ComponentInstance::Shared const& childComponentInstance,
      std::size_t index) override;
  void onChildRemoved(
      ComponentInstance::Shared const& childComponentInstance) override;
  void onPropsChanged(SharedConcreteProps const& props) override;
  void onButtonClick() override;
  void handleCommand(std::string const &commandName, folly::dynamic const &args) override;
};
} // namespace rnoh