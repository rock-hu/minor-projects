#pragma once
#include <react/renderer/graphics/Size.h>
#include <react/renderer/textlayoutmanager/TextLayoutManager.h>
#include <native_drawing/drawing_font_collection.h>
#include <native_drawing/drawing_text_typography.h>
#include <string>
#include "ArkUITypography.h"
#include <rawfile/raw_file_manager.h>
#include "RNOH/FeatureFlagRegistry.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"
#include "napi/native_api.h"
#include "TextMeasureRegistry.h"

namespace rnoh {

class TextMeasurer : public facebook::react::TextLayoutManagerDelegate {
 public:
  TextMeasurer(
      napi_env env,
      napi_ref measureTextFnRef,
      std::shared_ptr<TaskExecutor> taskExecutor,
      FeatureFlagRegistry::Shared featureFlagManager,
      int id)
      : m_env(env),
        m_measureTextFnRef(measureTextFnRef),
        m_taskExecutor(taskExecutor),
        m_featureFlagRegistry(featureFlagManager),
        m_rnInstanceId(id) {}
  facebook::react::TextMeasurement measure(
      facebook::react::AttributedString attributedString,
      facebook::react::ParagraphAttributes paragraphAttributes,
      facebook::react::LayoutConstraints layoutConstraints) override;

  ArkUITypographyBuilder measureTypography(
      facebook::react::AttributedString const& attributedString,
      facebook::react::ParagraphAttributes const& paragraphAttributes,
      facebook::react::LayoutConstraints const& layoutConstraints);

  std::vector<OH_Drawing_LineMetrics> getLineMetrics(
      facebook::react::AttributedString const& attributedString,
      facebook::react::ParagraphAttributes const& paragraphAttributes,
      facebook::react::LayoutConstraints const& layoutConstraints) override;
  
  void setTextMeasureParams(float m_fontScale, float m_scale, float DPI, bool m_halfleading);
    
  float getDPI(){ return m_DPI; };
  void registerFont(
    std::weak_ptr<NativeResourceManager> weakResourceManager,
    const std::string name,
    const std::string fontFilePath
  );
  SharedFontCollection getFontCollection();
  std::string getDefaultFontFamilyName();

 private:
  
  std::pair<ArkUITypographyBuilder, ArkUITypography> findFitFontSize(int maxFontSize,
    facebook::react::AttributedString& attributedString,
    facebook::react::ParagraphAttributes& paragraphAttributes,
    facebook::react::LayoutConstraints& layoutConstraints);
  
  std::string stringCapitalize(const std::string& strInput);
  void textCaseTransform(std::string& textContent, facebook::react::TextTransform type);
  bool existDefaultFont(std::string path);
  void updateDefaultFont();
  
  napi_env m_env;
  napi_ref m_measureTextFnRef;
  std::shared_ptr<TaskExecutor> m_taskExecutor;
  FeatureFlagRegistry::Shared m_featureFlagRegistry;
  int32_t getOHDrawingTextAlign(
      const facebook::react::TextAlignment& textAlign);
  float m_fontScale = 1.0;
  float m_scale = 1.0;
  int m_rnInstanceId = 0;
  float m_DPI = 306;
  bool m_halfleading = false;
  std::mutex m_fontCollectionMtx;
  SharedFontCollection m_fontCollection;
  std::mutex m_fontFileContentByFontFamilyMtx;
  std::unordered_map<std::string, std::vector<uint8_t>>
    m_fontFileContentByFontFamily;

  std::mutex m_defaultFontFamilyNameMtx;
  std::string m_defaultFontFamilyName;
};
} // namespace rnoh