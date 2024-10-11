/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "key_parser.h"
#include <cstring>
#include "acelite_config.h"
#include "js_config.h"
#include "keys.h"

namespace OHOS {
namespace ACELite {
#ifdef KEYWORD
#undef KEYWORD
#endif // KEYWORD

// Replace the struct keys.h "KEYWORD" sector for init
#ifdef ENABLE_KEY
#define KEYWORD(enumkey, keystr) {#keystr, K_##enumkey, static_cast<uint8_t>(strlen(#enumkey))},

static const struct {
    const char * const key;
} G_KEYWORD_INFO[KEYWORDS_MAX] = {
    {"UNKNOWN"},

#ifdef OHOS_ACELITE_KEYS_H
// include the keys.h again to redefine the "KEYWORD"
#undef OHOS_ACELITE_KEYS_H
#include "keys.h"
#endif // OHOS_ACELITE_KEYS_H
};
#undef KEYWORD
#endif

uint16_t KeyParser::ParseKeyId(const char *s, const size_t len)
{
    if (s == nullptr || len >= UINT16_MAX || len == 0) {
        return K_UNKNOWN;
    }
    /*
     * The string was compared should be the same as the js define,
     * with xxx-yyy instead the xxx_yyy,
     * but the enum type use K_xxx_yyy for Id return
     */
    switch (*s++) {
            // clang-format off
        case 'a':
#if FEATURE_COMPONENT_TEXT_SPANNABLE
            if (!strcmp(s, "bsolutesizespanstart")) {
                return K_ABSOLUTESIZESPANSTART;
            }
            if (!strcmp(s, "bsolutesizespanend")) {
                return K_ABSOLUTESIZESPANEND;
            }
            if (!strcmp(s, "bsolutesizespansize")) {
                return K_ABSOLUTESIZESPANSIZE;
            }
#endif // FEATURE_COMPONENT_TEXT_SPANNABLE
            if (!strcmp(s, "lignItems")) {
                return K_ALIGN_ITEMS;
            }
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
            if (!strcmp(s, "nalog-clock")) {
                return K_ANALOG_CLOCK;
            }
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
            if (!strcmp(s, "nimationDelay")) {
                return K_ANIMATION_DELAY;
            }
            if (!strcmp(s, "nimationDuration")) {
                return K_ANIMATION_DURATION;
            }
            if (!strcmp(s, "nimationFillMode")) {
                return K_ANIMATION_FILL_MODE;
            }
            if (!strcmp(s, "nimationIterationCount")) {
                return K_ANIMATION_ITERATION_COUNT;
            }
            if (!strcmp(s, "nimationName")) {
                return K_ANIMATION_NAME;
            }
            if (!strcmp(s, "nimationTimingFunction")) {
                return K_ANIMATION_TIMING_FUNCTION;
            }
            if (!strcmp(s, "rc")) {
                return K_ARC;
            }
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "utoplay")) {
                return K_AUTOPLAY;
            }
#endif // FEATURE_COMPONENT_VIDEO
            break;
        case 'b':
            if (!strcmp(s, "ackgroundColor")) {
                return K_BACKGROUND_COLOR;
            }
#if FEATURE_COMPONENT_TEXT_SPANNABLE
            if (!strcmp(s, "ackgroundcolorspancolor")) {
                return K_BACKGROUNDCOLORSPANCOLOR;
            }
            if (!strcmp(s, "ackgroundcolorspanstart")) {
                return K_BACKGROUNDCOLORSPANSTART;
            }
            if (!strcmp(s, "ackgroundcolorspanend")) {
                return K_BACKGROUNDCOLORSPANEND;
            }
#endif // FEATURE_COMPONENT_TEXT_SPANNABLE
            if (!strcmp(s, "ackgroundImage")) {
                return K_BACKGROUND_IMAGE;
            }
            if (!strcmp(s, "lockColor")) {
                return K_BLOCK_COLOR;
            }
            if (!strcmp(s, "orderBottomColor")) {
                return K_BORDER_BOTTOM_COLOR;
            }
            if (!strcmp(s, "orderLeftColor")) {
                return K_BORDER_LEFT_COLOR;
            }
            if (!strcmp(s, "orderRightColor")) {
                return K_BORDER_RIGHT_COLOR;
            }
            if (!strcmp(s, "orderTopColor")) {
                return K_BORDER_TOP_COLOR;
            }
            if (!strcmp(s, "orderColor")) {
                return K_BORDER_COLOR;
            }
            if (!strcmp(s, "orderRadius")) {
                return K_BORDER_RADIUS;
            }
            if (!strcmp(s, "orderBottomWidth")) {
                return K_BORDER_BOTTOM_WIDTH;
            }
            if (!strcmp(s, "orderLeftWidth")) {
                return K_BORDER_LEFT_WIDTH;
            }
            if (!strcmp(s, "orderRightWidth")) {
                return K_BORDER_RIGHT_WIDTH;
            }
            if (!strcmp(s, "orderTopWidth")) {
                return K_BORDER_TOP_WIDTH;
            }
            if (!strcmp(s, "orderWidth")) {
                return K_BORDER_WIDTH;
            }
            if (!strcmp(s, "reak")) {
                return K_BREAK;
            }
            break;
        case 'c':
#if (FEATURE_COMPONENT_CAMERA == 1)
            if (!strcmp(s, "amera")) {
                return K_CAMERA;
            }
#endif // FEATURE_COMPONENT_CAMERA
#if (FEATURE_COMPONENT_CANVAS == 1)
            if (!strcmp(s, "anvas")) {
                return K_CANVAS;
            }
#endif // FEATURE_COMPONENT_CANVAS
            if (!strcmp(s, "enter")) {
                return K_CENTER;
            }
            if (!strcmp(s, "enterX")) {
                return K_CENTER_X;
            }
            if (!strcmp(s, "enterY")) {
                return K_CENTER_Y;
            }
            if (!strcmp(s, "olor")) {
                return K_COLOR;
            }
            if (!strcmp(s, "olumn")) {
                return K_COLUMN;
            }
            if (!strcmp(s, "olumn-reverse")) {
                return K_COLUMN_REVERSE;
            }
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "ontrols")) {
                return K_CONTROLS;
            }
