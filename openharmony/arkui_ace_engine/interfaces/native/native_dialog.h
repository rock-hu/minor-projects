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

#ifndef ARKUI_NATIVE_DIALOG_H
#define ARKUI_NATIVE_DIALOG_H

#include <stdbool.h>
#include "native_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerates the actions for triggering closure of the dialog box.
 *
 * @since 12
 */
typedef enum {
    /** Touching the system-defined Back button or pressing the Esc key. */
    DIALOG_DISMISS_BACK_PRESS = 0,
    /** Touching the mask. */
    DIALOG_DISMISS_TOUCH_OUTSIDE,
    /** 点击关闭按钮。*/
    DIALOG_DISMISS_CLOSE_BUTTON,
    /** 下拉关闭。*/
    DIALOG_DISMISS_SLIDE_DOWN,
} ArkUI_DismissReason;

/**
 * @brief Enumerates the level mode.
 *
 * @since 16
 */
typedef enum {
    /** overlay mode. */
    ARKUI_LEVEL_MODE_OVERLAY = 0,
    /** embedded mode. */
    ARKUI_LEVEL_MODE_EMBEDDED,
} ArkUI_LevelMode;
 
/**
 * @brief Enumerates the immersive mode.
 *
 * @since 16
 */
typedef enum {
    /** Mask covering the parent node. */
    ARKUI_IMMERSIVE_MODE_DEFAULT = 0,
    /** Mask extend safe area includes status bar and navigation bar. */
    ARKUI_IMMERSIVE_MODE_EXTEND,
} ArkUI_ImmersiveMode;

/**
 * @brief Invoked when the dialog box is closed.
 *
 * @since 12
 */
typedef bool (*ArkUI_OnWillDismissEvent)(int32_t reason);

/**
 * @brief Defines a struct for a dialog box dismiss event.
 *
 * @since 12
 */
typedef struct ArkUI_DialogDismissEvent ArkUI_DialogDismissEvent;

/**
 * @brief Defines a struct for the content object of a custom dialog box.
 *
 * @since 16
 */
typedef struct ArkUI_CustomDialogOptions ArkUI_CustomDialogOptions;

/**
 * @brief Provides the custom dialog box APIs for the native side.
 *
 * @version 1
 * @since 12
 */
