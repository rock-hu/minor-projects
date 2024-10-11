/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_THEME_JS_THEME_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_THEME_JS_THEME_H

#include "bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "core/components/common/properties/color.h"

#define COLORS_NUMBER (51)

#define BRAND (0)
#define WARNING (1)
#define ALERT (2)
#define CONFIRM (3)

#define FONT_PRIMARY (4)
#define FONT_SECONDARY (5)
#define FONT_TERTIARY (6)
#define FONT_FOURTH (7)
#define FONT_EMPHASIZE (8)

#define FONT_ON_PRIMARY (9)
#define FONT_ON_SECONDARY (10)
#define FONT_ON_TERTIARY (11)
#define FONT_ON_FOURTH (12)

#define ICON_PRIMARY (13)
#define ICON_SECONDARY (14)
#define ICON_TERTIARY (15)
#define ICON_FOURTH (16)
#define ICON_EMPHASIZE (17)
#define ICON_SUB_EMPHASIZE (18)

#define ICON_ON_PRIMARY (19)
#define ICON_ON_SECONDARY (20)
#define ICON_ON_TERTIARY (21)
#define ICON_ON_FOURTH (22)

#define BACKGROUND_PRIMARY (23)
#define BACKGROUND_SECONDARY (24)
#define BACKGROUND_TERTIARY (25)
#define BACKGROUND_FOURTH (26)
#define BACKGROUND_EMPHASIZE (27)

#define COMP_FOREGROUND_PRIMARY (28)
#define COMP_BACKGROUND_PRIMARY (29)
#define COMP_BACKGROUND_PRIMARY_TRAN (30)
#define COMP_BACKGROUND_PRIMARY_CONTRARY (31)
#define COMP_BACKGROUND_GRAY (32)
#define COMP_BACKGROUND_SECONDARY (33)
#define COMP_BACKGROUND_TERTIARY (34)
#define COMP_BACKGROUND_EMPHASIZE (35)
#define COMP_BACKGROUND_NEUTRAL (36)
#define COMP_EMPHASIZE_SECONDARY (37)
#define COMP_EMPHASIZE_TERTIARY (38)
#define COMP_DIVIDER (39)
#define COMP_COMMON_CONTRARY (40)
#define COMP_BACKGROUND_FOCUS (41)
#define COMP_FOCUSED_PRIMARY (42)
#define COMP_FOCUSED_SECONDARY (43)
#define COMP_FOCUSED_TERTIARY (44)

#define INTERACTIVE_HOVER (45)
#define INTERACTIVE_PRESSED (46)
#define INTERACTIVE_FOCUS (47)
#define INTERACTIVE_ACTIVE (48)
#define INTERACTIVE_SELECT (49)
#define INTERACTIVE_CLICK (50)

namespace OHOS::Ace::Framework {
class JSThemeColors {
public:
    JSThemeColors() = default;
    virtual ~JSThemeColors() = default;

    void SetColors(const JSRef<JSArray>& colors)
    {
        for (int i = 0; i < COLORS_NUMBER; i++) {
            colors_.push_back(colors->GetValueAt(i));
        }
    }

    Color Brand() const
    {
        return ConvertJsValueToColor(colors_[BRAND]);
    }
    Color Warning() const
    {
        return ConvertJsValueToColor(colors_[WARNING]);
    }
    Color Alert() const
    {
        return ConvertJsValueToColor(colors_[ALERT]);
    }
    Color Confirm() const
    {
        return ConvertJsValueToColor(colors_[CONFIRM]);
    }

    Color FontPrimary() const
    {
        return ConvertJsValueToColor(colors_[FONT_PRIMARY]);
    }
    Color FontSecondary() const
    {
        return ConvertJsValueToColor(colors_[FONT_SECONDARY]);
    }
    Color FontTertiary() const
    {
        return ConvertJsValueToColor(colors_[FONT_TERTIARY]);
    }
    Color FontFourth() const
    {
        return ConvertJsValueToColor(colors_[FONT_FOURTH]);
    }
    Color FontEmphasize() const
    {
        return ConvertJsValueToColor(colors_[FONT_EMPHASIZE]);
    }

    Color FontOnPrimary() const
    {
        return ConvertJsValueToColor(colors_[FONT_ON_PRIMARY]);
    }
    Color FontOnSecondary() const
    {
        return ConvertJsValueToColor(colors_[FONT_ON_SECONDARY]);
    }
    Color FontOnTertiary() const
    {
        return ConvertJsValueToColor(colors_[FONT_ON_TERTIARY]);
    }
    Color FontOnFourth() const
    {
        return ConvertJsValueToColor(colors_[FONT_ON_FOURTH]);
    }

    Color IconPrimary() const
    {
        return ConvertJsValueToColor(colors_[ICON_PRIMARY]);
    }
    Color IconSecondary() const
    {
        return ConvertJsValueToColor(colors_[ICON_SECONDARY]);
    }
    Color IconTertiary() const
    {
        return ConvertJsValueToColor(colors_[ICON_TERTIARY]);
    }
    Color IconFourth() const
    {
        return ConvertJsValueToColor(colors_[ICON_FOURTH]);
    }
    Color IconEmphasize() const
    {
        return ConvertJsValueToColor(colors_[ICON_EMPHASIZE]);
    }
    Color IconSubEmphasize() const
    {
        return ConvertJsValueToColor(colors_[ICON_SUB_EMPHASIZE]);
    }