#endif // FEATURE_COMPONENT_VIDEO
            if (!strcmp(s, "hange")) {
                return K_CHANGE;
            }
            if (!strcmp(s, "heckbox")) {
                return K_CHECKBOX;
            }
            if (!strcmp(s, "hecked")) {
                return K_CHECKED;
            }
            if (!strcmp(s, "lick")) {
                return K_CLICK;
            }
            if (!strcmp(s, "hart")) {
                return K_CHART;
            }
            if (!strcmp(s, "lip")) {
                return K_CLIP;
            }
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
            if (!strcmp(s, "lock-hand")) {
                return K_CLOCK_HAND;
            }
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
            if (!strcmp(s, "ontain")) {
                return K_CONTAIN;
            }
            if (!strcmp(s, "over")) {
                return K_COVER;
            }
            break;
        case 'd':
            if (!strcmp(s, "atasets")) {
                return K_DATASETS;
            }
#if (FEATURE_DATE_FORMAT == 1)
            if (!strcmp(s, "ay")) {
                return K_DAY;
            }
#endif
#if (FEATURE_COMPONENT_DATE_PICKER == 1)
            if (!strcmp(s, "ate")) {
                return K_DATE;
            }
#endif // FEATURE_COMPONENT_DATE_PICKER
            if (!strcmp(s, "irection")) {
                return K_DIRECTION;
            }
            if (!strcmp(s, "isplay")) {
                return K_DISPLAY;
            }
            if (!strcmp(s, "iv")) {
                return K_DIV;
            }
            if (!strcmp(s, "rag")) {
                return K_DRAG;
            }
            if (!strcmp(s, "uration")) {
                return K_DURATION;
            }
            break;
        case 'e':
            if (!strcmp(s, "ase-in")) {
                return K_EASE_IN;
            }
            if (!strcmp(s, "ase-in-out")) {
                return K_EASE_IN_OUT;
            }
            if (!strcmp(s, "ase-out")) {
                return K_EASE_OUT;
            }
            if (!strcmp(s, "llipsis")) {
                return K_ELLIPSIS;
            }