typedef struct {
    /**
    * @brief Creates a custom dialog box and returns the pointer to the created dialog box.
    *
    * @note This method must be called before the <b>show</b> method.
    * @return Returns the pointer to the created custom dialog box; returns a null pointer if the creation fails.
    */
    ArkUI_NativeDialogHandle (*create)();
    /**
    * @brief Destroys a custom dialog box.
    *
    * @param handle Indicates the pointer to the custom dialog box controller.
    */
    void (*dispose)(ArkUI_NativeDialogHandle handle);
    /**
    * @brief Attaches the content of a custom dialog box.
    *
    * @note This method must be called before the <b>show</b> method.
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @param content Indicates the pointer to the root node of the custom dialog box content.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*setContent)(ArkUI_NativeDialogHandle handle, ArkUI_NodeHandle content);
    /**
    * @brief Detaches the content of a custom dialog box.
    *
    * @note This method must be called before the <b>show</b> method.
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*removeContent)(ArkUI_NativeDialogHandle handle);
    /**
    * @brief Sets the alignment mode for a custom dialog box.
    *
    * @note This method must be called before the <b>show</b> method.
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @param alignment Indicates the alignment mode. The parameter type is {@link ArkUI_Alignment}.
    * @param offsetX Indicates the horizontal offset of the custom dialog box. The value is a floating point number.
    * @param offsetY Indicates the vertical offset of the custom dialog box. The value is a floating point number.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*setContentAlignment)(ArkUI_NativeDialogHandle handle, int32_t alignment, float offsetX, float offsetY);
    /**
    * @brief Resets the alignment mode of a custom dialog box to its default settings.
    *
    * @note This method must be called before the <b>show</b> method.
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*resetContentAlignment)(ArkUI_NativeDialogHandle handle);
    /**
    * @brief Sets the modal mode for a custom dialog box.
    *
    * @note This method must be called before the <b>show</b> method.
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @param isModal Specifies whether the custom dialog box is a modal, which has a mask applied. The value
    * <b>true</b> means that the custom dialog box is a modal, and <b>false</b> means the opposite.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*setModalMode)(ArkUI_NativeDialogHandle handle, bool isModal);
    /**
    * @brief Specifies whether to allow users to touch the mask to dismiss the custom dialog box.
    *
    * @note This method must be called before the <b>show</b> method.
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @param autoCancel Specifies whether to allow users to touch the mask to dismiss the dialog box.
    * The value <b>true</b> means to allow users to do so, and <b>false</b> means the opposite.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*setAutoCancel)(ArkUI_NativeDialogHandle handle, bool autoCancel);
    /**
    * @brief Sets the mask for a custom dialog box.
    *
    * @note This method must be called before the <b>show</b> method.
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @param maskColor Indicates the mask color, in 0xARGB format.
    * @param maskRect Indicates the pointer to the mask area. Events outside the mask area are transparently
    * transmitted, and events within the mask area are not. The parameter type is {@link ArkUI_Rect}.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*setMask)(ArkUI_NativeDialogHandle handle, uint32_t maskColor, const ArkUI_Rect* maskRect);
    /**
    * @brief Sets the background color for a custom dialog box.
    *
    * @note This method must be called before the <b>show</b> method.
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @param backgroundColor Indicates the background color of the custom dialog box, in 0xARGB format.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*setBackgroundColor)(ArkUI_NativeDialogHandle handle, uint32_t backgroundColor);
    /**
    * @brief Sets the background corner radius for a custom dialog box.
    *
    * @note This method must be called before the <b>show</b> method.
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @param topLeft Indicates the radius of the upper left corner of the custom dialog box background.
    * @param topRight Indicates the radius of the upper right corner of the custom dialog box background.
    * @param bottomLeft Indicates the radius of the lower left corner of the custom dialog box background.
    * @param bottomRight Indicates the radius of the lower right corner of the custom dialog box background.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*setCornerRadius)(ArkUI_NativeDialogHandle handle, float topLeft, float topRight,
        float bottomLeft, float bottomRight);
    /**
    * @brief Sets the number of grid columns occupied by a custom dialog box.
    *
    * @note This method must be called before the <b>show</b> method.
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @param gridCount Indicates the number of grid columns occupied by the dialog box. The default value is subject to
    * the window size, and the maximum value is the maximum number of columns supported by the system.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*setGridColumnCount)(ArkUI_NativeDialogHandle handle, int32_t gridCount);
    /**
    * @brief Specifies whether to use a custom style for the custom dialog box.
    *
    * @note This method must be called before the <b>show</b> method.
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @param enableCustomStyle Specifies whether to use a custom style for the dialog box.
    * <b>true</b>: The dialog box automatically adapts its width to the child components; the rounded corner is 0;
    * the background color is transparent.
    * <b>false</b>: The dialog box automatically adapts its width to the grid system and its height to the child
    * components; the rounded corner is 24 vp.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*enableCustomStyle)(ArkUI_NativeDialogHandle handle, bool enableCustomStyle);
    /**
    * @brief Specifies whether to use a custom animation for a custom dialog box.
    *
    * @note This method must be called before the <b>show</b> method.
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @param enableCustomAnimation Specifies whether to use a custom animation. The value <b>true</b> means to use a
    * custom animation, and <b>false</b> means to use the default animation.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*enableCustomAnimation)(ArkUI_NativeDialogHandle handle, bool enableCustomAnimation);
    /**
    * @brief Registers a callback for a custom dialog box so that the user can decide whether to close the dialog box
    * after they touch the Back button or press the Esc key.
    *
    * @note This method must be called before the <b>show</b> method.
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @param eventHandler Indicates the callback to register. The parameter type is {@link ArkUI_OnWillDismissEvent}.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*registerOnWillDismiss)(ArkUI_NativeDialogHandle handle, ArkUI_OnWillDismissEvent eventHandler);
    /**
    * @brief Shows a custom dialog box.
    *
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @param showInSubWindow Specifies whether to show the dialog box in a sub-window.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*show)(ArkUI_NativeDialogHandle handle, bool showInSubWindow);
    /**
    * @brief Closes a custom dialog box. If the dialog box has been closed, this API does not take effect.
    *
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @return Returns <b>0</b> if the operation is successful; returns <b>401</b> if a parameter error occurs.
    */
    int32_t (*close)(ArkUI_NativeDialogHandle handle);

    /**
    * @brief Registers a listener for the dismiss event of the custom dialog box.
    *
    * @param handle Indicates the pointer to the custom dialog box controller.
    * @param userData Indicates the pointer to the custom data.
    * @param callback Indicates the callback for the dismiss event of the custom dialog box.
    * @return Returns the result code.
    *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
    *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
    */
    int32_t (*registerOnWillDismissWithUserData)(
        ArkUI_NativeDialogHandle handle, void* userData, void (*callback)(ArkUI_DialogDismissEvent* event));
} ArkUI_NativeDialogAPI_1;

