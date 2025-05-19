import type { AlertOptions } from "./AlertManager";

export type Buttons = Array<{
  text?: string,
  onPress?: ?Function,
}>;

type Options = {
  cancelable?: ?boolean,
  onDismiss?: ?() => void,
  ...
};

/**
 * Launches an alert dialog with the specified title and message.
 *
 * See https://reactnative.dev/docs/alert
 */
class Alert {
  static alert(
    title: ?string,
    message?: ?string,
    buttons?: Buttons,
    options?: Options
  ): void {
    const AlertManager = require("./AlertManager").default;
    if (!AlertManager) {
      return;
    }

    const alertOptions: AlertOptions = {
      title: title || "",
      message: message || "",
      cancelable: false,
    };

    const constants = AlertManager.getConstants();

    if (options && options.cancelable) {
      alertOptions.cancelable = options.cancelable;
    }

    // At most three buttons. Ignore rest.
    const validButtons: Buttons = (buttons ?? [{text: "OK"}]).slice(0, 3)
    const primaryButton = validButtons.pop();
    const secondaryButton = validButtons.pop();
    const thirdaryButton = validButtons.pop();

    if (primaryButton) {
      alertOptions.primaryButton = primaryButton.text || "";
    }
    if (secondaryButton) {
      alertOptions.secondaryButton = secondaryButton.text || "";
    }
    if (thirdaryButton) {
      alertOptions.thirdaryButton = thirdaryButton.text || "";
    }

    const onAction = (action, buttonKey) => {
      if (action === constants.buttonClicked) {
        if (buttonKey === constants.primaryButton) {
          primaryButton.onPress && primaryButton.onPress();
        } else if (buttonKey === constants.secondaryButton) {
          secondaryButton.onPress && secondaryButton.onPress();
        } else if (buttonKey === constants.thirdaryButton) {
          thirdaryButton.onPress && thirdaryButton.onPress();
        }
      } else if (action === constants.dismissed) {
        options && options.onDismiss && options.onDismiss();
      }
    };
    const onError = (errorMessage: string) => console.warn(errorMessage);
    AlertManager.alert(alertOptions, onError, onAction);
  }
}

module.exports = Alert;