#if (FEATURE_COMPONENT_DATE_PICKER == 1)
            if (!strcmp(s, "nd")) {
                return K_END;
            }
#endif // FEATURE_COMPONENT_DATE_PICKER
#if ((FEATURE_COMPONENT_CAMERA == 1) || (FEATURE_COMPONENT_VIDEO == 1))
            if (!strcmp(s, "rror")) {
                return K_ERROR;
            }
#endif // ((FEATURE_COMPONENT_CAMERA == 1) || (FEATURE_COMPONENT_VIDEO == 1))
            if (!strcmp(s, "xpand")) {
                return K_EXPAND;
            }
            break;
        case 'f':
            if (!strcmp(s, "alse")) {
                return K_FALSE;
            }
            if (!strcmp(s, "ill")) {
                return K_FILL;
            }
            if (!strcmp(s, "illmode")) {
                return K_FILL_MODE;
            }
            if (!strcmp(s, "ixedsize")) {
                return K_FIXED_SIZE;
            }
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "inish")) {
                return K_FINISH;
            }
#endif // FEATURE_COMPONENT_VIDEO
            if (!strcmp(s, "itOriginalSize")) {
                return K_FIT_ORIGINAL_SIZE;
            }
            if (!strcmp(s, "lex-end")) {
                return K_FLEX_END;
            }
            if (!strcmp(s, "lex-start")) {
                return K_FLEX_START;
            }
            if (!strcmp(s, "lexDirection")) {
                return K_FLEX_DIRECTION;
            }
            if (!strcmp(s, "lexWrap")) {
                return K_FLEX_WRAP;
            }
            if (!strcmp(s, "ontSize")) {
                return K_FONT_SIZE;
            }
            if (!strcmp(s, "ontFamily")) {
                return K_FONT_FAMILY;
            }
#if FEATURE_COMPONENT_TEXT_SPANNABLE
            if (!strcmp(s, "oregroundcolorspancolor")) {
                return K_FOREGROUNDCOLORSPANCOLOR;
            }
            if (!strcmp(s, "oregroundcolorspanstart")) {
                return K_FOREGROUNDCOLORSPANSTART;
            }
            if (!strcmp(s, "oregroundcolorspanend")) {
                return K_FOREGROUNDCOLORSPANEND;
            }
#endif // FEATURE_COMPONENT_TEXT_SPANNABLE
            if (!strcmp(s, "orwards")) {
                return K_FORWARDS;
            }
            break;
        case 'h':
            if (!strcmp(s, "eight")) {
                return K_HEIGHT;
            }
#if ((FEATURE_COMPONENT_ANALOG_CLOCK == 1) || (FEATURE_DATE_FORMAT == 1))
            if (!strcmp(s, "our")) {
                return K_HOUR;
            }
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
#if (FEATURE_DATE_FORMAT == 1)
            if (!strcmp(s, "our12")) {
                return K_HOUR12;
            }
