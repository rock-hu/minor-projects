/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "frameworks/bridge/declarative_frontend/jsview/js_shape_abstract.h"

#include "base/utils/utils.h"
#include "bridge/declarative_frontend/jsview/models/shape_abstract_model_impl.h"
#include "core/common/container.h"
#include "core/common/resource/resource_parse_utils.h"
#include "core/components_ng/pattern/shape/shape_abstract_model.h"
#include "core/components_ng/pattern/shape/shape_abstract_model_ng.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace {

ShapeAbstractModel* ShapeAbstractModel::GetInstance()
{
#ifdef NG_BUILD
    static NG::ShapeAbstractModelNG instance;
    return &instance;
#else
    if (Container::IsCurrentUseNewPipeline()) {
        static NG::ShapeAbstractModelNG instance;
        return &instance;
    } else {
        static Framework::ShapeAbstractModelImpl instance;
        return &instance;
    }
#endif
}
} // namespace OHOS::Ace

namespace OHOS::Ace::Framework {
namespace {
constexpr double DEFAULT_OPACITY = 1.0;
constexpr double MIN_OPACITY = 0.0;
constexpr double STROKE_MITERLIMIT_DEFAULT = 4.0f;
} // namespace

void JSShapeAbstract::SetStrokeDashArray(const JSCallbackInfo& info)
{
    std::vector<Dimension> dashArray;
    std::vector<RefPtr<ResourceObject>> resObjArray;
    bool hasResObj = false;
    UnRegisterResource("ShapeAbstractStrokeDashArray");
    if (info.Length() < 1 || !info[0]->IsArray()) {
        ShapeAbstractModel::GetInstance()->SetStrokeDashArray(dashArray);
        return;
    }
    JSRef<JSArray> array = JSRef<JSArray>::Cast(info[0]);
    auto length = static_cast<int32_t>(array->Length());
    for (int32_t i = 0; i < length; i++) {
        JSRef<JSVal> value = array->GetValueAt(i);
        CalcDimension dim;
        RefPtr<ResourceObject> resObj;
        bool paramIsValid = false;
        if (Container::LessThanAPIVersion(PlatformVersion::VERSION_TEN)) {
            paramIsValid = ParseJsDimensionVp(value, dim, resObj);
        } else {
            paramIsValid = ParseJsDimensionVpNG(value, dim, resObj);
        }
        if (resObj) {
            hasResObj = true;
        }
        if (paramIsValid) {
            dashArray.emplace_back(dim);
            resObjArray.emplace_back(resObj);
        } else {
            dashArray.clear();
            resObjArray.clear();
            break;
        }
    }
    // if odd,add twice
    if (static_cast<uint32_t>(length) == dashArray.size() && (static_cast<uint32_t>(length) & 1)) {
        for (int32_t i = 0; i < length; i++) {
            dashArray.emplace_back(dashArray[i]);
            resObjArray.emplace_back(resObjArray[i]);
        }
    }
    if (SystemProperties::ConfigChangePerform() &&  hasResObj) {
        ShapeAbstractModel::GetInstance()->SetStrokeDashArray(dashArray, resObjArray);
    }
    ShapeAbstractModel::GetInstance()->SetStrokeDashArray(dashArray);
}

void JSShapeAbstract::SetStroke(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }
    Color strokeColor;
    RefPtr<ResourceObject> strokeResObj;
    UnRegisterResource("ShapeAbstractStroke");
    if (!ParseJsColor(info[0], strokeColor, strokeResObj)) {
        ShapeAbstractModel::GetInstance()->SetStroke(Color::TRANSPARENT);
        return;
    }
    if (SystemProperties::ConfigChangePerform() && strokeResObj) {
        RegisterResource<Color>("ShapeAbstractStroke", strokeResObj, strokeColor);
    }
    ShapeAbstractModel::GetInstance()->SetStroke(strokeColor);
}

void JSShapeAbstract::SetFill(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }
    UnRegisterResource("ShapeAbstractFill");
    if (info[0]->IsString() && info[0]->ToString() == "none") {
        ShapeAbstractModel::GetInstance()->SetFill(Color::TRANSPARENT);
    } else {
        Color fillColor = Color::BLACK;
        RefPtr<ResourceObject> fillResObj;
        static const char shapeComponentName[] = "";
        static const char attrsShapeAbstractFill[] = "fill";
        CheckColor(info[0], fillColor, shapeComponentName, attrsShapeAbstractFill, fillResObj);
        if (SystemProperties::ConfigChangePerform() && fillResObj) {
            RegisterResource<Color>("ShapeAbstractFill", fillResObj, fillColor);
        }
        ShapeAbstractModel::GetInstance()->SetFill(fillColor);
    }
}