/**
 * @brief Provides the custom dialog box APIs for the native side.
 *
 * @version 2
 * @since 15
 */
typedef struct {
    /**
     * @brief Provides the custom dialog box APIs for the native side. The API scope is {@link ArkUI_NativeDialogAPI_1}
     *
     * @since 15
     */
    ArkUI_NativeDialogAPI_1 nativeDialogAPI1;
    /**
     * @brief Defines the distance between the customDialog and system keyboard.
     *
	 * @note This method must be called before the <b>show</b> method.
     * @param handle Indicates the pointer to the custom dialog box controller.
     * @param distance distance, in vp.
     * @param unit  Indicates the unit, which is an enumerated value of {@link ArkUI_LengthMetricUnit}
     * @return Returns the result code.
     *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
     *         Returns {@link ARKUI_ERROR_CODE_CAPI_INIT_ERROR} if the CAPI init error.
     *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
     * @since 15
     */
    int32_t (*setKeyboardAvoidDistance)(ArkUI_NativeDialogHandle handle, float distance, ArkUI_LengthMetricUnit unit);

    /**
     * @brief Sets the level mode for a custom dialog box.
     *
     * @note This method must be called before the <b>show</b> method.
     * @param handle Indicates the pointer to the custom dialog box controller.
     * @param levelMode Indicates the level mode. The parameter type is {@link ArkUI_LevelMode}.
     * @return Returns the error code.
     *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
     *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
     * @since 15
     */
    int32_t (*setLevelMode)(ArkUI_NativeDialogHandle handle, ArkUI_LevelMode levelMode);
 
    /**
     * @brief Sets the level uniqueId for a custom dialog box.
     *
     * @note This method must be called before the <b>setLevelMode</b> method.
     * @param handle Indicates the pointer to the custom dialog box controller.
     * @param uniqueId Indicates the unique id of any nodes in router or navigation pages.
     * @return Returns the error code.
     *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
     *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
     * @since 15
     */
    int32_t (*setLevelUniqueId)(ArkUI_NativeDialogHandle handle, int32_t uniqueId);
 
    /**
     * @brief Sets the immersive mode for a custom dialog box.
     *
     * @note This method must be called before the <b>show</b> method.
     * @param handle Indicates the pointer to the custom dialog box controller.
     * @param immersiveMode Indicates the immersive mode. The parameter type is {@link ArkUI_ImmersiveMode}.
     * @return Returns the error code.
     *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
     *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
     * @since 15
     */
    int32_t (*setImmersiveMode)(ArkUI_NativeDialogHandle handle, ArkUI_ImmersiveMode immersiveMode);
} ArkUI_NativeDialogAPI_2;

/**
 * @brief Provides the custom dialog box APIs for the native side.
 *
 * @version 3
 * @since 16
 */
