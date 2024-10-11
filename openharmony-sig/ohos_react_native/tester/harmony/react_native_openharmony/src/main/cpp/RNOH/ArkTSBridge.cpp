#include "ArkTSBridge.h"

#include <boost/exception/diagnostic_information.hpp>
#include <glog/logging.h>
#include <jsi/jsi.h>
#include "RNOH/Assert.h"
#include "RNOH/RNOHError.h"

namespace rnoh {

std::shared_ptr<ArkTSBridge> ArkTSBridge::instance = nullptr;
std::once_flag ArkTSBridge::initFlag;

ArkTSBridge::ArkTSBridge(napi_env env, napi_ref napiBridgeRef)
    : m_arkJs(ArkJS(env)), m_arkTSBridgeRef(napiBridgeRef) {
  LOG(INFO) << "ArkTSBridge::ArkTSBridge";
}

void ArkTSBridge::initializeInstance(
    napi_env env,
    napi_ref arkTSBridgeHandler) {
   std::call_once(initFlag, [&] {
        instance = std::shared_ptr<ArkTSBridge>(new ArkTSBridge(env, arkTSBridgeHandler));
    });
}

ArkTSBridge::Shared ArkTSBridge::getInstance() {
  RNOH_ASSERT_MSG(instance != nullptr, "ArkTSBridge is not initialized");
  return instance;
}

ArkTSBridge::~ArkTSBridge() {
  m_arkJs.deleteReference(m_arkTSBridgeRef);
}

void ArkTSBridge::handleError(std::exception_ptr ex) {
  try {
    LOG(ERROR) << boost::diagnostic_information(ex);
    std::rethrow_exception(ex);
  } catch (const RNOHError& e) {
    m_arkJs.getObject(m_arkTSBridgeRef)
        .call("handleError", {m_arkJs.createFromRNOHError(e)});
  } catch (const facebook::jsi::JSError& e) {
    m_arkJs.getObject(m_arkTSBridgeRef)
        .call("handleError", {m_arkJs.createFromJSError(e)});
  } catch (const std::exception& e) {
    m_arkJs.getObject(m_arkTSBridgeRef)
        .call("handleError", {m_arkJs.createFromException(e)});
  }
}

auto ArkTSBridge::getDisplayMetrics() -> DisplayMetrics {
  auto napiBridgeObject = m_arkJs.getReferenceValue(m_arkTSBridgeRef);
  auto methodImpl =
      m_arkJs.getObjectProperty(napiBridgeObject, "getDisplayMetrics");
  auto napiResult = m_arkJs.call<0>(methodImpl, {});
  return DisplayMetrics::fromNapiValue(m_arkJs.getEnv(), napiResult);
}

auto ArkTSBridge::getFoldStatus() -> uint32_t  {
  auto napiBridgeObject = m_arkJs.getReferenceValue(m_arkTSBridgeRef);
  auto methodImpl =
      m_arkJs.getObjectProperty(napiBridgeObject, "getFoldStatus");
  auto napiResult = m_arkJs.call<0>(methodImpl, {});
    return m_arkJs.getInteger(napiResult);
}

auto ArkTSBridge::getIsSplitScreenMode() -> bool  {
  auto napiBridgeObject = m_arkJs.getReferenceValue(m_arkTSBridgeRef);
  auto methodImpl =
      m_arkJs.getObjectProperty(napiBridgeObject, "getIsSplitScreenMode");
  auto napiResult = m_arkJs.call<0>(methodImpl, {});
    return m_arkJs.getBoolean(napiResult);
}

auto ArkTSBridge::getFontSizeScale() -> float  {
  auto napiBridgeObject = m_arkJs.getReferenceValue(m_arkTSBridgeRef);
  auto methodImpl =
      m_arkJs.getObjectProperty(napiBridgeObject, "getFontSizeScale");
  auto napiResult = m_arkJs.call<0>(methodImpl, {});
    return m_arkJs.getDouble(napiResult);
}

auto PhysicalPixels::fromNapiValue(napi_env env, napi_value value)
    -> PhysicalPixels {
  ArkJS arkJs(env);
  return {
      static_cast<float>(
          arkJs.getDouble(arkJs.getObjectProperty(value, "width"))),
      static_cast<float>(
          arkJs.getDouble(arkJs.getObjectProperty(value, "height"))),
      static_cast<float>(
          arkJs.getDouble(arkJs.getObjectProperty(value, "scale"))),
      static_cast<float>(
          arkJs.getDouble(arkJs.getObjectProperty(value, "fontScale"))),
      static_cast<float>(
          arkJs.getDouble(arkJs.getObjectProperty(value, "densityDpi"))),
  };
}

auto DisplayMetrics::fromNapiValue(napi_env env, napi_value value)
    -> DisplayMetrics {
  ArkJS arkJs(env);
  return {
      PhysicalPixels::fromNapiValue(
          env, arkJs.getObjectProperty(value, "windowPhysicalPixels")),
      PhysicalPixels::fromNapiValue(
          env, arkJs.getObjectProperty(value, "screenPhysicalPixels")),
  };
}

} // namespace rnoh