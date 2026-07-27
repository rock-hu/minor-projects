/**
 * Cordova接管H5的若干种弹窗类型
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/26 15:01
 */
export class CordovaDialogsHelper {
  static showAlert(message: string, gotResult: (success: boolean, message: string | null) => void) {
    AlertDialog.show({
      title: 'Alert',
      message: message,
      autoCancel: true,
      confirm: {
        value: '确认',
        action: () => {
          gotResult(true, null)
        }
      },
      cancel: () => {
        gotResult(false, null)
      }
    })
  }

  static showConfirm(message: string, gotResult: (success: boolean, message: string | null) => void) {
    AlertDialog.show({
      title: 'Confirm',
      message: message,
      autoCancel: true,
      primaryButton: {
        value: '确认',
        action: () => {
          gotResult(true, null)
        }
      },
      secondaryButton: {
        value: '取消',
        action: () => {
          gotResult(false, null)
        }
      },
      cancel: () => {
        gotResult(false, null)
      }
    })
  }

  static showPrompt(message: string, gotResult: (success: boolean, message: string | null) => void) {
    AlertDialog.show({
      message: message,
      autoCancel: false,
      primaryButton: {
        value: '确认',
        action: () => {
          gotResult(true, null)
        }
      },
      secondaryButton: {
        value: '取消',
        action: () => {
          gotResult(false, null)
        }
      }
    })
  }
}