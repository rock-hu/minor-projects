/**
 * Common constants of all function
 */
export class CommonConstants {
  /**
   *  Screen status change listener ID
   */
  static readonly COMMON_EVENT_SCREEN_CHANGE: number = 2;
  /**
   * Expanded foldable screen aspect ratio
   */
  static readonly FOLD_DEVICE_EXPAND_RATIO: number = 1;

  /**
   * Non-foldable screen aspect ratio
   */
  static readonly NOT_FOLD_DEVICE_EXPAND_RATIO: number = 9 / 16;

  /**
   * Height of the navigation bar of a foldable device
   */
  static readonly FOLD_DEVICE_NAV_HEIGHT: number = 38.72;

  /**
   * Height of the navigation bar of a non-foldable phone
   */
  static readonly PHONE_NAV_HEIGHT: number = 38.86;

  /**
   * Height of the navigation bar of a tablet
   */
  static readonly TABLET_NAV_HEIGHT: number = 39;

  /**
   * Device type
   */
  static readonly DEVICE_TYPES: string[] = ['phone', 'tablet'];

  /**
   *  Screen size change listener ID
   */
  static readonly WINDOW_SIZE_CHANGE: number = 1;

  /**
   *  Height of the close button area
   */
  static readonly TOP_BUTTON_HEIGHT: number = 56;

  /**
   *  Top title margin
   */
  static readonly TITLE_MARGIN_TOP: number = 8;
  /**
   *  Title row height
   */
  static readonly TITLE_MAIN_LINE_HEIGHT: number = 24;
  /**
   *  Scanning line margin
   */
  static readonly SCAN_LINE_MARGIN: number = 24;
  /**
   *  Scanning bottom area height
   */
  static readonly SCAN_BOTTOM_HEIGHT: number = 148;
  /**
   *  Flash light height
   */
  static readonly FLASH_HEIGHT: number = 52;
  /**
   *  Flash light bottom margin
   */
  static readonly FLASH_MARGIN_BOTTOM: number = 12;
  /**
   *  Scanning area height
   */
  static readonly PLACEHOLDER_HEIGHT: number = 56;
  /**
   *  Image press area height
   */
  static readonly ICON_PRESS_HEIGHT: number = 72;
  /**
   *  TabBar height
   */
  static readonly TAB_BAR_HEIGHT: number = 32;
  /**
   *  Ai Bottom Bar height
   */
  static readonly AI_BOTTOM_BAR_HEIGHT: number = 40;
  /**
   *  Close icon size
   */
  static readonly CLOSE_IMAGE_SIZE: number = 40;
  /**
   *  SM device padding
   */
  static readonly PADDING_SM: number = 16;
  /**
   *  MD device padding
   */
  static readonly PADDING_MD: number = 24;
  /**
   *  LG device padding
   */
  static readonly PADDING_LG: number = 32;
  /**
   *  SCAN_FONT_SIZE_SCALE_LIMIT
   */
  static readonly SCAN_FONT_SIZE_SCALE_LIMIT_SYMBOL_TEXT: number = 1.45;
  static readonly SCAN_FONT_SIZE_SCALE_LIMIT_TITLE_TEXT: number = 1.75;
  static readonly SCAN_FONT_SIZE_SCALE_LIMIT_2: number = 2;
  static readonly SCAN_FONT_SIZE_SCALE_LIMIT_DIALOG_TEXT: number = 3.2;

  static readonly GLOBAL_SCAN_DEVICE_IS_FOLDING: string = 'hmsCore_scan_isFoldable';
  static readonly GLOBAL_SCAN_DEVICE_TYPE: string = 'hmsCore_scan_device_type';
  static readonly GLOBAL_SCAN_TOP_RECT_HEIGHT: string = 'scan_topRect_height';
  static readonly GLOBAL_SCAN_IS_DECODING_GALLERY: string = 'scan_is_decoding_gallery';
  static readonly GLOBAL_SCAN_SELECT_A_PICTURE: string = 'scan_select_a_picture';
  static readonly EVENT_SCREEN_OFF: boolean = false;
  static readonly EVENT_SCREEN_ON: boolean = true;
}

