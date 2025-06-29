# Enums

> **NOTE**
>
> The initial APIs of this module are supported since API version 6. Newly added APIs will be marked with a superscript to indicate their earliest API version.

## WindowType<sup>7+</sup>

Enumerates the window types.

**System capability**: SystemCapability.WindowManager.WindowManager.Core

| Name                                 | Value| Description                                                                                    |
|-------------------------------------| ------ |----------------------------------------------------------------------------------------|
| TYPE_APP                            | 0      | Child window of an application.<br>**Model restriction**: This API can be used only in the FA model.                                                  |
| TYPE_SYSTEM_ALERT                   | 1      | System alert window.<br>**NOTE**<br>This property is supported since API version 7 and deprecated since API version 11.                               |
| TYPE_FLOAT<sup>9+</sup>             | 8      | Floating window.<br>**Model restriction**: This API can be used only in the stage model.<br>**Atomic service API**: This API can be used in atomic services since API version 12.|
| TYPE_DIALOG<sup>10+</sup>           | 16      | Modal window.<br>**Model restriction**: This API can be used only in the stage model.<br>**Atomic service API**: This API can be used in atomic services since API version 12.|
| TYPE_MAIN<sup>18+</sup>             | 32      | Main window of an application.<br>This window type cannot be used when creating a window. It is only for reading purposes in the return value of the **getWindowProperties** API.                              |

## AvoidAreaType<sup>7+</sup>

Enumerates the types of areas that window content should avoid.

**System capability**: SystemCapability.WindowManager.WindowManager.Core

**Atomic service API**: This API can be used in atomic services since API version 11.

| Name                            | Value  | Description                                                        |
| -------------------------------- | ---- | ------------------------------------------------------------ |
| TYPE_SYSTEM                      | 0    | Default area of the system. <!--RP11-->It contains the status bar and three-button navigation bar.<!--RP11End-->       |
| TYPE_CUTOUT                      | 1    | Notch area.                                                             |
| TYPE_SYSTEM_GESTURE<sup>9+</sup> | 2    | Gesture area. Currently, no device has this type of avoid area.                                   |
| TYPE_KEYBOARD<sup>9+</sup>       | 3    | Soft keyboard area.                                                             |
| TYPE_NAVIGATION_INDICATOR<sup>11+</sup> | 4    | Bottom navigation bar. <!--RP12-->OpenHarmony devices do not support this capability.<!--RP12End-->                             |

## Orientation<sup>9+</sup>

Enumerates the window orientations.