void JSShapeAbstract::SetStrokeDashOffset(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }
    CalcDimension offset(0.0f);
    if (Container::LessThanAPIVersion(PlatformVersion::VERSION_TEN)) {
        if (!ParseJsDimensionVp(info[0], offset)) {
            return;
        }
    } else {
        if (!ParseJsDimensionVpNG(info[0], offset)) {
            // set to default value(0.0f)
            offset.SetValue(0.0f);
        }
    }
    ShapeAbstractModel::GetInstance()->SetStrokeDashOffset(offset);
}

void JSShapeAbstract::SetStrokeLineCap(int lineCap)
{
    ShapeAbstractModel::GetInstance()->SetStrokeLineCap(lineCap);
}

void JSShapeAbstract::SetStrokeLineJoin(int lineJoin)
{
    ShapeAbstractModel::GetInstance()->SetStrokeLineJoin(lineJoin);
}

void JSShapeAbstract::SetStrokeMiterLimit(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }
    double miterLimit = STROKE_MITERLIMIT_DEFAULT;
    ParseJsDouble(info[0], miterLimit);
    ShapeAbstractModel::GetInstance()->SetStrokeMiterLimit(miterLimit);
}

void JSShapeAbstract::SetStrokeOpacity(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }
    double strokeOpacity = DEFAULT_OPACITY;
    RefPtr<ResourceObject> strokeOpacityResObj;
    ParseJsDouble(info[0], strokeOpacity, strokeOpacityResObj);
    UnRegisterResource("ShapeAbstractStrokeOpacity");
    if (SystemProperties::ConfigChangePerform() && strokeOpacityResObj) {
        RegisterResource<double>("ShapeAbstractStrokeOpacity", strokeOpacityResObj, strokeOpacity);
    }
    if (GreatOrEqual(strokeOpacity, 1.0)) {
        strokeOpacity = DEFAULT_OPACITY;
    }
    if (LessOrEqual(strokeOpacity, 0.0)) {
        strokeOpacity = MIN_OPACITY;
    }
    ShapeAbstractModel::GetInstance()->SetStrokeOpacity(strokeOpacity);
}

// https://svgwg.org/svg2-draft/painting.html#FillOpacity
void JSShapeAbstract::SetFillOpacity(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }
    double fillOpacity = DEFAULT_OPACITY;
    RefPtr<ResourceObject> fillOpacityResObj;
    ParseJsDouble(info[0], fillOpacity, fillOpacityResObj);
    UnRegisterResource("ShapeAbstractFillOpacity");
    if (SystemProperties::ConfigChangePerform() && fillOpacityResObj) {
        RegisterResource<double>("ShapeAbstractFillOpacity", fillOpacityResObj, fillOpacity);
    }
    if (GreatOrEqual(fillOpacity, DEFAULT_OPACITY)) {
        fillOpacity = DEFAULT_OPACITY;
    }
    if (LessOrEqual(fillOpacity, MIN_OPACITY)) {
        fillOpacity = MIN_OPACITY;
    }
    ShapeAbstractModel::GetInstance()->SetFillOpacity(fillOpacity);
}

void JSShapeAbstract::SetStrokeWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }
    // the default value is 1.0_vp
    CalcDimension lineWidth = 1.0_vp;
    RefPtr<ResourceObject> strokeWidthResObj;
    UnRegisterResource("ShapeAbstractStrokeWidth");
    if (info[0]->IsString()) {
        const std::string& value = info[0]->ToString();
        if (Container::LessThanAPIVersion(PlatformVersion::VERSION_TEN)) {
            lineWidth = StringUtils::StringToDimensionWithUnit(value, DimensionUnit::VP, 1.0);
        } else {
            if (!StringUtils::StringToDimensionWithUnitNG(value, lineWidth, DimensionUnit::VP, 1.0)) {
                // unit is invalid, use default value(1.0vp) instead.
                lineWidth = 1.0_vp;
            }
        }
    } else {
        ParseJsDimensionVp(info[0], lineWidth, strokeWidthResObj);
        if (SystemProperties::ConfigChangePerform() && strokeWidthResObj) {
            RegisterResource<CalcDimension>("ShapeAbstractStrokeWidth", strokeWidthResObj, lineWidth);
        }
    }
    if (lineWidth.IsNegative()) {
        lineWidth = 1.0_vp;
    }
    ShapeAbstractModel::GetInstance()->SetStrokeWidth(lineWidth);
}