#endif
            break;
        case 'i':
            if (!strcmp(s, "d")) {
                return K_ID;
            }
            if (!strcmp(s, "mage")) {
                return K_IMAGE;
            }
            if (!strcmp(s, "mage-animator")) {
                return K_IMAGE_ANIMATOR;
            }
            if (!strcmp(s, "mages")) {
                return K_IMAGES;
            }
            if (!strcmp(s, "nput")) {
                return K_INPUT;
            }
            if (!strcmp(s, "ndex")) {
                return K_INDEX;
            }
            if (!strcmp(s, "temselected")) {
                return K_ITEM_SELECTED;
            }
            if (!strcmp(s, "teration")) {
                return K_ITERATION;
            }
            break;
        case 'j':
            if (!strcmp(s, "ustifyContent")) {
                return K_JUSTIFY_CONTENT;
            }
            break;
#ifdef JS_EXTRA_EVENT_SUPPORT
        case 'k':
            if (!strcmp(s, "ey")) {
                return K_KEY;
            }
            break;
#endif
        case 'l':
            if (!strcmp(s, "abels")) {
                return K_LABELS;
            }
            if (!strcmp(s, "arger")) {
                return K_LARGER;
            }
            if (!strcmp(s, "eft")) {
                return K_LEFT;
            }
            if (!strcmp(s, "etterSpacing")) {
                return K_LETTER_SPACING;
            }
#if FEATURE_COMPONENT_TEXT_SPANNABLE
            if (!strcmp(s, "inebackgroundcolorspancolor")) {
                return K_LINEBACKGROUNDCOLORSPANCOLOR;
            }
            if (!strcmp(s, "inebackgroundcolorspanstart")) {
                return K_LINEBACKGROUNDCOLORSPANSTART;
            }
            if (!strcmp(s, "inebackgroundcolorspanend")) {
                return K_LINEBACKGROUNDCOLORSPANEND;
            }
#endif // FEATURE_COMPONENT_TEXT_SPANNABLE
            if (!strcmp(s, "ineHeight")) {
                return K_LINE_HEIGHT;
            }
            if (!strcmp(s, "ist")) {
                return K_LIST;
            }
            if (!strcmp(s, "ist-item")) {
                return K_LIST_ITEM;
            }
            if (!strcmp(s, "ongpress")) {
                return K_LONGPRESS;
            }
            if (!strcmp(s, "oop")) {
                return K_LOOP;
            }
#if (FEATURE_DATE_FORMAT == 1)
            if (!strcmp(s, "ong")) {
                return K_LONG;
            }
#endif
            break;
        case 'm':
            if (!strcmp(s, "argin")) {
                return K_MARGIN;
            }
            if (!strcmp(s, "arginBottom")) {
                return K_MARGIN_BOTTOM;
            }
            if (!strcmp(s, "arginLeft")) {
                return K_MARGIN_LEFT;
            }
            if (!strcmp(s, "arginRight")) {
                return K_MARGIN_RIGHT;
            }
            if (!strcmp(s, "arginTop")) {
                return K_MARGIN_TOP;
            }
            if (!strcmp(s, "arquee")) {
                return K_MARQUEE;
            }
            if (!strcmp(s, "ax")) {
                return K_MAX;
            }
#if (FEATURE_COMPONENT_EDITTEXT == 1)
            if (!strcmp(s, "axlength")) {
                return K_MAX_LENGTH;
            }
#endif // FEATURE_COMPONENT_EDITTEXT
            if (!strcmp(s, "in")) {
                return K_MIN;
            }
            if (!strcmp(s, "ini")) {
                return K_MINI;
            }
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "uted")) {
                return K_MUTED;
            }
#endif // FEATURE_COMPONENT_VIDEO
#if (FEATURE_NUMBER_FORMAT == 1)
            if (!strcmp(s, "inimumFractionDigits")) {
                return K_MINIMUMFRACTIONDIGITS;
            }
            if (!strcmp(s, "aximumFractionDigits")) {
                return K_MAXIMUMFRACTIONDIGITS;
            }
#endif
#if (FEATURE_DATE_FORMAT == 1)
            if (!strcmp(s, "inute")) {
                return K_MINUTE;
            }
            if (!strcmp(s, "onth")) {
                return K_MONTH;
            }