| Name                                 | Value  | Description                         |
| ------------------------------------- | ---- | ----------------------------- |
| UNSPECIFIED                           | 0    | Unspecified. The orientation is determined by the system.<br>**Atomic service API**: This API can be used in atomic services since API version 12.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core|
| PORTRAIT                              | 1    | Portrait.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core<br>**Atomic service API**: This API can be used in atomic services since API version 11.|
| LANDSCAPE                             | 2    | Landscape.<br>**Atomic service API**: This API can be used in atomic services since API version 12.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core|
| PORTRAIT_INVERTED                     | 3    | Reverse portrait.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core<br>**Atomic service API**: This API can be used in atomic services since API version 12.|
| LANDSCAPE_INVERTED                    | 4    | Reverse landscape.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core<br>**Atomic service API**: This API can be used in atomic services since API version 12.|
| AUTO_ROTATION                         | 5    | Automatically rotates with the sensor. The orientation can be portrait, landscape, reverse portrait, or reverse landscape.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core<br>**Atomic service API**: This API can be used in atomic services since API version 11.|
| AUTO_ROTATION_PORTRAIT                | 6    | Automatically rotates with the sensor in the vertical direction. The orientation can be portrait or reverse portrait.<br>**Atomic service API**: This API can be used in atomic services since API version 12.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core|
| AUTO_ROTATION_LANDSCAPE               | 7    | Automatically rotates with the sensor in the horizontal direction. The orientation can be landscape or reverse landscape.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core<br>**Atomic service API**: This API can be used in atomic services since API version 12.|
| AUTO_ROTATION_RESTRICTED              | 8    | Automatically rotates with the sensor, under the restriction of the rotation switch in the Control Panel. The orientation can be portrait, landscape, reverse portrait, or reverse landscape.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core<br>**Atomic service API**: This API can be used in atomic services since API version 12.|
| AUTO_ROTATION_PORTRAIT_RESTRICTED     | 9    | Automatically rotates with the sensor, under the restriction of the rotation switch in the Control Panel. The orientation can be portrait or reverse portrait.<br>**Atomic service API**: This API can be used in atomic services since API version 12.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core|
| AUTO_ROTATION_LANDSCAPE_RESTRICTED    | 10   | Automatically rotates with the sensor, under the restriction of the rotation switch in the Control Panel. The orientation can be landscape or reverse landscape.<br>**Atomic service API**: This API can be used in atomic services since API version 12.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core|
| LOCKED                                | 11   | Locked.<br>**Atomic service API**: This API can be used in atomic services since API version 12.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core|
| AUTO_ROTATION_UNSPECIFIED<sup>12+</sup>        | 12   | Automatically rotates with the sensor, under the restriction of the rotation switch in the Control Panel. The orientation that can be rotated to is determined by the system. For example, the window can rotate to portrait, landscape, or reverse landscape, but not reverse portrait, on a certain device.<br>**Atomic service API**: This API can be used in atomic services since API version 12.<br>**System capability**: SystemCapability.Window.SessionManager|
| USER_ROTATION_PORTRAIT<sup>12+</sup>           | 13   | Temporarily rotates to portrait mode, and then automatically rotates with the sensor, under the restriction of the rotation switch in the Control Panel. The orientation that can be rotated to is determined by the system.<br>**Atomic service API**: This API can be used in atomic services since API version 12.<br>**System capability**: SystemCapability.Window.SessionManager|
| USER_ROTATION_LANDSCAPE<sup>12+</sup>          | 14   | Temporarily rotates to landscape mode, and then automatically rotates with the sensor, under the restriction of the rotation switch in the Control Panel. The orientation that can be rotated to is determined by the system.<br>**Atomic service API**: This API can be used in atomic services since API version 12.<br>**System capability**: SystemCapability.Window.SessionManager|
| USER_ROTATION_PORTRAIT_INVERTED<sup>12+</sup>  | 15   | Temporarily rotates to reverse portrait mode, and then automatically rotates with the sensor, under the restriction of the rotation switch in the Control Panel. The orientation that can be rotated to is determined by the system.<br>**Atomic service API**: This API can be used in atomic services since API version 12.<br>**System capability**: SystemCapability.Window.SessionManager|
| USER_ROTATION_LANDSCAPE_INVERTED<sup>12+</sup> | 16   | Temporarily rotates to reverse landscape mode, and then automatically rotates with the sensor, under the restriction of the rotation switch in the Control Panel. The orientation that can be rotated to is determined by the system.<br>**Atomic service API**: This API can be used in atomic services since API version 12.<br>**System capability**: SystemCapability.Window.SessionManager|
| FOLLOW_DESKTOP<sup>12+</sup>                   | 17   | Follows the orientation of the home screen.<br>**Atomic service API**: This API can be used in atomic services since API version 12.<br>**System capability**: SystemCapability.Window.SessionManager|

## RectChangeReason<sup>12+</sup>

Enumerates the reasons for window rectangle (position and size) changes.

**System capability**: SystemCapability.Window.SessionManager

**Atomic service API**: This API can be used in atomic services since API version 12.

| Name                 | Value  | Description                                                        |
| --------------------- | ---- | ------------------------------------------------------------ |
| UNDEFINED                 | 0    | Default value.                                                  |
| MAXIMIZE                | 1    | The window is maximized.                                                  |
| RECOVER              | 2    | The window is restored to the previous state.                                                  |
| MOVE | 3    | The window is moved.|
| DRAG  | 4    | The window is zoomed in or out by dragging.|
| DRAG_START  | 5    | The window starts zooming in or out.|
| DRAG_END  | 6    | The window finishes zooming in or out.|

## ColorSpace<sup>8+</sup>

Enumerates the color spaces.

**Atomic service API**: This API can be used in atomic services since API version 12.

**System capability**: SystemCapability.WindowManager.WindowManager.Core

| Name      | Value| Description          |
| ---------- | ------ | -------------- |
| DEFAULT    | 0      | Default SRGB gamut.|
| WIDE_GAMUT | 1      | Wide-gamut.  |