void JSShapeAbstract::SetAntiAlias(bool antiAlias)
{
    ShapeAbstractModel::GetInstance()->SetAntiAlias(antiAlias);
}

void JSShapeAbstract::JsWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }

    SetWidth(info[0]);
}

void JSShapeAbstract::SetWidth(const JSRef<JSVal>& jsValue)
{
    CalcDimension value;
    RefPtr<ResourceObject> widthResObj;
    UnRegisterResource("ShapeAbstractWidth");
    if (jsValue->IsUndefined()) {
        ViewAbstractModel::GetInstance()->UpdateLayoutPolicyProperty(LayoutCalPolicy::NO_MATCH, true);
        ViewAbstractModel::GetInstance()->ClearWidthOrHeight(true);
        return;
    }
    if (Container::LessThanAPIVersion(PlatformVersion::VERSION_TEN)) {
        if (!ParseJsDimensionVp(jsValue, value, widthResObj)) {
            return;
        }
    } else {
        if (!ParseJsDimensionVpNG(jsValue, value, widthResObj)) {
            // JsWidth return, check if set LayoutPolicy before return.
            ViewAbstractModel::GetInstance()->ClearWidthOrHeight(true);
            if (jsValue->IsObject()) {
                JSRef<JSObject> object = JSRef<JSObject>::Cast(jsValue);
                JSRef<JSVal> layoutPolicy = object->GetProperty("id_");
                if (layoutPolicy->IsString()) {
                    auto policy = ParseLayoutPolicy(layoutPolicy->ToString());
                    ViewAbstractModel::GetInstance()->UpdateLayoutPolicyProperty(policy, true);
                    return;
                }
            }
            return;
        }
    }
    if (SystemProperties::ConfigChangePerform() && widthResObj) {
        RegisterResource<CalcDimension>("ShapeAbstractWidth", widthResObj, value);
    }
    if (LessNotEqual(value.Value(), 0.0)) {
        value.SetValue(0.0);
    }
    ShapeAbstractModel::GetInstance()->SetWidth(value);
}

void JSShapeAbstract::JsHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }

    SetHeight(info[0]);
}

void JSShapeAbstract::SetHeight(const JSRef<JSVal>& jsValue)
{
    CalcDimension value;
    RefPtr<ResourceObject> heightResObj;
    UnRegisterResource("ShapeAbstractHeight");
    if (jsValue->IsUndefined()) {
        ViewAbstractModel::GetInstance()->UpdateLayoutPolicyProperty(LayoutCalPolicy::NO_MATCH, false);
        ViewAbstractModel::GetInstance()->ClearWidthOrHeight(false);
        return;
    }
    if (Container::LessThanAPIVersion(PlatformVersion::VERSION_TEN)) {
        if (!ParseJsDimensionVp(jsValue, value, heightResObj)) {
            return;
        }
    } else {
        if (!ParseJsDimensionVpNG(jsValue, value, heightResObj)) {
            // JsHeight return, check if set LayoutPolicy before return.
            ViewAbstractModel::GetInstance()->ClearWidthOrHeight(false);
            if (jsValue->IsObject()) {
                JSRef<JSObject> object = JSRef<JSObject>::Cast(jsValue);
                JSRef<JSVal> layoutPolicy = object->GetProperty("id_");
                if (layoutPolicy->IsString()) {
                    auto policy = ParseLayoutPolicy(layoutPolicy->ToString());
                    ViewAbstractModel::GetInstance()->UpdateLayoutPolicyProperty(policy, false);
                    return;
                }
            }
            return;
        }
    }
    if (SystemProperties::ConfigChangePerform() && heightResObj) {
        RegisterResource<CalcDimension>("ShapeAbstractHeight", heightResObj, value);
    }

    if (LessNotEqual(value.Value(), 0.0)) {
        value.SetValue(0.0);
    }
    ShapeAbstractModel::GetInstance()->SetHeight(value);
}

void JSShapeAbstract::JsSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }

    if (!info[0]->IsObject()) {
        return;
    }

    JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
    SetWidth(sizeObj->GetProperty("width"));
    SetHeight(sizeObj->GetProperty("height"));
}

