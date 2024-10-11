#pragma once

#include "ArkUINode.h"
#include "TextInputNodeBase.h"
#include "react/renderer/components/textinput/TextInputEventEmitter.h"
#include "react/renderer/components/textinput/TextInputProps.h"
#include "react/renderer/graphics/Color.h"

namespace rnoh {

class TextAreaNodeDelegate {
 public:
  virtual ~TextAreaNodeDelegate() = default;
  virtual void onChange(std::string text) {};
  virtual void onBlur() {};
  virtual void onFocus() {};
  virtual void onPasteOrCut() {};
  virtual void onTextSelectionChange(int32_t location, int32_t length) {};
  virtual void onContentScroll() {};
  virtual void onContentSizeChange(float width, float height, bool multiline) {};
};

class TextAreaNode : public TextInputNodeBase {
 protected:
  TextAreaNodeDelegate* m_textAreaNodeDelegate;

 public:
  TextAreaNode();
  ~TextAreaNode() override;

  facebook::react::Point getTextAreaOffset() const;

  facebook::react::Rect getTextContentRect() const override;

  void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs)
      override;

  void onNodeEvent(ArkUI_NodeEventType eventType, std::string_view eventString)
      override;

  void setTextAreaNodeDelegate(TextAreaNodeDelegate* textAreaNodeDelegate);

  void setTextContent(std::string const& textContent);

  void setInputType(ArkUI_TextAreaType keyboardType);

  void setFont(facebook::react::TextAttributes const& textAttributes) override;

  void setCaretColor(facebook::react::SharedColor const& color) override;

  void setMaxLength(int32_t maxLength) override;

  void setPlaceholder(std::string const& placeholder) override;

  void setPlaceholderColor(facebook::react::SharedColor const& color) override;

  void defaultSetPadding();
  
  void setEnterKeyType(ArkUI_EnterKeyType returnKeyType);
  
  void SetContextMenuHidden(bool const& hidden);
  
  void setTextContentType(std::string const& textContentType);
  
  void setUnderlineColorAndroid(facebook::react::SharedColor const& underlineColorAndroid);
  
  void setAutoFill(bool autoFill);

  void setBlurOnSubmit(bool blurOnSubmit);
  
  void setshowSoftInputOnFocus(int32_t enable);
    
  void setInputFilter(std::string const& inputFilter);

  std::string getTextContent() override;
};

} // namespace rnoh