## WindowEventType<sup>10+</sup>

Enumerates the window lifecycle states.

**Atomic service API**: This API can be used in atomic services since API version 11.

| Name      | Value| Description      |
| ---------- | ------ | ---------- |
| WINDOW_SHOWN      | 1      | The window is running in the foreground.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core|
| WINDOW_ACTIVE     | 2      | The window gains focus.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core|
| WINDOW_INACTIVE   | 3      | The window loses focus.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core|
| WINDOW_HIDDEN     | 4      | The window is running in the background.<br>**System capability**: SystemCapability.WindowManager.WindowManager.Core|
| WINDOW_DESTROYED<sup>11+</sup>  | 7      | The window is destroyed.<br>**System capability**: SystemCapability.Window.SessionManager|

## WindowStatusType<sup>11+</sup>

Enumerates the window modes.

**Atomic service API**: This API can be used in atomic services since API version 12.

**System capability**: SystemCapability.Window.SessionManager

| Name      | Value  | Description                         |
| ---------- | ---- | ----------------------------- |
| UNDEFINED  | 0    | The window mode is not defined by the application.      |
| FULL_SCREEN | 1    | The application is displayed in full screen.            |
| MAXIMIZE    | 2    | The application window is maximized.  |
| MINIMIZE    | 3    | The application window is minimized.  |
| FLOATING    | 4    | The application is displayed in a floating window.  |
| SPLIT_SCREEN  | 5    | The application is displayed in split-screen mode.  |

## MaximizePresentation<sup>12+</sup>

Enumerates the layout when the window is maximized.

**System capability**: SystemCapability.Window.SessionManager

| Name      | Value  | Description                         |
| ---------- | ---- | ----------------------------- |
| FOLLOW_APP_IMMERSIVE_SETTING  | 0    | The window, when maximized, follows the immersive layout of the application.<br>**Atomic service API**: This API can be used in atomic services since API version 12.      |
| EXIT_IMMERSIVE | 1    | The window, when maximized, exits the immersive layout.<br>**Atomic service API**: This API can be used in atomic services since API version 12.            |
| ENTER_IMMERSIVE    | 2    | The window, when maximized, transitions into the immersive layout, and the window title bar and dock bar are displayed when the cursor hovers over the hot zone.<br>**Atomic service API**: This API can be used in atomic services since API version 12.  |
| ENTER_IMMERSIVE_DISABLE_TITLE_AND_DOCK_HOVER<sup>14+</sup>    | 3    | The window, when maximized, transitions into the immersive layout, and the window title bar and dock bar are not displayed when the cursor hovers over the hot zone.<br>**Atomic service API**: This API can be used in atomic services since API version 14.  |

## WindowAnimationCurve<sup>20+</sup>

Enumerates the types of window animation curves.

**Atomic service API**: This API can be used in atomic services since API version 20.

**System capability**: SystemCapability.Window.SessionManager