void JSShapeAbstract::ObjectWidth(const JSCallbackInfo& info)
{
    info.ReturnSelf();
    if (info.Length() < 1) {
        return;
    }

    ObjectWidth(info[0]);
}

void JSShapeAbstract::ObjectWidth(const JSRef<JSVal>& jsValue)
{
    CalcDimension value;
    RefPtr<ResourceObject> widthResObj;
    if (!ParseJsDimensionVp(jsValue, value, widthResObj)) {
        return;
    }
    if (SystemProperties::ConfigChangePerform() && widthResObj) {
        auto&& updateFunc = [](const RefPtr<ResourceObject>& resObj, BasicShape& basicShape) {
            CalcDimension shapeValue;
            ResourceParseUtils::ParseResDimensionVp(resObj, shapeValue);
            if (LessNotEqual(shapeValue.Value(), 0.0)) {
                return;
            }
            basicShape.SetWidth(shapeValue);
        };
        basicShape_->AddResource("shapeAbstract.width", widthResObj, std::move(updateFunc));
    }
    if (LessNotEqual(value.Value(), 0.0)) {
        return;
    }
    if (basicShape_) {
        basicShape_->SetWidth(value);
    }
}

void JSShapeAbstract::ObjectHeight(const JSCallbackInfo& info)
{
    info.ReturnSelf();
    if (info.Length() < 1) {
        return;
    }

    ObjectHeight(info[0]);
}

void JSShapeAbstract::ObjectHeightUpdate(const RefPtr<ResourceObject>& heightResObj)
{
    if (heightResObj) {
        auto&& updateFunc = [](const RefPtr<ResourceObject>& resObj, BasicShape& basicShape) {
            CalcDimension shapeValue;
            ResourceParseUtils::ParseResDimensionVp(resObj, shapeValue);
            if (LessNotEqual(shapeValue.Value(), 0.0)) {
                return;
            }
            basicShape.SetHeight(shapeValue);
        };
        basicShape_->AddResource("shapeAbstract.height", heightResObj, std::move(updateFunc));
    }
}

void JSShapeAbstract::ObjectHeight(const JSRef<JSVal>& jsValue)
{
    CalcDimension value;
    if (!SystemProperties::ConfigChangePerform()) {
        if (!ParseJsDimensionVp(jsValue, value)) {
            return;
        }
    } else {
        RefPtr<ResourceObject> heightResObj;
        if (!ParseJsDimensionVp(jsValue, value, heightResObj)) {
            LOGE("fail to parse the Dimension!");
            return;
        }
        ObjectHeightUpdate(heightResObj);
    }
    if (LessNotEqual(value.Value(), 0.0)) {
        return;
    }
    if (basicShape_) {
        basicShape_->SetHeight(value);
    }
}

void JSShapeAbstract::ObjectSize(const JSCallbackInfo& info)
{
    info.ReturnSelf();
    if (info.Length() < 1) {
        return;
    }

    if (!info[0]->IsObject()) {
        return;
    }

    JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
    ObjectWidth(sizeObj->GetProperty("width"));
    ObjectHeight(sizeObj->GetProperty("height"));
}

void JSShapeAbstract::ObjectOffsetUpdate(const RefPtr<ResourceObject>& xResObj, RefPtr<ResourceObject> yResObj)
{
    if (xResObj) {
        auto&& updateFunc = [](const RefPtr<ResourceObject>& resObj, BasicShape& basicShape) {
            CalcDimension shapeValueX;
            ResourceParseUtils::ParseResDimensionVp(resObj, shapeValueX);
            CalcDimension shapeValueY = basicShape.GetOffset().GetY();
            basicShape.SetOffset(DimensionOffset(shapeValueX, shapeValueY));
        };
        basicShape_->AddResource("shapeAbstract.ObjectOffset.X", xResObj, std::move(updateFunc));
    }
    if (yResObj) {
        auto&& updateFunc = [](const RefPtr<ResourceObject>& resObj, BasicShape& basicShape) {
            CalcDimension shapeValueY;
            ResourceParseUtils::ParseResDimensionVp(resObj, shapeValueY);
            CalcDimension shapeValueX = basicShape.GetOffset().GetX();
            basicShape.SetOffset(DimensionOffset(shapeValueX, shapeValueY));
        };
        basicShape_->AddResource("shapeAbstract.ObjectOffset.Y", yResObj, std::move(updateFunc));
    }
}