#endif
            break;
        case 'n':
            if (!strcmp(s, "ame")) {
                return K_NAME;
            }
            if (!strcmp(s, "one")) {
                return K_NONE;
            }
#if (FEATURE_DATE_FORMAT == 1)
            if (!strcmp(s, "umeric")) {
                return K_NUMERIC;
            }
#endif
            break;
        case 'o':
            if (!strcmp(s, "bjectFit")) {
                return K_OBJECT_FIT;
            }
            if (!strcmp(s, "pacity")) {
                return K_OPACITY;
            }
            if (!strcmp(s, "ptions")) {
                return K_OPTIONS;
            }
            break;
        case 'p':
            if (!strcmp(s, "adding")) {
                return K_PADDING;
            }
            if (!strcmp(s, "addingBottom")) {
                return K_PADDING_BOTTOM;
            }
            if (!strcmp(s, "addingLeft")) {
                return K_PADDING_LEFT;
            }
            if (!strcmp(s, "addingRight")) {
                return K_PADDING_RIGHT;
            }
            if (!strcmp(s, "addingTop")) {
                return K_PADDING_TOP;
            }
#if (FEATURE_COMPONENT_EDITTEXT == 1)
            if (!strcmp(s, "assword")) {
                return K_PASSWORD;
            }
#endif // FEATURE_COMPONENT_EDITTEXT
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "ause")) {
                return K_PAUSE;
            }
#endif // FEATURE_COMPONENT_VIDEO
            if (!strcmp(s, "ercent")) {
                return K_PERCENT;
            }
            if (!strcmp(s, "icker-view")) {
                return K_PICKER_VIEW;
            }
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
            if (!strcmp(s, "ivotX")) {
                return K_PIVOT_X;
            }
            if (!strcmp(s, "ivotY")) {
                return K_PIVOT_Y;
            }
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
#if (FEATURE_COMPONENT_EDITTEXT == 1)
            if (!strcmp(s, "laceholder")) {
                return K_PLACEHOLDER;
            }
            if (!strcmp(s, "laceholderColor")) {
                return K_PLACEHOLDER_COLOR;
            }
#endif // FEATURE_COMPONENT_EDITTEXT
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "repared")) {
                return K_PREPARED;
            }
#endif // FEATURE_COMPONENT_VIDEO
            if (!strcmp(s, "rogress")) {
                return K_PROGRESS;
            }
            break;
#if (FEATURE_COMPONENT_QRCODE == 1)
        case 'q':
            if (!strcmp(s, "rcode")) {
                return K_QRCODE;
            }
            break;
#endif // FEATURE_COMPONENT_QRCODE
        case 'r':
            if (!strcmp(s, "adio")) {
                return K_RADIO;
            }
            if (!strcmp(s, "adius")) {
                return K_RADIUS;
            }
            if (!strcmp(s, "ange")) {
                return K_RANGE;
            }
            if (!strcmp(s, "ef")) {
                return K_REF;
            }
#if FEATURE_COMPONENT_TEXT_SPANNABLE
            if (!strcmp(s, "elativesizespanstart")) {
                return K_RELATIVESIZESPANSTART;
            }
            if (!strcmp(s, "elativesizespanend")) {
                return K_RELATIVESIZESPANEND;
            }
            if (!strcmp(s, "elativesizespansize")) {
                return K_RELATIVESIZESPANSIZE;
            }