    Color IconOnPrimary() const
    {
        return ConvertJsValueToColor(colors_[ICON_ON_PRIMARY]);
    }
    Color IconOnSecondary() const
    {
        return ConvertJsValueToColor(colors_[ICON_ON_SECONDARY]);
    }
    Color IconOnTertiary() const
    {
        return ConvertJsValueToColor(colors_[ICON_ON_TERTIARY]);
    }
    Color IconOnFourth() const
    {
        return ConvertJsValueToColor(colors_[ICON_ON_FOURTH]);
    }

    Color BackgroundPrimary() const
    {
        return ConvertJsValueToColor(colors_[BACKGROUND_PRIMARY]);
    }
    Color BackgroundSecondary() const
    {
        return ConvertJsValueToColor(colors_[BACKGROUND_SECONDARY]);
    }
    Color BackgroundTertiary() const
    {
        return ConvertJsValueToColor(colors_[BACKGROUND_TERTIARY]);
    }
    Color BackgroundFourth() const
    {
        return ConvertJsValueToColor(colors_[BACKGROUND_FOURTH]);
    }
    Color BackgroundEmphasize() const
    {
        return ConvertJsValueToColor(colors_[BACKGROUND_EMPHASIZE]);
    }

    Color CompForegroundPrimary() const
    {
        return ConvertJsValueToColor(colors_[COMP_FOREGROUND_PRIMARY]);
    }
    Color CompBackgroundPrimary() const
    {
        return ConvertJsValueToColor(colors_[COMP_BACKGROUND_PRIMARY]);
    }
    Color CompBackgroundPrimaryTran() const
    {
        return ConvertJsValueToColor(colors_[COMP_BACKGROUND_PRIMARY_TRAN]);
    }
    Color CompBackgroundPrimaryContrary() const
    {
        return ConvertJsValueToColor(colors_[COMP_BACKGROUND_PRIMARY_CONTRARY]);
    }
    Color CompBackgroundGray() const
    {
        return ConvertJsValueToColor(colors_[COMP_BACKGROUND_GRAY]);
    }
    Color CompBackgroundSecondary() const
    {
        return ConvertJsValueToColor(colors_[COMP_BACKGROUND_SECONDARY]);
    }
    Color CompBackgroundTertiary() const
    {
        return ConvertJsValueToColor(colors_[COMP_BACKGROUND_TERTIARY]);
    }
    Color CompBackgroundEmphasize() const
    {
        return ConvertJsValueToColor(colors_[COMP_BACKGROUND_EMPHASIZE]);
    }
    Color CompBackgroundNeutral() const
    {
        return ConvertJsValueToColor(colors_[COMP_BACKGROUND_NEUTRAL]);
    }
    Color CompEmphasizeSecondary() const
    {
        return ConvertJsValueToColor(colors_[COMP_EMPHASIZE_SECONDARY]);
    }
    Color CompEmphasizeTertiary() const
    {
        return ConvertJsValueToColor(colors_[COMP_EMPHASIZE_TERTIARY]);
    }
    Color CompDivider() const
    {
        return ConvertJsValueToColor(colors_[COMP_DIVIDER]);
    }
    Color CompCommonContrary() const
    {
        return ConvertJsValueToColor(colors_[COMP_COMMON_CONTRARY]);
    }
    Color CompBackgroundFocus() const
    {
        return ConvertJsValueToColor(colors_[COMP_BACKGROUND_FOCUS]);
    }
    Color CompFocusedPrimary() const
    {
        return ConvertJsValueToColor(colors_[COMP_FOCUSED_PRIMARY]);
    }
    Color CompFocusedSecondary() const
    {
        return ConvertJsValueToColor(colors_[COMP_FOCUSED_SECONDARY]);
    }
    Color CompFocusedTertiary() const
    {
        return ConvertJsValueToColor(colors_[COMP_FOCUSED_TERTIARY]);
    }

    Color InteractiveHover() const
    {
        return ConvertJsValueToColor(colors_[INTERACTIVE_HOVER]);
    }
    Color InteractivePressed() const
    {
        return ConvertJsValueToColor(colors_[INTERACTIVE_PRESSED]);
    }
    Color InteractiveFocus() const
    {
        return ConvertJsValueToColor(colors_[INTERACTIVE_FOCUS]);
    }
    Color InteractiveActive() const
    {
        return ConvertJsValueToColor(colors_[INTERACTIVE_ACTIVE]);
    }
    Color InteractiveSelect() const
    {
        return ConvertJsValueToColor(colors_[INTERACTIVE_SELECT]);
    }
    Color InteractiveClick() const
    {
        return ConvertJsValueToColor(colors_[INTERACTIVE_CLICK]);
    }
private:
    Color ConvertJsValueToColor(const JSRef<JSVal>& jsValue) const
    {
        Color color;
        JSViewAbstract::ParseJsColor(jsValue, color);
        return color;
    }

    std::vector<JSRef<JSVal>> colors_;
};

class JSTheme {
public:
    JSTheme() = default;
    virtual ~JSTheme() = default;

    void SetColors(const JSThemeColors& colors)
    {
        colors_ = colors;
    }

    const JSThemeColors& Colors() const
    {
        return colors_;
    }
private:
    JSThemeColors colors_;
};

class JSThemeScope {
public:
    static std::map<int32_t, JSTheme> jsThemes;
    // indicates whether application use WithTheme containers
    inline static bool jsThemeScopeEnabled = false;
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_THEME_JS_THEME_H