void JSShapeAbstract::ObjectOffset(const JSCallbackInfo& info)
{
    info.ReturnSelf();
    if (info.Length() > 0 && info[0]->IsObject()) {
        JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> xVal = sizeObj->GetProperty("x");
        JSRef<JSVal> yVal = sizeObj->GetProperty("y");
        CalcDimension x;
        CalcDimension y;
        if (!SystemProperties::ConfigChangePerform()) {
            if (basicShape_ && ParseJsDimensionVp(xVal, x) && ParseJsDimensionVp(yVal, y)) {
                basicShape_->SetOffset(DimensionOffset(x, y));
            }
        } else {
            RefPtr<ResourceObject> xResObj;
            RefPtr<ResourceObject> yResObj;
            if (basicShape_ && ParseJsDimensionVp(xVal, x, xResObj) && ParseJsDimensionVp(yVal, y, yResObj)) {
                ObjectOffsetUpdate(xResObj, yResObj);
                basicShape_->SetOffset(DimensionOffset(x, y));
            }
        }
    }
}

void JSShapeAbstract::ObjectFillUpdate(const RefPtr<ResourceObject>& fillResObj)
{
    if (fillResObj) {
        auto&& updateFunc = [](const RefPtr<ResourceObject>& resObj, BasicShape& basicShape) {
            Color shapeValue;
            ResourceParseUtils::ParseResColor(resObj, shapeValue);
            basicShape.SetColor(shapeValue);
        };
        basicShape_->AddResource("shapeAbstract.fill", fillResObj, std::move(updateFunc));
    }
}

void JSShapeAbstract::ObjectFill(const JSCallbackInfo& info)
{
    info.ReturnSelf();
    if (info.Length() < 1) {
        return;
    }

    Color color;
    if (!SystemProperties::ConfigChangePerform()) {
        if (ParseJsColor(info[0], color) && basicShape_) {
            basicShape_->SetColor(color);
        }
    } else {
        RefPtr<ResourceObject> fillResObj;
        if (ParseJsColor(info[0], color, fillResObj) && basicShape_) {
            ObjectFillUpdate(fillResObj);
            basicShape_->SetColor(color);
        }
    }
}

void JSShapeAbstract::JSBind(BindingTarget globalObj)
{
    JSClass<JSShapeAbstract>::Declare("JSShapeAbstract");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSShapeAbstract>::StaticMethod("stroke", &JSShapeAbstract::SetStroke, opt);
    JSClass<JSShapeAbstract>::StaticMethod("fill", &JSShapeAbstract::SetFill, opt);
    JSClass<JSShapeAbstract>::StaticMethod("foregroundColor", &JSShapeAbstract::SetForegroundColor, opt);
    JSClass<JSShapeAbstract>::StaticMethod("strokeDashOffset", &JSShapeAbstract::SetStrokeDashOffset, opt);
    JSClass<JSShapeAbstract>::StaticMethod("strokeDashArray", &JSShapeAbstract::SetStrokeDashArray);
    JSClass<JSShapeAbstract>::StaticMethod("strokeLineCap", &JSShapeAbstract::SetStrokeLineCap, opt);
    JSClass<JSShapeAbstract>::StaticMethod("strokeLineJoin", &JSShapeAbstract::SetStrokeLineJoin, opt);
    JSClass<JSShapeAbstract>::StaticMethod("strokeMiterLimit", &JSShapeAbstract::SetStrokeMiterLimit, opt);
    JSClass<JSShapeAbstract>::StaticMethod("strokeOpacity", &JSShapeAbstract::SetStrokeOpacity, opt);
    JSClass<JSShapeAbstract>::StaticMethod("fillOpacity", &JSShapeAbstract::SetFillOpacity, opt);
    JSClass<JSShapeAbstract>::StaticMethod("strokeWidth", &JSShapeAbstract::SetStrokeWidth, opt);
    JSClass<JSShapeAbstract>::StaticMethod("antiAlias", &JSShapeAbstract::SetAntiAlias, opt);
    JSClass<JSShapeAbstract>::StaticMethod("width", &JSShapeAbstract::JsWidth, opt);
    JSClass<JSShapeAbstract>::StaticMethod("height", &JSShapeAbstract::JsHeight, opt);
    JSClass<JSShapeAbstract>::StaticMethod("size", &JSShapeAbstract::JsSize, opt);
    JSClass<JSShapeAbstract>::StaticMethod("onAttach", &JSInteractableView::JsOnAttach);
    JSClass<JSShapeAbstract>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSShapeAbstract>::StaticMethod("onDetach", &JSInteractableView::JsOnDetach);
    JSClass<JSShapeAbstract>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSShapeAbstract>::InheritAndBind<JSViewAbstract>(globalObj);
}