#endif // FEATURE_COMPONENT_TEXT_SPANNABLE
            if (!strcmp(s, "everse")) {
                return K_REVERSE;
            }
            if (!strcmp(s, "ight")) {
                return K_RIGHT;
            }
            if (!strcmp(s, "otate")) {
                return K_ROTATE;
            }
            if (!strcmp(s, "ow")) {
                return K_ROW;
            }
            if (!strcmp(s, "ow-reverse")) {
                return K_ROW_REVERSE;
            }
            break;
        case 's':
            if (!strcmp(s, "cale-down")) {
                return K_SCALE_DOWN;
            }
            if (!strcmp(s, "crollamount")) {
                return K_SCROLLAMOUNT;
            }
            if (!strcmp(s, "crolldelay")) {
                return K_SCROLLDELAY;
            }
            if (!strcmp(s, "crollend")) {
                return K_SCROLLEND;
            }
            if (!strcmp(s, "crollstart")) {
                return K_SCROLLSTART;
            }
            if (!strcmp(s, "crolltop")) {
                return K_SCROLLTOP;
            }
            if (!strcmp(s, "crollbottom")) {
                return K_SCROLLBOTTOM;
            }
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
            if (!strcmp(s, "ec")) {
                return K_SEC;
            }
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "eeked")) {
                return K_SEEKED;
            }
            if (!strcmp(s, "eeking")) {
                return K_SEEKING;
            }
#endif // FEATURE_COMPONENT_VIDEO
            if (!strcmp(s, "elected")) {
                return K_SELECTED;
            }
            if (!strcmp(s, "electedColor")) {
                return K_SELECTED_COLOR;
            }
            if (!strcmp(s, "electedFontFamily")) {
                return K_SELECTED_FONT_FAMILY;
            }
            if (!strcmp(s, "electedFontSize")) {
                return K_SELECTED_FONT_SIZE;
            }
            if (!strcmp(s, "how")) {
                return K_SHOW;
            }
#if (FEATURE_DATE_FORMAT == 1)
            if (!strcmp(s, "hort")) {
                return K_SHORT;
            }
#endif
            if (!strcmp(s, "lider")) {
                return K_SLIDER;
            }
            if (!strcmp(s, "pace-around")) {
                return K_SPACE_AROUND;
            }
            if (!strcmp(s, "pace-between")) {
                return K_SPACE_BETWEEN;
            }
            if (!strcmp(s, "pace-evenly")) {
                return K_SPACE_EVENLY;
            }
#if FEATURE_COMPONENT_TEXT_SPANNABLE
            if (!strcmp(s, "pannablestyle")) {
                return K_SPANNABLESTYLE;
            }
            if (!strcmp(s, "pannablestart")) {
                return K_SPANNABLESTART;
            }
            if (!strcmp(s, "pannableend")) {
                return K_SPANNABLEEND;
            }
#endif // FEATURE_COMPONENT_TEXT_SPANNABLE
            if (!strcmp(s, "rc")) {
                return K_SRC;
            }
            if (!strcmp(s, "tack")) {
                return K_STACK;
            }
            if (!strcmp(s, "top")) {
                return K_STOP;
            }
#if ((FEATURE_COMPONENT_DATE_PICKER == 1) || (FEATURE_COMPONENT_VIDEO == 1))
            if (!strcmp(s, "tart")) {
                return K_START;
            }
#endif // ((FEATURE_COMPONENT_DATE_PICKER == 1) || (FEATURE_COMPONENT_VIDEO == 1))
            if (!strcmp(s, "tartAngle")) {
                return K_START_ANGLE;
            }
            if (!strcmp(s, "tandard")) {
                return K_STANDARD;
            }
            if (!strcmp(s, "trokeWidth")) {
                return K_STROKE_WIDTH;
            }
#if (FEATURE_NUMBER_FORMAT == 1)
            if (!strcmp(s, "tyle")) {
                return K_STYLE;
            }
#endif // FEATURE_NUMBER_FORMAT
            if (!strcmp(s, "wipe")) {
                return K_SWIPE;
            }
            if (!strcmp(s, "wiper")) {
                return K_SWIPER;
            }
            if (!strcmp(s, "witch")) {
                return K_SWITCH;
            }
            break;
        case 't':
