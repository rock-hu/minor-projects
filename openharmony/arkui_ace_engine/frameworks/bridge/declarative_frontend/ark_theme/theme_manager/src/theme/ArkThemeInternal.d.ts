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

/**
 * Defines the struct of internal Theme.
 *
 * @interface ThemeInternal
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @crossplatform
 * @atomicservice
 * @since 12
 */
declare interface ThemeInternal extends Theme {
    /**
    *  Define tokens associated with component shape.
    *
    * @type { Shapes }
    * @syscap SystemCapability.ArkUI.ArkUI.Full
    * @crossplatform
    * @atomicservice
    * @since 12
    */
    shapes: Shapes;

    /**
    *  Define tokens associated with Text component.
    *
    * @type { Typography }
    * @syscap SystemCapability.ArkUI.ArkUI.Full
    * @crossplatform
    * @atomicservice
    * @since 12
    */
    typography: Typography;
}

/**
 * Defines the struct of CornerRadius.
 *
 * @interface CornerRadius
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @crossplatform
 * @atomicservice
 * @since 12
 */
declare interface CornerRadius {
    none: Length;
    level1: Length;
    level2: Length;
    level3: Length;
    level4: Length;
    level5: Length;
    level6: Length;
    level7: Length;
    level8: Length;
    level9: Length;
    level10: Length;
    level11: Length;
    level12: Length;
    level16: Length;
}

/**
 * Defines the struct of Paddings.
 *
 * @interface Paddings
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @crossplatform
 * @atomicservice
 * @since 12
 */
declare interface Paddings {
    level0: Length;
    level1: Length;
    level2: Length;
    level3: Length;
    level4: Length;
    level5: Length;
    level6: Length;
    level7: Length;
    level8: Length;
    level9: Length;
    level10: Length;
    level11: Length;
    level12: Length;
    level16: Length;
    level24: Length;
    level32: Length;
    level36: Length;
}

/**
 * Defines the struct of Borders.
 *
 * @interface Borders
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @crossplatform
 * @atomicservice
 * @since 12
 */
declare interface Borders {
    none: Length;
    xs: Length;
    s: Length;
    m: Length;
    l: Length;
    xl: Length;
}

/**
 * Defines the struct of Outlines.
 *
 * @interface Outlines
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @crossplatform
 * @atomicservice
 * @since 12
 */
declare interface Outlines {
    none: Dimension;
    xs: Dimension;
    s: Dimension;
    m: Dimension;
    l: Dimension;
    xl: Dimension;
}

/**
 * Defines the struct of Shapes.
 *
 * @interface Shapes
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @crossplatform
 * @atomicservice
 * @since 12
 */
declare interface Shapes {
    
    /**
    * CornerRadius Value Set. 
    *
    * @type { CornerRadius }
    * @syscap SystemCapability.ArkUI.ArkUI.Full
    * @crossplatform
    * @atomicservice
    * @since 12
    */
    cornerRadius: CornerRadius;

    /**
    * Paddings Value Set. 
    *
    * @type { Paddings }
    * @syscap SystemCapability.ArkUI.ArkUI.Full
    * @crossplatform
    * @atomicservice
    * @since 12
    */
    paddings: Paddings;

    /**
    * Borders Value Set. 
    *
    * @type { Borders }
    * @syscap SystemCapability.ArkUI.ArkUI.Full
    * @crossplatform
    * @atomicservice
    * @since 12
    */
    borders: Borders;

    /**
    * Outlines Value Set. 
    *
    * @type { Outlines }
    * @syscap SystemCapability.ArkUI.ArkUI.Full
    * @crossplatform
    * @atomicservice
    * @since 12
    */
    outlines: Outlines;
}

/**
 * Defines the struct of TypographyStyle.
 *
 * @interface TypographyStyle
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @crossplatform
 * @atomicservice
 * @since 12
 */
declare interface TypographyStyle {
    weight: FontWeight | number | string;
    size: Length;
}

/**
 * Defines the struct of Typography.
 *
 * @interface Typography
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @crossplatform
 * @atomicservice
 * @since 12
 */
declare interface Typography {

    //display
    displayLarge: TypographyStyle;
    displayMiddle: TypographyStyle;
    displaySmall: TypographyStyle;

    // Title
    titleLarge: TypographyStyle;
    titleMiddle: TypographyStyle;
    titleSmall: TypographyStyle;

    // Subtitle
    subtitleLarge: TypographyStyle;
    subtitleMiddle: TypographyStyle;
    subtitleSmall: TypographyStyle;

    // Body
    bodyLarge: TypographyStyle;
    bodyMiddle: TypographyStyle;
    bodySmall: TypographyStyle;

    // Caption
    captionLarge: TypographyStyle;
    captionMiddle: TypographyStyle;
    captionSmall: TypographyStyle;
}