typedef struct {
    /**
     * @brief Provides the custom dialog box APIs for the native side. The API scope is {@link ArkUI_NativeDialogAPI_1}
     *
     * @since 16
     */
    ArkUI_NativeDialogAPI_1 nativeDialogAPI1;
    /**
     * @brief Provides the custom dialog box APIs for the native side. The API scope is {@link ArkUI_NativeDialogAPI_2}
     *
     * @since 16
     */
    ArkUI_NativeDialogAPI_2 nativeDialogAPI2;
    /**
     * @brief Sets the display order for a custom dialog box.
     *
     * @note This method must be called before the <b>show</b> method.
     * @param handle Indicates the pointer to the custom dialog box controller.
     * @param levelOrder Indicates the display order. The valid range is [-100000.0, 100000.0].
     * @return Returns the error code.
     *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
     *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
     * @since 16
     */
    int32_t (*setLevelOrder)(ArkUI_NativeDialogHandle handle, double levelOrder);

    /**
     * @brief Registers a listener callback before the dialog openAnimation starts.
     *
     * @param handle Indicates the pointer to the custom dialog box controller.
     * @param userData Indicates the pointer to the custom data.
     * @param callback Indicates the callback before the dialog openAnimation starts.
     * @return Returns the result code.
     *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
     *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
     * @since 16
     */
    int32_t (*registerOnWillAppear)(
        ArkUI_NativeDialogHandle handle, void* userData, void (*callback)(void* userData));

    /**
     * @brief Registers a listener callback when the dialog appears.
     *
     * @param handle Indicates the pointer to the custom dialog box controller.
     * @param userData Indicates the pointer to the custom data.
     * @param callback Indicates the callback when the dialog appears.
     * @return Returns the result code.
     *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
     *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
     * @since 16
     */
    int32_t (*registerOnDidAppear)(
        ArkUI_NativeDialogHandle handle, void* userData, void (*callback)(void* userData));

    /**
     * @brief Registers a listener callback before the dialog closeAnimation starts.
     *
     * @param handle Indicates the pointer to the custom dialog box controller.
     * @param userData Indicates the pointer to the custom data.
     * @param callback Indicates the callback before the dialog closeAnimation starts.
     * @return Returns the result code.
     *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
     *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
     * @since 16
     */
    int32_t (*registerOnWillDisappear)(
        ArkUI_NativeDialogHandle handle, void* userData, void (*callback)(void* userData));

    /**
     * @brief Registers a listener callback when the dialog disappears.
     *
     * @param handle Indicates the pointer to the custom dialog box controller.
     * @param userData Indicates the pointer to the custom data.
     * @param callback Indicates the callback when the dialog disappears.
     * @return Returns the result code.
     *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
     *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
     * @since 16
     */
    int32_t (*registerOnDidDisappear)(
        ArkUI_NativeDialogHandle handle, void* userData, void (*callback)(void* userData));

    /**
     * @brief Sets whether to get focus when the custom dialog is displayed.
     *
     * @param handle Indicates the pointer to the custom dialog box controller.
     * @param focusable Specifies whether to get focus when the custom dialog is displayed. The default value is true.
     * @return Returns the error code.
     *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
     *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
     * @since 16
     */
    int32_t (*setFocusable)(ArkUI_NativeDialogHandle handle, bool focusable);
} ArkUI_NativeDialogAPI_3;

/**
 * @brief Sets whether to block the system behavior of dismissing a dialog box.
 *
 * @param event Indicates the pointer to a dialog box dismiss event object.
 * @param shouldBlockDismiss Indicates whether to block the system behavior of dismissing the dialog box. The value
 *                           <b>true</b> means to block the system behavior, and <b>false</b> means the opposite.
 * @since 12
 */
void OH_ArkUI_DialogDismissEvent_SetShouldBlockDismiss(ArkUI_DialogDismissEvent* event, bool shouldBlockDismiss);

/**
 * @brief Obtains the pointer to user data in a dialog box dismiss event object.
 *
 * @param event Indicates the pointer to a dialog box dismiss event object.
 *
 * @return Returns the pointer to user data.
 * @since 12
 */