void JSShapeAbstract::SetSize(const JSCallbackInfo& info)
{
    if (info.Length() > 0 && info[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> width = obj->GetProperty("width");
        JSRef<JSVal> height = obj->GetProperty("height");
        SetWidth(width);
        SetHeight(height);
    }
}

void JSShapeAbstract::ObjectPositionUpdate(DimensionOffset& position, RefPtr<ResourceObject>& xResObj,
    RefPtr<ResourceObject>& yResObj)
{
    if (SystemProperties::ConfigChangePerform() && xResObj) {
        auto&& updateFunc = [position](const RefPtr<ResourceObject>& resObj, BasicShape& basicShape) {
            DimensionOffset& value = const_cast<DimensionOffset&>(position);
            CalcDimension x;
            CalcDimension y;
            if (!ResourceParseUtils::ParseResDimensionVp(resObj, x)) {
                x = basicShape.GetPosition().GetX();
            }
            y = basicShape.GetPosition().GetY();
            value.SetX(x);
            value.SetY(y);
            basicShape.SetPosition(value);
        };
        basicShape_->AddResource("shapeAbstract.position.xResObj", xResObj, std::move(updateFunc));
    }
    if (SystemProperties::ConfigChangePerform() && yResObj) {
        auto&& updateFunc = [position](const RefPtr<ResourceObject>& resObj, BasicShape& basicShape) {
            DimensionOffset& value = const_cast<DimensionOffset&>(position);
            CalcDimension x;
            CalcDimension y;
            x = basicShape.GetPosition().GetX();
            if (!ResourceParseUtils::ParseResDimensionVp(resObj, y)) {
                y = basicShape.GetPosition().GetY();
            }
            value.SetX(x);
            value.SetY(y);
            basicShape.SetPosition(value);
        };
        basicShape_->AddResource("shapeAbstract.position.yResObj", yResObj, std::move(updateFunc));
    }
}

void JSShapeAbstract::ObjectPosition(const JSCallbackInfo& info)
{
    info.ReturnSelf();
    if (!(info.Length() > 0 && info[0]->IsObject())) {
        LOGE("Info is invalid!");
        return;
    }
    JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
    JSRef<JSVal> xVal = sizeObj->GetProperty("x");
    JSRef<JSVal> yVal = sizeObj->GetProperty("y");
    CalcDimension x;
    CalcDimension y;
    DimensionOffset position(x, y);
    CHECK_NULL_VOID(basicShape_);
    RefPtr<ResourceObject> xResObj;
    RefPtr<ResourceObject> yResObj;
    if (ParseJsDimensionVp(xVal, x, xResObj)) {
        position.SetX(x);
    }
    if (ParseJsDimensionVp(yVal, y, yResObj)) {
        position.SetY(y);
    }
    ObjectPositionUpdate(position, xResObj, yResObj);
    basicShape_->SetPosition(position);
}

void JSShapeAbstract::SetForegroundColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }
    Color foregroundColor;
    RefPtr<ResourceObject> foregroundColorResObj;
    ForegroundColorStrategy strategy;
    UnRegisterResource("ShapeAbstractForegroundColor");
    if (ParseJsColorStrategy(info[0], strategy)) {
        ShapeAbstractModel::GetInstance()->SetFill(Color::FOREGROUND);
        ViewAbstractModel::GetInstance()->SetForegroundColorStrategy(strategy);
        return;
    }
    if (!ParseJsColor(info[0], foregroundColor, foregroundColorResObj)) {
        ShapeAbstractModel::GetInstance()->SetFill(Color::BLACK);
        ViewAbstractModel::GetInstance()->SetForegroundColor(Color::BLACK);
        return;
    }
    if (SystemProperties::ConfigChangePerform() && foregroundColorResObj) {
        RegisterResource<Color>("ShapeAbstractForegroundColor", foregroundColorResObj, foregroundColor);
    }
    ShapeAbstractModel::GetInstance()->SetForegroundColor(foregroundColor);
    ViewAbstractModel::GetInstance()->SetForegroundColor(foregroundColor);
}
} // namespace OHOS::Ace::Framework