| Name               | Value  | Description                                                        |
| ------------------- | ---- | ------------------------------------------------------------ |
| LINEAR              | 0    | The animation speed is constant from start to finish.<br>When this curve type is used, **duration** in [WindowAnimationConfig](arkts-apis-window-i.md#windowanimationconfig20) is mandatory.<br>When this curve type is used, **param** in [WindowAnimationConfig](arkts-apis-window-i.md#windowanimationconfig20) is optional and does not take effect.|
| INTERPOLATION_SPRING | 1    | Interpolator spring curve, an animation curve from 0 to 1, where the actual animation values are interpolated based on the curve. The animation duration is subject to the curve parameters, rather than the **duration** parameter in [WindowAnimationConfig](arkts-apis-window-i.md#windowanimationconfig20).<br>When this curve type is used, **duration** in [WindowAnimationConfig](arkts-apis-window-i.md#windowanimationconfig20) is optional and does not take effect.<br>When this curve type is used, **param** in [WindowAnimationConfig](arkts-apis-window-i.md#windowanimationconfig20) is mandatory.|

## WindowTransitionType<sup>20+</sup>

Enumerates the types of window transition animations.

**Atomic service API**: This API can be used in atomic services since API version 20.

**System capability**: SystemCapability.Window.SessionManager

| Name   | Value  | Description                      |
| ------- | ---- | -------------------------- |
| DESTROY | 0    | Transition animation when the window is destroyed.|

## WindowAnchor<sup>20+</sup>

Enumerates the window anchor points.

**Atomic service API**: This API can be used in atomic services since API version 20.

**System capability**: SystemCapability.Window.SessionManager

| Name      | Value  | Description                         |
| ---------- | ---- | ----------------------------- |
| TOP_START  | 0    | Top left corner of the window.|
| TOP  | 1    | Horizontal center point along the top edge of the window.|
| TOP_END  | 2    | Top right corner of the window.|
| START  | 3    | Vertical center point along the left edge of the window.|
| CENTER  | 4    | Center point of the window, both horizontally and vertically.|
| END  | 5    | Vertical center point along the right edge of the window.|
| BOTTOM_START  | 6    | Bottom left corner of the window.|
| BOTTOM  | 7    | Horizontal center point along the bottom edge of the window.|
| BOTTOM_END  | 8    | Bottom right corner of the window.|

## RotationChangeType<sup>19+</sup>

Enumerates the types of window rotation events.

**Atomic service API**: This API can be used in atomic services since API version 19.

**System capability**: SystemCapability.Window.SessionManager

| Name  |  Value| Description                  |
| ------ | ---- | --------------------- |
| WINDOW_WILL_ROTATE| 0 | The window is about to rotate.|
| WINDOW_DID_ROTATE | 1 | The window has finished rotating.|

## RectType<sup>19+</sup>

Enumerates the types of window rectangle coordinate systems.

**Atomic service API**: This API can be used in atomic services since API version 19.

**System capability**: SystemCapability.Window.SessionManager

| Name  | Value| Description                  |
| ------ | ---- | --------------------- |
| RELATIVE_TO_SCREEN | 0 | The window rectangle is relative to the screen coordinate system.|
| RELATIVE_TO_PARENT_WINDOW | 1 | The window rectangle is relative to the parent window coordinate system.|

## GlobalWindowMode<sup>20+</sup>

Enumerates the window modes.

**Atomic service API**: This API can be used in atomic services since API version 20.

**System capability**: SystemCapability.Window.SessionManager

| Name  | Value| Description                   |
| ------ | --- | ------------------------ |
| FULLSCREEN | 1 | Full-screen window.|
| SPLIT  | 1 << 1 | Split-screen window.|
| FLOAT   | 1 << 2  | Floating window.|
| PIP   | 1 << 3  | PiP.|

## WindowStageEventType<sup>9+</sup>

Enumerates the lifecycle states of a window stage.

**Model restriction**: This API can be used only in the stage model.

**System capability**: SystemCapability.WindowManager.WindowManager.Core

**Atomic service API**: This API can be used in atomic services since API version 11.

| Name      | Value| Description      |
| ---------- | ------ | ---------- |
| SHOWN      | 1      | The window stage is shown in the foreground, for example, when launching from the application icon, triggered whether it is the first launch or resuming from the background.|
| ACTIVE     | 2      | The window stage gains focus, for example, the state of the application window after handling a click event, or the state after the application is launched.|
| INACTIVE   | 3      | The window stage loses focus, for example, the state of the window that was in focus when a new application is opened or another window is clicked.|
| HIDDEN     | 4      | The window stage is running in the background, for example, when the application exists after swiping up or the application window is closed.|
| RESUMED<sup>11+</sup> | 5      | The window stage is interactive in the foreground, for example, when an application is opened and can interact with the user.|
| PAUSED<sup>11+</sup>  | 6      | The window stage is non-interactive in the foreground, for example, when the application enters the recent tasks screen after swiping up from the bottom of the screen.|

## ModalityType<sup>14+</sup>

Enumerates the modality types of the child window.

**System capability**: SystemCapability.Window.SessionManager

**Atomic service API**: This API can be used in atomic services since API version 14.

| Name                | Value     | Description      |
| -------------------- | ------ | ---------- |
| WINDOW_MODALITY      | 0      | Window-modal. Select this value when you want the parent window not to respond to user interactions.|
| APPLICATION_MODALITY | 1      | Application-modal. Select this value when you want both the parent window and all windows of other instances of the application not to respond to user interactions.<br> This API can be used only on 2-in-1 devices.|