void* OH_ArkUI_DialogDismissEvent_GetUserData(ArkUI_DialogDismissEvent* event);

/**
 * @brief Obtains the c from a dialog box dismiss event object.
 *
 * @param event Indicates the pointer to a dialog box dismiss event object.
 *
 * @return Returns the dismissal reason. Returns <b>-1</b> if an exception occurs.
 *         {@link DIALOG_DISMISS_BACK_PRESS}: touching the Back button, swiping left or right on the screen, or
 *                                            pressing the Esc key.
 *         {@link DIALOG_DISMISS_TOUCH_OUTSIDE}: touching the mask.
 *         {@link DIALOG_DISMISS_CLOSE_BUTTON}: touching the Close button.
 *         {@link DIALOG_DISMISS_SLIDE_DOWN}: sliding down.
 * @since 12
 */
int32_t OH_ArkUI_DialogDismissEvent_GetDismissReason(ArkUI_DialogDismissEvent* event);

/**
 * @brief Displays a custom dialog box.
 *
 * @param options Dialog box parameters.
 * @param callback Indicates the callback for creating dialog. The dialog id is returned.
 * @return Returns the error code.
 *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
 *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
 * @since 16
 */
int32_t OH_ArkUI_CustomDialog_OpenDialog(ArkUI_CustomDialogOptions* options, void (*callback)(int32_t dialogId));

/**
 * @brief Closes a custom dialog box.
 *
 * @param dialogId Dialog id.
 * @return Returns the error code.
 *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
 *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
 * @since 16
 */
int32_t OH_ArkUI_CustomDialog_CloseDialog(int32_t dialogId);

/**
 * @brief Creates custom dialog box options.
 *
 * @param content Content of the custom dialog box.
 * @return Returns the pointer to the custom dialog box options.
 * @since 16
 */
ArkUI_CustomDialogOptions* OH_ArkUI_CustomDialog_CreateOptions(ArkUI_NodeHandle content);

/**
 * @brief Destroys the custom dialog box options.
 *
 * @param options The pointer to the custom dialog box options.
 * @since 16
 */
void OH_ArkUI_CustomDialog_DisposeOptions(ArkUI_CustomDialogOptions* options);

/**
 * @brief Sets the level mode for a custom dialog box.
 *
 * @note This method must be called before the <b>OH_ArkUI_CustomDialog_OpenDialog</b> method.
 * @param options Indicates the pointer to the custom dialog options.
 * @param levelMode Indicates the level mode. The parameter type is {@link ArkUI_LevelMode}.
 * @return Returns the error code.
 *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
 *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
 * @since 16
 */
int32_t OH_ArkUI_CustomDialog_SetLevelMode(ArkUI_CustomDialogOptions* options, ArkUI_LevelMode levelMode);

/**
 * @brief Sets the level uniqueId for a custom dialog box.
 *
 * @note This method must be called before the <b>OH_ArkUI_CustomDialog_OpenDialog</b> method.
 * @param options Indicates the pointer to the custom dialog options.
 * @param uniqueId Indicates the unique id of any nodes in router or navigation pages.
 * @return Returns the error code.
 *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
 *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
 * @since 16
 */
int32_t OH_ArkUI_CustomDialog_SetLevelUniqueId(ArkUI_CustomDialogOptions* options, int32_t uniqueId);

/**
 * @brief Sets the immersive mode for a custom dialog box.
 *
 * @note This method must be called before the <b>OH_ArkUI_CustomDialog_OpenDialog</b> method.
 * @param options Indicates the pointer to the custom dialog options.
 * @param immersiveMode Indicates the immersive mode. The parameter type is {@link ArkUI_ImmersiveMode}.
 * @return Returns the error code.
 *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
 *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
 * @since 16
 */
int32_t OH_ArkUI_CustomDialog_SetImmersiveMode(ArkUI_CustomDialogOptions* options, ArkUI_ImmersiveMode immersiveMode);

#ifdef __cplusplus
};
#endif

#endif // ARKUI_NATIVE_DIALOG_H