#if (FEATURE_COMPONENT_TABS == 1)
            if (!strcmp(s, "ab-bar")) {
                return K_TAB_BAR;
            }
            if (!strcmp(s, "ab-content")) {
                return K_TAB_CONTENT;
            }
            if (!strcmp(s, "abs")) {
                return K_TABS;
            }
#endif // FEATURE_COMPONENT_TABS
            if (!strcmp(s, "ime")) {
                return K_TIME;
            }
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "imeupdate")) {
                return K_TIME_UPDATE;
            }
#endif // FEATURE_COMPONENT_VIDEO
            if (!strcmp(s, "op")) {
                return K_TOP;
            }
            if (!strcmp(s, "otalAngle")) {
                return K_TOTAL_ANGLE;
            }
            if (!strcmp(s, "ouchend")) {
                return K_TOUCHEND;
            }
            if (!strcmp(s, "ouchmove")) {
                return K_TOUCHMOVE;
            }
            if (!strcmp(s, "ouchstart")) {
                return K_TOUCHSTART;
            }
#ifdef JS_EXTRA_EVENT_SUPPORT
            if (!strcmp(s, "ouchcancel")) {
                return K_TOUCHCANCEL;
            }
#endif
            if (!strcmp(s, "ext")) {
                return K_TEXT;
            }
            if (!strcmp(s, "ranslateX")) {
                return K_TRANSLATE_X;
            }
            if (!strcmp(s, "ranslateY")) {
                return K_TRANSLATE_Y;
            }
            if (!strcmp(s, "rue")) {
                return K_TRUE;
            }
            if (!strcmp(s, "extAlign")) {
                return K_TEXT_ALIGN;
            }
            if (!strcmp(s, "extOverflow")) {
                return K_TEXT_OVERFLOW;
            }
#if ((FEATURE_COMPONENT_ANALOG_CLOCK == 1) || (FEATURE_COMPONENT_EDITTEXT == 1))
            if (!strcmp(s, "ype")) {
                return K_TYPE;
            }
#endif // (FEATURE_COMPONENT_ANALOG_CLOCK == 1) || (FEATURE_COMPONENT_EDITTEXT == 1)
            break;
        case 'u':
#if (FEATURE_NUMBER_FORMAT == 1)
            if (!strcmp(s, "seGrouping")) {
                return K_USEGROUP;
            }
#endif // FEATURE_NUMBER_FORMAT
            break;
        case 'v':
            if (!strcmp(s, "alue")) {
                return K_VALUE;
            }
            if (!strcmp(s, "ertical")) {
                return K_VERTICAL;
            }
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "ideo")) {
                return K_VIDEO;
            }
#endif // FEATURE_COMPONENT_VIDEO
            break;
        case 'w':
            if (!strcmp(s, "idth")) {
                return K_WIDTH;
            }
            if (!strcmp(s, "rap")) {
                return K_WRAP;
            }
            break;
        case '2':
#if (FEATURE_DATE_FORMAT == 1)
            if (!strcmp(s, "-digit")) {
                return K_DIGIT2;
            }
            break;
#endif
        default:
            break;
            // clang-format on
    }
    return K_UNKNOWN;
}

uint16_t KeyParser::ParseKeyId(const char * const s)
{
    if (s == nullptr) {
        return K_UNKNOWN;
    }

    size_t len = strlen(s);
    if (len >= UINT16_MAX) {
        return K_UNKNOWN;
    }
    return ParseKeyId(s, len);
}

bool KeyParser::IsKeyValid(uint16_t id)
{
    return ((id > K_UNKNOWN) && (id < KEYWORDS_MAX));
}

const char *KeyParser::GetKeyById(uint16_t id)
{
    if (!IsKeyValid(id)) {
        return "UNKNOWN";
    }
#if (ENABLE_KEY == 1)
    return G_KEYWORD_INFO[id].key;
#else
    return "UNKNOWN";
#endif // ENABLE_KEY
}
} // namespace ACELite
} // namespace OHOS
