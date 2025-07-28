import { int32, float32 } from "@koalaui/common"
export enum PointerStyle {
    DEFAULT = 0,
    EAST = 1,
    WEST = 2,
    SOUTH = 3,
    NORTH = 4,
    WEST_EAST = 5,
    NORTH_SOUTH = 6,
    NORTH_EAST = 7,
    NORTH_WEST = 8,
    SOUTH_EAST = 9,
    SOUTH_WEST = 10,
    NORTH_EAST_SOUTH_WEST = 11,
    NORTH_WEST_SOUTH_EAST = 12,
    CROSS = 13,
    CURSOR_COPY = 14,
    CURSOR_FORBID = 15,
    COLOR_SUCKER = 16,
    HAND_GRABBING = 17,
    HAND_OPEN = 18,
    HAND_POINTING = 19,
    HELP = 20,
    MOVE = 21,
    RESIZE_LEFT_RIGHT = 22,
    RESIZE_UP_DOWN = 23,
    SCREENSHOT_CHOOSE = 24,
    SCREENSHOT_CURSOR = 25,
    TEXT_CURSOR = 26,
    ZOOM_IN = 27,
    ZOOM_OUT = 28,
    MIDDLE_BTN_EAST = 29,
    MIDDLE_BTN_WEST = 30,
    MIDDLE_BTN_SOUTH = 31,
    MIDDLE_BTN_NORTH = 32,
    MIDDLE_BTN_NORTH_SOUTH = 33,
    MIDDLE_BTN_NORTH_EAST = 34,
    MIDDLE_BTN_NORTH_WEST = 35,
    MIDDLE_BTN_SOUTH_EAST = 36,
    MIDDLE_BTN_SOUTH_WEST = 37,
    MIDDLE_BTN_NORTH_SOUTH_WEST_EAST = 38,
    HORIZONTAL_TEXT_CURSOR = 39,
    CURSOR_CROSS = 40,
    CURSOR_CIRCLE = 41,
    LOADING = 42,
    RUNNING = 43
}
export enum RectWidthStyle {
    TIGHT = 0,
    MAX = 1
}
export enum Affinity {
    UPSTREAM = 0,
    DOWNSTREAM = 1
}
export enum RectHeightStyle {
    TIGHT = 0,
    MAX = 1,
    INCLUDE_LINE_SPACE_MIDDLE = 2,
    INCLUDE_LINE_SPACE_TOP = 3,
    INCLUDE_LINE_SPACE_BOTTOM = 4,
    STRUT = 5
}
export enum LengthUnit {
    PX = 0,
    VP = 1,
    FP = 2,
    PERCENT = 3,
    LPX = 4
}
export enum PerfMonitorActionType {
    LAST_DOWN = 0,
    LAST_UP = 1,
    FIRST_MOVE = 2
}
export enum PerfMonitorSourceType {
    PERF_TOUCH_EVENT = 0,
    PERF_MOUSE_EVENT = 1,
    PERF_TOUCHPAD_EVENT = 2,
    PERF_JOYSTICK_EVENT = 3,
    PERF_KEY_EVENT = 4
}
export enum EffectDirection {
    DOWN = 0,
    UP = 1
}
export enum EffectScope {
    LAYER = 0,
    WHOLE = 1
}
export enum DialogAlignment {
    TOP = 0,
    Top = 0,
    CENTER = 1,
    Center = 1,
    BOTTOM = 2,
    Bottom = 2,
    DEFAULT = 3,
    Default = 3,
    TOP_START = 4,
    TopStart = 4,
    TOP_END = 5,
    TopEnd = 5,
    CENTER_START = 6,
    CenterStart = 6,
    CENTER_END = 7,
    CenterEnd = 7,
    BOTTOM_START = 8,
    BottomStart = 8,
    BOTTOM_END = 9,
    BottomEnd = 9
}
export enum DialogButtonDirection {
    AUTO = 0,
    HORIZONTAL = 1,
    VERTICAL = 2
}
export enum IndexerAlign {
    LEFT = 0,
    Left = 0,
    RIGHT = 1,
    Right = 1,
    START = 2,
    END = 3
}
export enum BadgePosition {
    RIGHT_TOP = 0,
    RightTop = 0,
    RIGHT = 1,
    Right = 1,
    LEFT = 2,
    Left = 2
}
export enum ButtonType {
    CAPSULE = 0,
    Capsule = 0,
    CIRCLE = 1,
    Circle = 1,
    NORMAL = 2,
    Normal = 2,
    ROUNDED_RECTANGLE = 3
}
export enum ButtonStyleMode {
    NORMAL = 0,
    EMPHASIZED = 1,
    TEXTUAL = 2
}
export enum ButtonRole {
    NORMAL = 0,
    ERROR = 1
}
export enum ControlSize {
    SMALL = "small",
    NORMAL = "normal"
}
export enum CalendarAlign {
    START = 0,
    CENTER = 1,
    END = 2
}
export enum SelectStatus {
    ALL = 0,
    All = 0,
    PART = 1,
    Part = 1,
    NONE = 2,
    None = 2
}
export enum FinishCallbackType {
    REMOVED = 0,
    LOGICALLY = 1
}
export enum TouchTestStrategy {
    DEFAULT = 0,
    FORWARD_COMPETITION = 1,
    FORWARD = 2
}
export enum TransitionHierarchyStrategy {
    NONE = 0,
    ADAPTIVE = 1
}
export enum ChainStyle {
    SPREAD = 0,
    SPREAD_INSIDE = 1,
    PACKED = 2
}
export enum TransitionEdge {
    TOP = 0,
    BOTTOM = 1,
    START = 2,
    END = 3
}
export enum EffectType {
    DEFAULT = 0,
    WINDOW_EFFECT = 1
}
export enum PreDragStatus {
    ACTION_DETECTING_STATUS = 0,
    READY_TO_TRIGGER_DRAG_ACTION = 1,
    PREVIEW_LIFT_STARTED = 2,
    PREVIEW_LIFT_FINISHED = 3,
    PREVIEW_LANDING_STARTED = 4,
    PREVIEW_LANDING_FINISHED = 5,
    ACTION_CANCELED_BEFORE_DRAG = 6
}
export enum SourceType {
    UNKNOWN = 0,
    Unknown = 0,
    MOUSE = 1,
    Mouse = 1,
    TOUCH_SCREEN = 2,
    TouchScreen = 2
}
export enum SourceTool {
    UNKNOWN = 0,
    Unknown = 0,
    FINGER = 1,
    Finger = 1,
    PEN = 2,
    Pen = 2,
    MOUSE = 3,
    TOUCHPAD = 4,
    JOYSTICK = 5
}
export enum RepeatMode {
    REPEAT = 0,
    Repeat = 0,
    STRETCH = 1,
    Stretch = 1,
    ROUND = 2,
    Round = 2,
    SPACE = 3,
    Space = 3
}
export enum BlurStyle {
    THIN = 0,
    Thin = 0,
    REGULAR = 1,
    Regular = 1,
    THICK = 2,
    Thick = 2,
    BACKGROUND_THIN = 3,
    BACKGROUND_REGULAR = 4,
    BACKGROUND_THICK = 5,
    BACKGROUND_ULTRA_THICK = 6,
    NONE = 7,
    COMPONENT_ULTRA_THIN = 8,
    COMPONENT_THIN = 9,
    COMPONENT_REGULAR = 10,
    COMPONENT_THICK = 11,
    COMPONENT_ULTRA_THICK = 12
}
export enum BlurStyleActivePolicy {
    FOLLOWS_WINDOW_ACTIVE_STATE = 0,
    ALWAYS_ACTIVE = 1,
    ALWAYS_INACTIVE = 2
}
export enum ThemeColorMode {
    SYSTEM = 0,
    LIGHT = 1,
    DARK = 2
}
export enum AdaptiveColor {
    DEFAULT = 0,
    AVERAGE = 1
}
export enum ModalTransition {
    DEFAULT = 0,
    NONE = 1,
    ALPHA = 2
}
export enum ShadowType {
    COLOR = 0,
    BLUR = 1
}
export enum ShadowStyle {
    OUTER_DEFAULT_XS = 0,
    OUTER_DEFAULT_SM = 1,
    OUTER_DEFAULT_MD = 2,
    OUTER_DEFAULT_LG = 3,
    OUTER_FLOATING_SM = 4,
    OUTER_FLOATING_MD = 5
}
export enum SafeAreaType {
    SYSTEM = 0,
    CUTOUT = 1,
    KEYBOARD = 2
}
export enum SafeAreaEdge {
    TOP = 0,
    BOTTOM = 1,
    START = 2,
    END = 3
}
export enum LayoutSafeAreaType {
    SYSTEM = 0
}
export enum LayoutSafeAreaEdge {
    TOP = 0,
    BOTTOM = 1
}
export enum SheetSize {
    MEDIUM = 0,
    LARGE = 1,
    FIT_CONTENT = 2
}
export enum DragBehavior {
    COPY = 0,
    MOVE = 1
}
export enum DragResult {
    DRAG_SUCCESSFUL = 0,
    DRAG_FAILED = 1,
    DRAG_CANCELED = 2,
    DROP_ENABLED = 3,
    DROP_DISABLED = 4
}
export enum BlendMode {
    NONE = 0,
    CLEAR = 1,
    SRC = 2,
    DST = 3,
    SRC_OVER = 4,
    DST_OVER = 5,
    SRC_IN = 6,
    DST_IN = 7,
    SRC_OUT = 8,
    DST_OUT = 9,
    SRC_ATOP = 10,
    DST_ATOP = 11,
    XOR = 12,
    PLUS = 13,
    MODULATE = 14,
    SCREEN = 15,
    OVERLAY = 16,
    DARKEN = 17,
    LIGHTEN = 18,
    COLOR_DODGE = 19,
    COLOR_BURN = 20,
    HARD_LIGHT = 21,
    SOFT_LIGHT = 22,
    DIFFERENCE = 23,
    EXCLUSION = 24,
    MULTIPLY = 25,
    HUE = 26,
    SATURATION = 27,
    COLOR = 28,
    LUMINOSITY = 29
}
export enum BlendApplyType {
    FAST = 0,
    OFFSCREEN = 1
}
export enum SheetType {
    BOTTOM = 0,
    CENTER = 1,
    POPUP = 2
}
export enum SheetMode {
    OVERLAY = 0,
    EMBEDDED = 1
}
export enum ScrollSizeMode {
    FOLLOW_DETENT = 0,
    CONTINUOUS = 1
}
export enum SheetKeyboardAvoidMode {
    NONE = 0,
    TRANSLATE_AND_RESIZE = 1,
    RESIZE_ONLY = 2,
    TRANSLATE_AND_SCROLL = 3
}
export enum DismissReason {
    PRESS_BACK = 0,
    TOUCH_OUTSIDE = 1,
    CLOSE_BUTTON = 2,
    SLIDE_DOWN = 3
}
export enum MenuPreviewMode {
    NONE = 0,
    IMAGE = 1
}
export enum OutlineStyle {
    SOLID = 0,
    DASHED = 1,
    DOTTED = 2
}
export enum DragPreviewMode {
    AUTO = 1,
    DISABLE_SCALE = 2,
    ENABLE_DEFAULT_SHADOW = 3,
    ENABLE_DEFAULT_RADIUS = 4
}
export enum MenuPolicy {
    DEFAULT = 0,
    HIDE = 1,
    SHOW = 2
}
export enum ContentClipMode {
    CONTENT_ONLY = 0,
    BOUNDARY = 1,
    SAFE_AREA = 2
}
export enum KeyboardAvoidMode {
    DEFAULT = 0,
    NONE = 1
}
export enum HoverModeAreaType {
    TOP_SCREEN = 0,
    BOTTOM_SCREEN = 1
}
export enum ModelType {
    TEXTURE = 0,
    SURFACE = 1
}
export enum DataPanelType {
    LINE = 0,
    Line = 0,
    CIRCLE = 1,
    Circle = 1
}
export enum CheckBoxShape {
    CIRCLE = 0,
    ROUNDED_SQUARE = 1
}
export enum Color {
    WHITE = 0,
    White = 0,
    BLACK = 1,
    Black = 1,
    BLUE = 2,
    Blue = 2,
    BROWN = 3,
    Brown = 3,
    GRAY = 4,
    Gray = 4,
    GREEN = 5,
    Green = 5,
    GREY = 6,
    Grey = 6,
    ORANGE = 7,
    Orange = 7,
    PINK = 8,
    Pink = 8,
    RED = 9,
    Red = 9,
    YELLOW = 10,
    Yellow = 10,
    TRANSPARENT = 11,
    Transparent = 11
}
export enum ColoringStrategy {
    INVERT = "invert",
    AVERAGE = "average",
    PRIMARY = "primary"
}
export enum ImageFit {
    CONTAIN = 0,
    Contain = 0,
    COVER = 1,
    Cover = 1,
    AUTO = 2,
    Auto = 2,
    FILL = 3,
    Fill = 3,
    SCALE_DOWN = 4,
    ScaleDown = 4,
    NONE = 5,
    None = 5,
    TOP_START = 7,
    TOP = 8,
    TOP_END = 9,
    START = 10,
    CENTER = 11,
    END = 12,
    BOTTOM_START = 13,
    BOTTOM = 14,
    BOTTOM_END = 15
}
export enum BorderStyle {
    DOTTED = 0,
    Dotted = 0,
    DASHED = 1,
    Dashed = 1,
    SOLID = 2,
    Solid = 2
}
export enum LineJoinStyle {
    MITER = 0,
    Miter = 0,
    ROUND = 1,
    Round = 1,
    BEVEL = 2,
    Bevel = 2
}
export enum TouchType {
    DOWN = 0,
    Down = 0,
    UP = 1,
    Up = 1,
    MOVE = 2,
    Move = 2,
    CANCEL = 3,
    Cancel = 3
}
export enum MouseButton {
    LEFT = 0,
    Left = 0,
    RIGHT = 1,
    Right = 1,
    MIDDLE = 2,
    Middle = 2,
    BACK = 3,
    Back = 3,
    FORWARD = 4,
    Forward = 4,
    NONE = 5,
    None = 5
}
export enum MouseAction {
    PRESS = 0,
    Press = 0,
    RELEASE = 1,
    Release = 1,
    MOVE = 2,
    Move = 2,
    HOVER = 3,
    Hover = 3
}
export enum AnimationStatus {
    INITIAL = 0,
    Initial = 0,
    RUNNING = 1,
    Running = 1,
    PAUSED = 2,
    Paused = 2,
    STOPPED = 3,
    Stopped = 3
}
export enum Curve {
    LINEAR = 0,
    Linear = 0,
    EASE = 1,
    Ease = 1,
    EASE_IN = 2,
    EaseIn = 2,
    EASE_OUT = 3,
    EaseOut = 3,
    EASE_IN_OUT = 4,
    EaseInOut = 4,
    FAST_OUT_SLOW_IN = 5,
    FastOutSlowIn = 5,
    LINEAR_OUT_SLOW_IN = 6,
    LinearOutSlowIn = 6,
    FAST_OUT_LINEAR_IN = 7,
    FastOutLinearIn = 7,
    EXTREME_DECELERATION = 8,
    ExtremeDeceleration = 8,
    SHARP = 9,
    Sharp = 9,
    RHYTHM = 10,
    Rhythm = 10,
    SMOOTH = 11,
    Smooth = 11,
    FRICTION = 12,
    Friction = 12
}
export enum FillMode {
    NONE = 0,
    None = 0,
    FORWARDS = 1,
    Forwards = 1,
    BACKWARDS = 2,
    Backwards = 2,
    BOTH = 3,
    Both = 3
}
export enum PlayMode {
    NORMAL = 0,
    Normal = 0,
    REVERSE = 1,
    Reverse = 1,
    ALTERNATE = 2,
    Alternate = 2,
    ALTERNATE_REVERSE = 3,
    AlternateReverse = 3
}
export enum KeyType {
    DOWN = 0,
    Down = 0,
    UP = 1,
    Up = 1
}
export enum KeySource {
    UNKNOWN = 0,
    Unknown = 0,
    KEYBOARD = 1,
    Keyboard = 1
}
export enum Edge {
    TOP = 0,
    Top = 0,
    CENTER = 1,
    Center = 1,
    BOTTOM = 2,
    Bottom = 2,
    BASELINE = 3,
    Baseline = 3,
    START = 4,
    Start = 4,
    MIDDLE = 5,
    Middle = 5,
    END = 6,
    End = 6
}
export enum Week {
    MON = 0,
    Mon = 0,
    TUE = 1,
    Tue = 1,
    WED = 2,
    Wed = 2,
    THUR = 3,
    Thur = 3,
    FRI = 4,
    Fri = 4,
    SAT = 5,
    Sat = 5,
    SUN = 6,
    Sun = 6
}
export enum Direction {
    LTR = 0,
    Ltr = 0,
    RTL = 1,
    Rtl = 1,
    AUTO = 2,
    Auto = 2
}
export enum BarState {
    OFF = 0,
    Off = 0,
    AUTO = 1,
    Auto = 1,
    ON = 2,
    On = 2
}
export enum EdgeEffect {
    SPRING = 0,
    Spring = 0,
    FADE = 1,
    Fade = 1,
    NONE = 2,
    None = 2
}
export enum Alignment {
    TOP_START = 0,
    TopStart = 0,
    TOP = 1,
    Top = 1,
    TOP_END = 2,
    TopEnd = 2,
    START = 3,
    Start = 3,
    CENTER = 4,
    Center = 4,
    END = 5,
    End = 5,
    BOTTOM_START = 6,
    BottomStart = 6,
    BOTTOM = 7,
    Bottom = 7,
    BOTTOM_END = 8,
    BottomEnd = 8
}
export enum TransitionType {
    ALL = 0,
    All = 0,
    INSERT = 1,
    Insert = 1,
    DELETE = 2,
    Delete = 2
}
export enum RelateType {
    FILL = 0,
    FIT = 1
}
export enum Visibility {
    VISIBLE = 0,
    Visible = 0,
    HIDDEN = 1,
    Hidden = 1,
    NONE = 2,
    None = 2
}
export enum LineCapStyle {
    BUTT = 0,
    Butt = 0,
    ROUND = 1,
    Round = 1,
    SQUARE = 2,
    Square = 2
}
export enum Axis {
    VERTICAL = 0,
    Vertical = 0,
    HORIZONTAL = 1,
    Horizontal = 1
}
export enum HorizontalAlign {
    START = 0,
    Start = 0,
    CENTER = 1,
    Center = 1,
    END = 2,
    End = 2
}
export enum FlexAlign {
    START = 0,
    Start = 0,
    CENTER = 1,
    Center = 1,
    END = 2,
    End = 2,
    SPACE_BETWEEN = 3,
    SpaceBetween = 3,
    SPACE_AROUND = 4,
    SpaceAround = 4,
    SPACE_EVENLY = 5,
    SpaceEvenly = 5
}
export enum ItemAlign {
    AUTO = 0,
    Auto = 0,
    START = 1,
    Start = 1,
    CENTER = 2,
    Center = 2,
    END = 3,
    End = 3,
    BASELINE = 4,
    Baseline = 4,
    STRETCH = 5,
    Stretch = 5
}
export enum FlexDirection {
    ROW = 0,
    Row = 0,
    COLUMN = 1,
    Column = 1,
    ROW_REVERSE = 2,
    RowReverse = 2,
    COLUMN_REVERSE = 3,
    ColumnReverse = 3
}
export enum PixelRoundCalcPolicy {
    NO_FORCE_ROUND = 0,
    FORCE_CEIL = 1,
    FORCE_FLOOR = 2
}
export enum FlexWrap {
    NO_WRAP = 0,
    NoWrap = 0,
    WRAP = 1,
    Wrap = 1,
    WRAP_REVERSE = 2,
    WrapReverse = 2
}
export enum VerticalAlign {
    TOP = 0,
    Top = 0,
    CENTER = 1,
    Center = 1,
    BOTTOM = 2,
    Bottom = 2
}
export enum ImageRepeat {
    NO_REPEAT = 0,
    NoRepeat = 0,
    X = 1,
    Y = 2,
    XY = 3
}
export enum ImageSize {
    AUTO = 0,
    Auto = 0,
    COVER = 1,
    Cover = 1,
    CONTAIN = 2,
    Contain = 2,
    FILL = 3
}
export enum GradientDirection {
    LEFT = 0,
    Left = 0,
    TOP = 1,
    Top = 1,
    RIGHT = 2,
    Right = 2,
    BOTTOM = 3,
    Bottom = 3,
    LEFT_TOP = 4,
    LeftTop = 4,
    LEFT_BOTTOM = 5,
    LeftBottom = 5,
    RIGHT_TOP = 6,
    RightTop = 6,
    RIGHT_BOTTOM = 7,
    RightBottom = 7,
    NONE = 8,
    None = 8
}
export enum SharedTransitionEffectType {
    STATIC = 0,
    Static = 0,
    EXCHANGE = 1,
    Exchange = 1
}
export enum FontStyle {
    NORMAL = 0,
    Normal = 0,
    ITALIC = 1,
    Italic = 1
}
export enum FontWeight {
    LIGHTER = 0,
    Lighter = 0,
    NORMAL = 1,
    Normal = 1,
    REGULAR = 2,
    Regular = 2,
    MEDIUM = 3,
    Medium = 3,
    BOLD = 4,
    Bold = 4,
    BOLDER = 5,
    Bolder = 5
}
export enum TextAlign {
    CENTER = 0,
    Center = 0,
    START = 1,
    Start = 1,
    END = 2,
    End = 2,
    JUSTIFY = 3
}
export enum TextOverflow {
    NONE = 0,
    None = 0,
    CLIP = 1,
    Clip = 1,
    ELLIPSIS = 2,
    Ellipsis = 2,
    MARQUEE = 3
}
export enum TextDecorationType {
    NONE = 0,
    None = 0,
    UNDERLINE = 1,
    Underline = 1,
    OVERLINE = 2,
    Overline = 2,
    LINE_THROUGH = 3,
    LineThrough = 3
}
export enum TextCase {
    NORMAL = 0,
    Normal = 0,
    LOWER_CASE = 1,
    LowerCase = 1,
    UPPER_CASE = 2,
    UpperCase = 2
}
export enum TextHeightAdaptivePolicy {
    MAX_LINES_FIRST = 0,
    MIN_FONT_SIZE_FIRST = 1,
    LAYOUT_CONSTRAINT_FIRST = 2
}
export enum ResponseType {
    RIGHT_CLICK = 0,
    RightClick = 0,
    LONG_PRESS = 1,
    LongPress = 1
}
export enum HoverEffect {
    AUTO = 0,
    Auto = 0,
    SCALE = 1,
    Scale = 1,
    HIGHLIGHT = 2,
    Highlight = 2,
    NONE = 3,
    None = 3
}
export enum Placement {
    LEFT = 0,
    Left = 0,
    RIGHT = 1,
    Right = 1,
    TOP = 2,
    Top = 2,
    BOTTOM = 3,
    Bottom = 3,
    TOP_LEFT = 4,
    TopLeft = 4,
    TOP_RIGHT = 5,
    TopRight = 5,
    BOTTOM_LEFT = 6,
    BottomLeft = 6,
    BOTTOM_RIGHT = 7,
    BottomRight = 7,
    LEFT_TOP = 8,
    LeftTop = 8,
    LEFT_BOTTOM = 9,
    LeftBottom = 9,
    RIGHT_TOP = 10,
    RightTop = 10,
    RIGHT_BOTTOM = 11,
    RightBottom = 11
}
export enum ArrowPointPosition {
    START = "Start",
    CENTER = "Center",
    END = "End"
}
export enum CopyOptions {
    NONE = 0,
    None = 0,
    IN_APP = 1,
    InApp = 1,
    LOCAL_DEVICE = 2,
    LocalDevice = 2,
    CROSS_DEVICE = 3
}
export enum HitTestMode {
    DEFAULT = 0,
    Default = 0,
    BLOCK = 1,
    Block = 1,
    TRANSPARENT = 2,
    Transparent = 2,
    NONE = 3,
    None = 3
}
export enum TitleHeight {
    MAIN_ONLY = 0,
    MainOnly = 0,
    MAIN_WITH_SUB = 1,
    MainWithSub = 1
}
export enum ModifierKey {
    CTRL = 0,
    SHIFT = 1,
    ALT = 2
}
export enum FunctionKey {
    ESC = 0,
    F1 = 1,
    F2 = 2,
    F3 = 3,
    F4 = 4,
    F5 = 5,
    F6 = 6,
    F7 = 7,
    F8 = 8,
    F9 = 9,
    F10 = 10,
    F11 = 11,
    F12 = 12,
    TAB = 13,
    DPAD_UP = 14,
    DPAD_DOWN = 15,
    DPAD_LEFT = 16,
    DPAD_RIGHT = 17
}
export enum ImageSpanAlignment {
    BASELINE = 0,
    BOTTOM = 1,
    CENTER = 2,
    TOP = 3
}
export enum ObscuredReasons {
    PLACEHOLDER = 0
}
export enum TextContentStyle {
    DEFAULT = 0,
    INLINE = 1
}
export enum ClickEffectLevel {
    LIGHT = 0,
    MIDDLE = 1,
    HEAVY = 2
}
export enum XComponentType {
    SURFACE = 0,
    COMPONENT = 1,
    TEXTURE = 2,
    NODE = 3
}
export enum NestedScrollMode {
    SELF_ONLY = 0,
    SELF_FIRST = 1,
    PARENT_FIRST = 2,
    PARALLEL = 3
}
export enum ScrollSource {
    DRAG = 0,
    FLING = 1,
    EDGE_EFFECT = 2,
    OTHER_USER_INPUT = 3,
    SCROLL_BAR = 4,
    SCROLL_BAR_FLING = 5,
    SCROLLER = 6,
    SCROLLER_ANIMATION = 7
}
export enum RenderFit {
    CENTER = 0,
    TOP = 1,
    BOTTOM = 2,
    LEFT = 3,
    RIGHT = 4,
    TOP_LEFT = 5,
    TOP_RIGHT = 6,
    BOTTOM_LEFT = 7,
    BOTTOM_RIGHT = 8,
    RESIZE_FILL = 9,
    RESIZE_CONTAIN = 10,
    RESIZE_CONTAIN_TOP_LEFT = 11,
    RESIZE_CONTAIN_BOTTOM_RIGHT = 12,
    RESIZE_COVER = 13,
    RESIZE_COVER_TOP_LEFT = 14,
    RESIZE_COVER_BOTTOM_RIGHT = 15
}
export enum DialogButtonStyle {
    DEFAULT = 0,
    HIGHLIGHT = 1
}
export enum WordBreak {
    NORMAL = 0,
    BREAK_ALL = 1,
    BREAK_WORD = 2
}
export enum LineBreakStrategy {
    GREEDY = 0,
    HIGH_QUALITY = 1,
    BALANCED = 2
}
export enum EllipsisMode {
    START = 0,
    CENTER = 1,
    END = 2
}
export enum OptionWidthMode {
    FIT_CONTENT = "fit_content",
    FIT_TRIGGER = "fit_trigger"
}
export enum IlluminatedType {
    NONE = 0,
    BORDER = 1,
    CONTENT = 2,
    BORDER_CONTENT = 3,
    BLOOM_BORDER = 4,
    BLOOM_BORDER_CONTENT = 5
}
export enum FoldStatus {
    FOLD_STATUS_UNKNOWN = 0,
    FOLD_STATUS_EXPANDED = 1,
    FOLD_STATUS_FOLDED = 2,
    FOLD_STATUS_HALF_FOLDED = 3
}
export enum AppRotation {
    ROTATION_0 = 0,
    ROTATION_90 = 1,
    ROTATION_180 = 2,
    ROTATION_270 = 3
}
export enum EmbeddedType {
    EMBEDDED_UI_EXTENSION = 0
}
export enum MarqueeUpdateStrategy {
    DEFAULT = 0,
    PRESERVE_POSITION = 1
}
export enum TextDecorationStyle {
    SOLID = 0,
    DOUBLE = 1,
    DOTTED = 2,
    DASHED = 3,
    WAVY = 4
}
export enum TextSelectableMode {
    SELECTABLE_UNFOCUSABLE = 0,
    SELECTABLE_FOCUSABLE = 1,
    UNSELECTABLE = 2
}
export enum AccessibilityHoverType {
    HOVER_ENTER = 0,
    HOVER_MOVE = 1,
    HOVER_EXIT = 2,
    HOVER_CANCEL = 3
}
export enum WidthBreakpoint {
    WIDTH_XS = 0,
    WIDTH_SM = 1,
    WIDTH_MD = 2,
    WIDTH_LG = 3,
    WIDTH_XL = 4
}
export enum HeightBreakpoint {
    HEIGHT_SM = 0,
    HEIGHT_MD = 1,
    HEIGHT_LG = 2
}
export enum FocusPriority {
    AUTO = 0,
    PRIOR = 2000,
    PREVIOUS = 3000
}
export enum FormDimension {
    DIMENSION_1_2 = 0,
    Dimension_1_2 = 0,
    DIMENSION_2_2 = 1,
    Dimension_2_2 = 1,
    DIMENSION_2_4 = 2,
    Dimension_2_4 = 2,
    DIMENSION_4_4 = 3,
    Dimension_4_4 = 3,
    DIMENSION_2_1 = 4,
    Dimension_2_1 = 4,
    DIMENSION_1_1 = 6,
    DIMENSION_6_4 = 7
}
export enum FormRenderingMode {
    FULL_COLOR = 0,
    SINGLE_COLOR = 1
}
export enum FormShape {
    RECT = 1,
    CIRCLE = 2
}
export enum PanDirection {
    NONE = 0,
    None = 0,
    HORIZONTAL = 1,
    Horizontal = 1,
    LEFT = 2,
    Left = 2,
    RIGHT = 3,
    Right = 3,
    VERTICAL = 4,
    Vertical = 4,
    UP = 5,
    Up = 5,
    DOWN = 6,
    Down = 6,
    ALL = 7,
    All = 7
}
export enum SwipeDirection {
    NONE = 0,
    None = 0,
    HORIZONTAL = 1,
    Horizontal = 1,
    VERTICAL = 2,
    Vertical = 2,
    ALL = 3,
    All = 3
}
export enum GestureMode {
    SEQUENCE = 0,
    Sequence = 0,
    PARALLEL = 1,
    Parallel = 1,
    EXCLUSIVE = 2,
    Exclusive = 2
}
export enum GestureMask {
    NORMAL = 0,
    Normal = 0,
    IGNORE_INTERNAL = 1,
    IgnoreInternal = 1
}
export enum GestureJudgeResult {
    CONTINUE = 0,
    REJECT = 1
}
export namespace GestureControl {
    export enum GestureType {
        TAP_GESTURE = 0,
        LONG_PRESS_GESTURE = 1,
        PAN_GESTURE = 2,
        PINCH_GESTURE = 3,
        SWIPE_GESTURE = 4,
        ROTATION_GESTURE = 5,
        DRAG = 6,
        CLICK = 7
    }
}
export enum GesturePriority {
    NORMAL = 0,
    PRIORITY = 1
}
export enum GestureRecognizerState {
    READY = 0,
    DETECTING = 1,
    PENDING = 2,
    BLOCKED = 3,
    SUCCESSFUL = 4,
    FAILED = 5
}
export enum GridDirection {
    ROW = 0,
    Row = 0,
    COLUMN = 1,
    Column = 1,
    ROW_REVERSE = 2,
    RowReverse = 2,
    COLUMN_REVERSE = 3,
    ColumnReverse = 3
}
export enum GridItemAlignment {
    DEFAULT = 0,
    STRETCH = 1
}
export enum GridItemStyle {
    NONE = 0,
    PLAIN = 1
}
export enum SizeType {
    AUTO = 0,
    Auto = 0,
    XS = 1,
    SM = 2,
    MD = 3,
    LG = 4
}
export enum BreakpointsReference {
    WINDOW_SIZE = 0,
    WindowSize = 0,
    COMPONENT_SIZE = 1,
    ComponentSize = 1
}
export enum GridRowDirection {
    ROW = 0,
    Row = 0,
    ROW_REVERSE = 1,
    RowReverse = 1
}
export enum ImageRenderMode {
    ORIGINAL = 0,
    Original = 0,
    TEMPLATE = 1,
    Template = 1
}
export enum ImageContent {
    EMPTY = 0
}
export enum DynamicRangeMode {
    HIGH = 0,
    CONSTRAINT = 1,
    STANDARD = 2
}
export enum ImageInterpolation {
    NONE = 0,
    None = 0,
    LOW = 1,
    Low = 1,
    MEDIUM = 2,
    Medium = 2,
    HIGH = 3,
    High = 3
}
export enum ImageAnalyzerType {
    SUBJECT = 0,
    TEXT = 1,
    OBJECT_LOOKUP = 2
}
export enum DataOperationType {
    ADD = "add",
    DELETE = "delete",
    EXCHANGE = "exchange",
    MOVE = "move",
    CHANGE = "change",
    RELOAD = "reload"
}
export enum ScrollState {
    IDLE = 0,
    Idle = 0,
    SCROLL = 1,
    Scroll = 1,
    FLING = 2,
    Fling = 2
}
export enum ListItemAlign {
    START = 0,
    Start = 0,
    CENTER = 1,
    Center = 1,
    END = 2,
    End = 2
}
export enum ListItemGroupArea {
    NONE = 0,
    IN_LIST_ITEM_AREA = 1,
    IN_HEADER_AREA = 2,
    IN_FOOTER_AREA = 3
}
export enum StickyStyle {
    NONE = 0,
    None = 0,
    HEADER = 1,
    Header = 1,
    FOOTER = 2,
    Footer = 2
}
export enum ChainEdgeEffect {
    DEFAULT = 0,
    STRETCH = 1
}
export enum ScrollSnapAlign {
    NONE = 0,
    START = 1,
    CENTER = 2,
    END = 3
}
export enum Sticky {
    NONE = 0,
    None = 0,
    NORMAL = 1,
    Normal = 1,
    OPACITY = 2,
    Opacity = 2
}
export enum EditMode {
    NONE = 0,
    None = 0,
    DELETABLE = 1,
    Deletable = 1,
    MOVABLE = 2,
    Movable = 2
}
export enum SwipeEdgeEffect {
    SPRING = 0,
    Spring = 0,
    NONE = 1,
    None = 1
}
export enum SwipeActionState {
    COLLAPSED = 0,
    EXPANDED = 1,
    ACTIONING = 2
}
export enum ListItemStyle {
    NONE = 0,
    CARD = 1
}
export enum ListItemGroupStyle {
    NONE = 0,
    CARD = 1
}
export enum LoadingProgressStyle {
    DEFAULT = 0,
    Default = 0,
    CIRCULAR = 1,
    Circular = 1,
    ORBITAL = 2,
    Orbital = 2
}
export enum LocationIconStyle {
    FULL_FILLED = 0,
    LINES = 1
}
export enum LocationDescription {
    CURRENT_LOCATION = 0,
    ADD_LOCATION = 1,
    SELECT_LOCATION = 2,
    SHARE_LOCATION = 3,
    SEND_LOCATION = 4,
    LOCATING = 5,
    LOCATION = 6,
    SEND_CURRENT_LOCATION = 7,
    RELOCATION = 8,
    PUNCH_IN = 9,
    CURRENT_POSITION = 10
}
export enum LocationButtonOnClickResult {
    SUCCESS = 0,
    TEMPORARY_AUTHORIZATION_FAILED = 1
}
export enum SubMenuExpandingMode {
    SIDE_EXPAND = 0,
    EMBEDDED_EXPAND = 1,
    STACK_EXPAND = 2
}
export enum NavigationSystemTransitionType {
    DEFAULT = 0,
    NONE = 1,
    TITLE = 2,
    CONTENT = 3
}
export enum NavDestinationMode {
    STANDARD = 0,
    DIALOG = 1
}
export enum NavRouteMode {
    PUSH_WITH_RECREATE = 0,
    PUSH = 1,
    REPLACE = 2
}
export enum NavigationMode {
    STACK = 0,
    Stack = 0,
    SPLIT = 1,
    Split = 1,
    AUTO = 2,
    Auto = 2
}
export enum NavBarPosition {
    START = 0,
    Start = 0,
    END = 1,
    End = 1
}
export enum NavigationTitleMode {
    FREE = 0,
    Free = 0,
    FULL = 1,
    Full = 1,
    MINI = 2,
    Mini = 2
}
export enum LaunchMode {
    STANDARD = 0,
    MOVE_TO_TOP_SINGLETON = 1,
    POP_TO_SINGLETON = 2,
    NEW_INSTANCE = 3
}
export enum ToolbarItemStatus {
    NORMAL = 0,
    DISABLED = 1,
    ACTIVE = 2
}
export enum NavigationOperation {
    PUSH = 1,
    POP = 2,
    REPLACE = 3
}
export enum BarStyle {
    STANDARD = 0,
    STACK = 1,
    SAFE_AREA_PADDING = 2
}
export enum NavigationType {
    PUSH = 0,
    Push = 0,
    BACK = 1,
    Back = 1,
    REPLACE = 2,
    Replace = 2
}
export enum RouteType {
    NONE = 0,
    None = 0,
    PUSH = 1,
    Push = 1,
    POP = 2,
    Pop = 2
}
export enum SlideEffect {
    LEFT = 0,
    Left = 0,
    RIGHT = 1,
    Right = 1,
    TOP = 2,
    Top = 2,
    BOTTOM = 3,
    Bottom = 3,
    START = 5,
    END = 6
}
export enum PanelMode {
    MINI = 0,
    Mini = 0,
    HALF = 1,
    Half = 1,
    FULL = 2,
    Full = 2
}
export enum PanelType {
    MINIBAR = 0,
    Minibar = 0,
    FOLDABLE = 1,
    Foldable = 1,
    TEMPORARY = 2,
    Temporary = 2,
    CUSTOM = 3
}
export enum PanelHeight {
    WRAP_CONTENT = "wrapContent"
}
export enum ParticleType {
    POINT = "point",
    IMAGE = "image"
}
export enum ParticleEmitterShape {
    RECTANGLE = "rectangle",
    CIRCLE = "circle",
    ELLIPSE = "ellipse"
}
export enum DistributionType {
    UNIFORM = 0,
    GAUSSIAN = 1
}
export enum ParticleUpdater {
    NONE = "none",
    RANDOM = "random",
    CURVE = "curve"
}
export enum DisturbanceFieldShape {
    RECT = 0,
    CIRCLE = 1,
    ELLIPSE = 2
}
export enum PasteIconStyle {
    LINES = 0
}
export enum PasteDescription {
    PASTE = 0
}
export enum PasteButtonOnClickResult {
    SUCCESS = 0,
    TEMPORARY_AUTHORIZATION_FAILED = 1
}
export enum PatternLockChallengeResult {
    CORRECT = 1,
    WRONG = 2
}
export enum ProgressType {
    LINEAR = 0,
    Linear = 0,
    RING = 1,
    Ring = 1,
    ECLIPSE = 2,
    Eclipse = 2,
    SCALE_RING = 3,
    ScaleRing = 3,
    CAPSULE = 4,
    Capsule = 4
}
export enum ProgressStatus {
    LOADING = 0,
    PROGRESSING = 1
}
export enum ProgressStyle {
    LINEAR = 0,
    Linear = 0,
    RING = 1,
    Ring = 1,
    ECLIPSE = 2,
    Eclipse = 2,
    SCALE_RING = 3,
    ScaleRing = 3,
    CAPSULE = 4,
    Capsule = 4
}
export enum RadioIndicatorType {
    TICK = 0,
    DOT = 1,
    CUSTOM = 2
}
export enum RefreshStatus {
    INACTIVE = 0,
    Inactive = 0,
    DRAG = 1,
    Drag = 1,
    OVER_DRAG = 2,
    OverDrag = 2,
    REFRESH = 3,
    Refresh = 3,
    DONE = 4,
    Done = 4
}
export enum BarrierDirection {
    LEFT = 0,
    RIGHT = 1,
    TOP = 2,
    BOTTOM = 3
}
export enum LocalizedBarrierDirection {
    START = 0,
    END = 1,
    TOP = 2,
    BOTTOM = 3
}
export enum RichEditorDeleteDirection {
    BACKWARD = 0,
    FORWARD = 1
}
export enum RichEditorSpanType {
    TEXT = 0,
    IMAGE = 1,
    MIXED = 2,
    BUILDER = 3
}
export enum RichEditorResponseType {
    RIGHT_CLICK = 0,
    LONG_PRESS = 1,
    SELECT = 2
}
export enum SaveIconStyle {
    FULL_FILLED = 0,
    LINES = 1,
    PICTURE = 2
}
export enum SaveDescription {
    DOWNLOAD = 0,
    DOWNLOAD_FILE = 1,
    SAVE = 2,
    SAVE_IMAGE = 3,
    SAVE_FILE = 4,
    DOWNLOAD_AND_SHARE = 5,
    RECEIVE = 6,
    CONTINUE_TO_RECEIVE = 7,
    SAVE_TO_GALLERY = 8,
    EXPORT_TO_GALLERY = 9,
    QUICK_SAVE_TO_GALLERY = 10,
    RESAVE_TO_GALLERY = 11
}
export enum SaveButtonOnClickResult {
    SUCCESS = 0,
    TEMPORARY_AUTHORIZATION_FAILED = 1
}
export enum ScrollDirection {
    VERTICAL = 0,
    Vertical = 0,
    HORIZONTAL = 1,
    Horizontal = 1,
    FREE = 2,
    Free = 2,
    NONE = 3,
    None = 3
}
export enum ScrollAlign {
    START = 0,
    CENTER = 1,
    END = 2,
    AUTO = 3
}
export enum ScrollBarDirection {
    VERTICAL = 0,
    Vertical = 0,
    HORIZONTAL = 1,
    Horizontal = 1
}
export enum CancelButtonStyle {
    CONSTANT = 0,
    INVISIBLE = 1,
    INPUT = 2
}
export enum SearchType {
    NORMAL = 0,
    NUMBER = 2,
    PHONE_NUMBER = 3,
    EMAIL = 5,
    NUMBER_DECIMAL = 12,
    URL = 13
}
export enum SecurityComponentLayoutDirection {
    HORIZONTAL = 0,
    VERTICAL = 1
}
export enum ArrowPosition {
    END = 0,
    START = 1
}
export enum MenuAlignType {
    START = 0,
    CENTER = 1,
    END = 2
}
export enum SliderStyle {
    OUT_SET = 0,
    OutSet = 0,
    IN_SET = 1,
    InSet = 1,
    NONE = 2
}
export enum SliderChangeMode {
    BEGIN = 0,
    Begin = 0,
    MOVING = 1,
    Moving = 1,
    END = 2,
    End = 2,
    CLICK = 3,
    Click = 3
}
export enum SliderInteraction {
    SLIDE_AND_CLICK = 0,
    SLIDE_ONLY = 1,
    SLIDE_AND_CLICK_UP = 2
}
export enum SliderBlockType {
    DEFAULT = 0,
    IMAGE = 1,
    SHAPE = 2
}
export enum ColorMode {
    LIGHT = 0,
    DARK = 1
}
export enum LayoutDirection {
    LTR = 0,
    RTL = 1,
    AUTO = 2,
    Auto = 2
}
export enum ItemState {
    NORMAL = 0,
    Normal = 0,
    DISABLED = 1,
    Disabled = 1,
    WAITING = 2,
    Waiting = 2,
    SKIP = 3,
    Skip = 3
}
export enum SwiperDisplayMode {
    LEGACY_STRETCH = 0,
    Stretch = 0,
    LEGACY_AUTO_LINEAR = 1,
    AutoLinear = 1,
    STRETCH = 2,
    AUTO_LINEAR = 3
}
export enum SwiperNestedScrollMode {
    SELF_ONLY = 0,
    SELF_FIRST = 1
}
export enum SymbolRenderingStrategy {
    SINGLE = 0,
    MULTIPLE_COLOR = 1,
    MULTIPLE_OPACITY = 2
}
export enum SymbolEffectStrategy {
    NONE = 0,
    SCALE = 1,
    HIERARCHICAL = 2
}
export enum EffectFillStyle {
    CUMULATIVE = 0,
    ITERATIVE = 1
}
export enum BarMode {
    SCROLLABLE = 0,
    Scrollable = 0,
    FIXED = 1,
    Fixed = 1
}
export enum AnimationMode {
    CONTENT_FIRST = 0,
    ACTION_FIRST = 1,
    NO_ANIMATION = 2
}
export enum BarPosition {
    START = 0,
    Start = 0,
    END = 1,
    End = 1
}
export enum LayoutStyle {
    ALWAYS_CENTER = 0,
    ALWAYS_AVERAGE_SPLIT = 1,
    SPACE_BETWEEN_OR_CENTER = 2
}
export enum SelectedMode {
    INDICATOR = 0,
    BOARD = 1
}
export enum LayoutMode {
    AUTO = 0,
    VERTICAL = 1,
    HORIZONTAL = 2
}
export enum TextSpanType {
    TEXT = 0,
    IMAGE = 1,
    MIXED = 2
}
export enum TextResponseType {
    RIGHT_CLICK = 0,
    LONG_PRESS = 1,
    SELECT = 2
}
export enum TextAreaType {
    NORMAL = 0,
    NUMBER = 2,
    PHONE_NUMBER = 3,
    EMAIL = 5,
    NUMBER_DECIMAL = 12,
    URL = 13
}
export enum ContentType {
    USER_NAME = 0,
    PASSWORD = 1,
    NEW_PASSWORD = 2,
    FULL_STREET_ADDRESS = 3,
    HOUSE_NUMBER = 4,
    DISTRICT_ADDRESS = 5,
    CITY_ADDRESS = 6,
    PROVINCE_ADDRESS = 7,
    COUNTRY_ADDRESS = 8,
    PERSON_FULL_NAME = 9,
    PERSON_LAST_NAME = 10,
    PERSON_FIRST_NAME = 11,
    PHONE_NUMBER = 12,
    PHONE_COUNTRY_CODE = 13,
    FULL_PHONE_NUMBER = 14,
    EMAIL_ADDRESS = 15,
    BANK_CARD_NUMBER = 16,
    ID_CARD_NUMBER = 17,
    NICKNAME = 23,
    DETAIL_INFO_WITHOUT_STREET = 24,
    FORMAT_ADDRESS = 25
}
export enum TextDataDetectorType {
    PHONE_NUMBER = 0,
    URL = 1,
    EMAIL = 2,
    ADDRESS = 3,
    DATE_TIME = 4
}
export enum TextDeleteDirection {
    BACKWARD = 0,
    FORWARD = 1
}
export enum MenuType {
    SELECTION_MENU = 0,
    PREVIEW_MENU = 1
}
export enum InputType {
    NORMAL = 0,
    Normal = 0,
    NUMBER = 1,
    Number = 1,
    PHONE_NUMBER = 2,
    PhoneNumber = 2,
    EMAIL = 3,
    Email = 3,
    PASSWORD = 4,
    Password = 4,
    NUMBER_PASSWORD = 8,
    SCREEN_LOCK_PASSWORD = 9,
    USER_NAME = 10,
    NEW_PASSWORD = 11,
    NUMBER_DECIMAL = 12,
    URL = 13
}
export enum EnterKeyType {
    GO = 2,
    Go = 2,
    SEARCH = 3,
    Search = 3,
    SEND = 4,
    Send = 4,
    NEXT = 5,
    Next = 5,
    DONE = 6,
    Done = 6,
    PREVIOUS = 7,
    NEW_LINE = 8
}
export enum TextInputStyle {
    DEFAULT = 0,
    Default = 0,
    INLINE = 1,
    Inline = 1
}
export enum TimePickerFormat {
    HOUR_MINUTE = 0,
    HOUR_MINUTE_SECOND = 1
}
export enum ToggleType {
    CHECKBOX = 0,
    Checkbox = 0,
    SWITCH = 1,
    Switch = 1,
    BUTTON = 2,
    Button = 2
}
export enum SeekMode {
    PREVIOUS_KEYFRAME = 0,
    PreviousKeyframe = 0,
    NEXT_KEYFRAME = 1,
    NextKeyframe = 1,
    CLOSEST_KEYFRAME = 2,
    ClosestKeyframe = 2,
    ACCURATE = 3,
    Accurate = 3
}
export enum PlaybackSpeed {
    SPEED_FORWARD_0_75_X = 0,
    Speed_Forward_0_75_X = 0,
    SPEED_FORWARD_1_00_X = 1,
    Speed_Forward_1_00_X = 1,
    SPEED_FORWARD_1_25_X = 2,
    Speed_Forward_1_25_X = 2,
    SPEED_FORWARD_1_75_X = 3,
    Speed_Forward_1_75_X = 3,
    SPEED_FORWARD_2_00_X = 4,
    Speed_Forward_2_00_X = 4
}
export enum MessageLevel {
    DEBUG = 0,
    Debug = 0,
    ERROR = 1,
    Error = 1,
    INFO = 2,
    Info = 2,
    LOG = 3,
    Log = 3,
    WARN = 4,
    Warn = 4
}
export enum MixedMode {
    ALL = 0,
    All = 0,
    COMPATIBLE = 1,
    Compatible = 1,
    NONE = 2,
    None = 2
}
export enum HitTestType {
    EDIT_TEXT = 0,
    EditText = 0,
    EMAIL = 1,
    Email = 1,
    HTTP_ANCHOR = 2,
    HttpAnchor = 2,
    HTTP_ANCHOR_IMG = 3,
    HttpAnchorImg = 3,
    IMG = 4,
    Img = 4,
    MAP = 5,
    Map = 5,
    PHONE = 6,
    Phone = 6,
    UNKNOWN = 7,
    Unknown = 7
}
export enum CacheMode {
    DEFAULT = 0,
    Default = 0,
    NONE = 1,
    None = 1,
    ONLINE = 2,
    Online = 2,
    ONLY = 3,
    Only = 3
}
export enum OverScrollMode {
    NEVER = 0,
    ALWAYS = 1
}
export enum WebDarkMode {
    OFF = 0,
    Off = 0,
    ON = 1,
    On = 1,
    AUTO = 2,
    Auto = 2
}
export enum WebCaptureMode {
    HOME_SCREEN = 0
}
export enum ThreatType {
    THREAT_ILLEGAL = 0,
    THREAT_FRAUD = 1,
    THREAT_RISK = 2,
    THREAT_WARNING = 3
}
export enum RenderExitReason {
    PROCESS_ABNORMAL_TERMINATION = 0,
    ProcessAbnormalTermination = 0,
    PROCESS_WAS_KILLED = 1,
    ProcessWasKilled = 1,
    PROCESS_CRASHED = 2,
    ProcessCrashed = 2,
    PROCESS_OOM = 3,
    ProcessOom = 3,
    PROCESS_EXIT_UNKNOWN = 4,
    ProcessExitUnknown = 4
}
export enum SslError {
    INVALID = 0,
    Invalid = 0,
    HOST_MISMATCH = 1,
    HostMismatch = 1,
    DATE_INVALID = 2,
    DateInvalid = 2,
    UNTRUSTED = 3,
    Untrusted = 3
}
export enum FileSelectorMode {
    FILE_OPEN_MODE = 0,
    FileOpenMode = 0,
    FILE_OPEN_MULTIPLE_MODE = 1,
    FileOpenMultipleMode = 1,
    FILE_OPEN_FOLDER_MODE = 2,
    FileOpenFolderMode = 2,
    FILE_SAVE_MODE = 3,
    FileSaveMode = 3
}
export enum WebLayoutMode {
    NONE = 0,
    FIT_CONTENT = 1
}
export enum RenderProcessNotRespondingReason {
    INPUT_TIMEOUT = 0,
    NAVIGATION_COMMIT_TIMEOUT = 1
}
export enum ProtectedResourceType {
    MIDI_SYSEX = "TYPE_MIDI_SYSEX",
    MidiSysex = "TYPE_MIDI_SYSEX",
    VIDEO_CAPTURE = "TYPE_VIDEO_CAPTURE",
    AUDIO_CAPTURE = "TYPE_AUDIO_CAPTURE",
    SENSOR = "TYPE_SENSOR"
}
export enum ContextMenuSourceType {
    NONE = 0,
    None = 0,
    MOUSE = 1,
    Mouse = 1,
    LONG_PRESS = 2,
    LongPress = 2
}
export enum ContextMenuMediaType {
    NONE = 0,
    None = 0,
    IMAGE = 1,
    Image = 1
}
export enum ContextMenuInputFieldType {
    NONE = 0,
    None = 0,
    PLAIN_TEXT = 1,
    PlainText = 1,
    PASSWORD = 2,
    Password = 2,
    NUMBER = 3,
    Number = 3,
    TELEPHONE = 4,
    Telephone = 4,
    OTHER = 5,
    Other = 5
}
export enum NativeEmbedStatus {
    CREATE = 0,
    UPDATE = 1,
    DESTROY = 2,
    ENTER_BFCACHE = 3,
    LEAVE_BFCACHE = 4
}
export enum ContextMenuEditStateFlags {
    NONE = 0,
    CAN_CUT = 1,
    CAN_COPY = 2,
    CAN_PASTE = 4,
    CAN_SELECT_ALL = 8
}
export enum WebNavigationType {
    UNKNOWN = 0,
    MAIN_FRAME_NEW_ENTRY = 1,
    MAIN_FRAME_EXISTING_ENTRY = 2,
    NAVIGATION_TYPE_NEW_SUBFRAME = 4,
    NAVIGATION_TYPE_AUTO_SUBFRAME = 5
}
export enum RenderMode {
    ASYNC_RENDER = 0,
    SYNC_RENDER = 1
}
export enum ViewportFit {
    AUTO = 0,
    CONTAINS = 1,
    COVER = 2
}
export enum WebKeyboardAvoidMode {
    RESIZE_VISUAL = 0,
    RESIZE_CONTENT = 1,
    OVERLAYS_CONTENT = 2
}
export enum WebElementType {
    IMAGE = 1
}
export enum WebResponseType {
    LONG_PRESS = 1
}
export enum SideBarContainerType {
    EMBED = 0,
    Embed = 0,
    OVERLAY = 1,
    Overlay = 1,
    AUTO = 2
}
export enum SideBarPosition {
    START = 0,
    Start = 0,
    END = 1,
    End = 1
}
export enum WaterFlowLayoutMode {
    ALWAYS_TOP_DOWN = 0,
    SLIDING_WINDOW = 1
}
export enum DpiFollowStrategy {
    FOLLOW_HOST_DPI = 0,
    FOLLOW_UI_EXTENSION_ABILITY_DPI = 1
}
export enum StyledStringKey {
    FONT = 0,
    DECORATION = 1,
    BASELINE_OFFSET = 2,
    LETTER_SPACING = 3,
    TEXT_SHADOW = 4,
    LINE_HEIGHT = 5,
    BACKGROUND_COLOR = 6,
    URL = 7,
    GESTURE = 100,
    PARAGRAPH_STYLE = 200,
    IMAGE = 300,
    CUSTOM_SPAN = 400,
    USER_DATA = 500
}