#include "ScrollViewComponentInstance.h"
#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
#include <react/renderer/components/scrollview/ScrollViewState.h>
#include <react/renderer/core/ConcreteState.h>
#include <cmath>
#include <optional>
#include "CustomNodeComponentInstance.h"
#include "PullToRefreshViewComponentInstance.h"
#include "conversions.h"

namespace rnoh {

ScrollViewComponentInstance::ScrollViewComponentInstance(Context context)
    : CppComponentInstance(std::move(context)) {
  getLocalRootArkUINode().insertChild(m_scrollNode, 0);
  m_scrollNode.insertChild(m_contentContainerNode);
  // NOTE: perhaps this needs to take rtl into account?
  m_scrollNode.setAlignment(ARKUI_ALIGNMENT_TOP_START);
  m_scrollNode.setScrollNodeDelegate(this);
  m_scrollNode.setNestedScroll(ARKUI_SCROLL_NESTED_MODE_SELF_FIRST);
}

StackNode& ScrollViewComponentInstance::getLocalRootArkUINode() {
  return m_scrollContainerNode;
}

void ScrollViewComponentInstance::onChildInserted(
    ComponentInstance::Shared const& childComponentInstance,
    std::size_t index) {
  CppComponentInstance::onChildInserted(childComponentInstance, index);
  m_contentContainerNode.insertChild(
      childComponentInstance->getLocalRootArkUINode(), index);
}

void ScrollViewComponentInstance::onChildRemoved(
    ComponentInstance::Shared const& childComponentInstance) {
  CppComponentInstance::onChildRemoved(childComponentInstance);
  m_contentContainerNode.removeChild(
      childComponentInstance->getLocalRootArkUINode());
}

void ScrollViewComponentInstance::setLayout(
    facebook::react::LayoutMetrics layoutMetrics) {
  getLocalRootArkUINode().setSize(layoutMetrics.frame.size);
  m_scrollNode.setSize(layoutMetrics.frame.size);
  m_layoutMetrics = layoutMetrics;
  if (m_containerSize != layoutMetrics.frame.size) {
    m_containerSize = layoutMetrics.frame.size;
  }
  markBoundingBoxAsDirty();
}

void rnoh::ScrollViewComponentInstance::updateOffsetAfterChildChange(
    facebook::react::Point offset,
    double diff) {
  if (diff <= 0) {
    return;
  }

  if (isHorizontal(m_props)) {
    if (offset.x + m_containerSize.width <= m_contentSize.width) {
      return;
    }
  } else {
    if (offset.y + m_containerSize.height <= m_contentSize.height) {
      return;
    }
  }

  facebook::react::Point targetOffset = {offset.x, offset.y};
  if (isHorizontal(m_props)) {
    targetOffset.x = m_contentSize.width - m_containerSize.width;
  } else {
    targetOffset.y = m_contentSize.height - m_containerSize.height;
  }

  if (targetOffset.x < 0) {
    targetOffset.x = 0;
  }
  if (targetOffset.y < 0) {
    targetOffset.y = 0;
  }

  onScrollStart();
  m_scrollNode.scrollTo(
      targetOffset.x, targetOffset.y, false, m_scrollToOverflowEnabled);
}

void rnoh::ScrollViewComponentInstance::onStateChanged(
    SharedConcreteState const& state) {
  CppComponentInstance::onStateChanged(state);
  auto stateData = state->getData();
  if (m_contentSize != stateData.getContentSize()) {
    double diff = isHorizontal(m_props)
        ? m_contentSize.width - stateData.getContentSize().width
        : m_contentSize.height - stateData.getContentSize().height;
    m_contentContainerNode.setSize(stateData.getContentSize());
    m_contentSize = stateData.getContentSize();
    updateOffsetAfterChildChange(getCurrentOffset(), diff);
  }
}

void rnoh::ScrollViewComponentInstance::onPropsChanged(
    SharedConcreteProps const& props) {
  CppComponentInstance::onPropsChanged(props);

  /**
   * This block is needed to detects which batch of mutations were triggered by
   * appearance of the keyboard.
   * "__keyboardAvoidingViewBottomHeight" is injected by KeyboardAvoidingView.
   * ScrollView needs to be placed directly inside KeyboardAvoidingView.
   */
  double parentKeyboardAvoidingViewBottomHeight = 0;
  if (props->rawProps.count("__keyboardAvoidingViewBottomHeight") > 0) {
    parentKeyboardAvoidingViewBottomHeight =
        props->rawProps["__keyboardAvoidingViewBottomHeight"].asDouble();
  }
  double prevParentKeyboardAvoidingViewBottomHeight = 0;
  if (m_props != nullptr &&
      m_props->rawProps.count("__keyboardAvoidingViewBottomHeight") > 0) {
    prevParentKeyboardAvoidingViewBottomHeight =
        m_props->rawProps["__keyboardAvoidingViewBottomHeight"].asDouble();
  }
  auto keyboardAvoider = m_keyboardAvoider.lock();
  if (keyboardAvoider && parentKeyboardAvoidingViewBottomHeight > 0 &&
      parentKeyboardAvoidingViewBottomHeight !=
          prevParentKeyboardAvoidingViewBottomHeight) {
    m_shouldAdjustScrollPositionOnNextRender = true;
  }
  
  auto rawProps = ScrollViewRawProps::getFromDynamic(props->rawProps);

  if (props->rawProps.count("persistentScrollbar") > 0) {
    m_persistentScrollbar = props->rawProps["persistentScrollbar"].asBool();
  }
  m_scrollEventThrottle = props->scrollEventThrottle;
  m_disableIntervalMomentum = props->disableIntervalMomentum;
  m_scrollToOverflowEnabled = props->scrollToOverflowEnabled;
  m_scrollNode.setHorizontal(isHorizontal(props))
      .setFriction(getFrictionFromDecelerationRate(props->decelerationRate))
      .setScrollBarDisplayMode(getScrollBarDisplayMode(
          isHorizontal(props),
          m_persistentScrollbar,
          props->showsVerticalScrollIndicator,
          props->showsHorizontalScrollIndicator))
      .setScrollBarColor(
          props->indicatorStyle ==
                  facebook::react::ScrollViewIndicatorStyle::White
              ? 0x66FFFFFF
              : 0x66000000)
      .setEnablePaging(props->pagingEnabled);

  if (rawProps.overScrollMode.has_value()) {
    if (m_rawProps.overScrollMode != rawProps.overScrollMode) {
      m_rawProps.overScrollMode = rawProps.overScrollMode;
      m_scrollNode.setScrollOverScrollMode(m_rawProps.overScrollMode.value());
    }
  } else {
    if (!m_props || props->bounces != m_props->bounces ||
        (isHorizontal(props) && props->alwaysBounceHorizontal != m_props->alwaysBounceHorizontal) ||
        (!isHorizontal(props) && props->alwaysBounceVertical != m_props->alwaysBounceVertical)) {
      m_scrollNode.setEdgeEffect(props->bounces,
        isHorizontal(props) ? props->alwaysBounceHorizontal : props->alwaysBounceVertical);
    }
  }

    
  if (rawProps.nestedScrollEnabled.has_value()) {
     m_rawProps.nestedScrollEnabled = rawProps.nestedScrollEnabled;
  }
  auto newEnableScrollInteraction = isEnableScrollInteraction(props && props->scrollEnabled);
  if (newEnableScrollInteraction != m_enableScrollInteraction) {
    m_enableScrollInteraction = newEnableScrollInteraction;
    m_scrollNode.setEnableScrollInteraction(m_enableScrollInteraction);
  }  
    
  if (m_rawProps.endFillColor != rawProps.endFillColor) {
    m_rawProps.endFillColor = rawProps.endFillColor;
    if (m_rawProps.endFillColor.has_value()) {
      m_scrollNode.setEndFillColor(m_rawProps.endFillColor.value());
    }
  }

  if (!m_props || props->contentOffset != m_props->contentOffset ||
      props->scrollToOverflowEnabled != m_props->scrollToOverflowEnabled) {
    m_scrollNode.scrollTo(
        props->contentOffset.x, props->contentOffset.y, false, m_scrollToOverflowEnabled);
    updateStateWithContentOffset(props->contentOffset);
  }
  if (!m_props || props->centerContent != m_props->centerContent) {
    if (props->centerContent) {
      m_scrollNode.setCenterContent(true);
    } else {
      m_scrollNode.setCenterContent(false);
    }
  }
  setScrollSnap(
      props->snapToStart,
      props->snapToEnd,
      props->snapToOffsets,
      props->snapToInterval,
      props->snapToAlignment);

  auto borderMetrics = props->resolveBorderMetrics(m_layoutMetrics);
  m_contentContainerNode.setMargin(
      -borderMetrics.borderWidths.left,
      -borderMetrics.borderWidths.top,
      0.f,
      0.f);
}

void ScrollViewComponentInstance::onCommandReceived(
    std::string const& commandName,
    folly::dynamic const& args) {
  if (commandName == "scrollTo") {
    facebook::react::Float x = args[0].asDouble();
    facebook::react::Float y = args[1].asDouble();
    m_targetOffsetOfScrollToCommand = {x, y};
    m_scrollNode.scrollTo(
        x,
        y,
        args[2].asBool(),
        m_scrollToOverflowEnabled);
  } else if (commandName == "scrollToEnd") {
    scrollToEnd(args[0].asBool());
  }
}

void rnoh::ScrollViewComponentInstance::onNativeResponderBlockChange(
    bool isBlocked) {
  m_isNativeResponderBlocked = isBlocked;
  auto newEnableScrollInteraction = isEnableScrollInteraction(m_props && m_props->scrollEnabled);
  if (newEnableScrollInteraction != m_enableScrollInteraction) {
    m_enableScrollInteraction = newEnableScrollInteraction;  
    m_scrollNode.setEnableScrollInteraction(m_enableScrollInteraction);
  }

  auto parent =std::dynamic_pointer_cast<rnoh::PullToRefreshViewComponentInstance>(this->getParent().lock());
    if (parent){
            if (isBlocked) {
            parent->setRefreshPullDownRation(0.0);
            } else {
            parent->setRefreshPullDownRation(1.0);
        }
    }  

  m_scrollNode.markDirty();
}

bool rnoh::ScrollViewComponentInstance::isEnableScrollInteraction(bool scrollEnabled) {
  if (m_isNativeResponderBlocked) {
        return false;
  }  
  if (!scrollEnabled) {
       return false; 
  }
  if (m_rawProps.nestedScrollEnabled.has_value() && !m_rawProps.nestedScrollEnabled.value() && isNestedScroll()) {
     return false;   
  }
    
  return true;
}

bool rnoh::ScrollViewComponentInstance::isNestedScroll() {
    auto parent = m_parent.lock();
    while(parent) {
        // TODO: how to judge a component is scrollable component.
        if (parent->getComponentName() == this->getComponentName()) {
            return true;
        }
        parent = parent->getParent().lock();
    }
    return false;
}

facebook::react::Point rnoh::ScrollViewComponentInstance::getCurrentOffset()
    const {
  auto offset = m_scrollNode.getScrollOffset();
  auto contentViewOffset = getContentViewOffset();
  return offset - contentViewOffset;
}

void rnoh::ScrollViewComponentInstance::updateStateWithContentOffset(
    facebook::react::Point contentOffset) {
  if (!m_state) {
    return;
  }
  m_state->updateState([contentOffset](auto const& stateData) {
    auto newData = stateData;
    newData.contentOffset = contentOffset;
    return std::make_shared<
        facebook::react::ScrollViewShadowNode::ConcreteState::Data const>(
        newData);
  });
}

facebook::react::ScrollViewMetrics
ScrollViewComponentInstance::getScrollViewMetrics() {
  auto scrollViewMetrics = facebook::react::ScrollViewMetrics();
  scrollViewMetrics.responderIgnoreScroll = true;
  scrollViewMetrics.zoomScale = 1;
  scrollViewMetrics.contentSize = m_contentSize;
  scrollViewMetrics.contentOffset = m_scrollNode.getScrollOffset();
  scrollViewMetrics.containerSize = m_containerSize;
  return scrollViewMetrics;
}

bool ScrollViewComponentInstance::isHandlingTouches() const {
  return m_scrollState != IDLE;
}

void ScrollViewComponentInstance::onScroll() {
  auto scrollViewMetrics = getScrollViewMetrics();
  if (!isContentSmallerThanContainer() && m_allowScrollPropagation &&
      !isAtEnd(scrollViewMetrics.contentOffset)) {
    m_scrollNode.setNestedScroll(ARKUI_SCROLL_NESTED_MODE_SELF_ONLY);
    m_allowScrollPropagation = false;
  }
  auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now().time_since_epoch())
                 .count();
  if (m_allowNextScrollEvent || isCloseToTargetOffset(scrollViewMetrics.contentOffset) ||
      ((m_scrollEventThrottle == 0 ||
      m_scrollEventThrottle < now - m_lastScrollDispatchTime) &&
       scrollMovedBySignificantOffset(scrollViewMetrics.contentOffset))) {
    m_lastScrollDispatchTime = now;
    VLOG(2) << "onScroll (contentOffset: " << scrollViewMetrics.contentOffset.x
            << ", " << scrollViewMetrics.contentOffset.y
            << "; contentSize: " << scrollViewMetrics.contentSize.width << ", "
            << scrollViewMetrics.contentSize.height
            << "; containerSize: " << scrollViewMetrics.containerSize.width
            << ", " << scrollViewMetrics.containerSize.height << ")";
    m_eventEmitter->onScroll(scrollViewMetrics);
    updateStateWithContentOffset(scrollViewMetrics.contentOffset);
    sendEventForNativeAnimations(scrollViewMetrics);
    m_currentOffset = scrollViewMetrics.contentOffset;
    updateContentClippedSubviews();
  }
}

void ScrollViewComponentInstance::onScrollStart() {
  m_allowNextScrollEvent = false;
}

void ScrollViewComponentInstance::onScrollStop() {
  m_allowNextScrollEvent = true;

  if (m_scrollState == ScrollState::FLING) {
    emitOnMomentumScrollEndEvent();
  } else if (m_scrollState == ScrollState::SCROLL) {
    emitOnScrollEndDragEvent();
  }
  m_scrollState = ScrollState::IDLE;
  if (!isContentSmallerThanContainer() && !m_allowScrollPropagation &&
      isAtEnd(m_currentOffset)) {
    m_scrollNode.setNestedScroll(ARKUI_SCROLL_NESTED_MODE_SELF_FIRST);
    m_allowScrollPropagation = true;
  }
  auto scrollViewMetrics = getScrollViewMetrics();
  if (isAtEnd(scrollViewMetrics.contentOffset)) {
    m_eventEmitter->onScroll(scrollViewMetrics);
    sendEventForNativeAnimations(scrollViewMetrics);
  }
}

float ScrollViewComponentInstance::onScrollFrameBegin(
    float offset,
    int32_t scrollState) {
  if (!m_props->scrollEnabled) {
    m_recentScrollFrameOffset = 0;
    return 0;
  }
  m_recentScrollFrameOffset = offset;
  auto newScrollState = static_cast<ScrollState>(scrollState);
  if (m_scrollState != newScrollState) {
    if (m_scrollState == ScrollState::SCROLL) {
      emitOnScrollEndDragEvent();
    } else if (m_scrollState == ScrollState::FLING) {
      emitOnMomentumScrollEndEvent();
    }
    auto scrollViewMetrics = getScrollViewMetrics();
    if (scrollState == ScrollState::SCROLL) {
      m_eventEmitter->onScrollBeginDrag(scrollViewMetrics);
    } else if (scrollState == ScrollState::FLING) {
      m_eventEmitter->onMomentumScrollBegin(scrollViewMetrics);
    }
  }
  m_scrollState = newScrollState;
  return offset;
}

void ScrollViewComponentInstance::emitOnScrollEndDragEvent() {
  if (m_disableIntervalMomentum) {
    disableIntervalMomentum();
  }
  auto scrollViewMetrics = getScrollViewMetrics();
  m_eventEmitter->onScrollEndDrag(scrollViewMetrics);
  updateStateWithContentOffset(scrollViewMetrics.contentOffset);
}
void ScrollViewComponentInstance::emitOnMomentumScrollEndEvent() {
  auto scrollViewMetrics = getScrollViewMetrics();
  m_eventEmitter->onMomentumScrollEnd(scrollViewMetrics);
  updateStateWithContentOffset(scrollViewMetrics.contentOffset);
}

void ScrollViewComponentInstance::updateContentClippedSubviews(bool childrenChange) {
  if (!m_children.empty() && m_children[0] != nullptr) {
    auto contentContainer =
        std::dynamic_pointer_cast<CustomNodeComponentInstance>(m_children[0]);
    if (contentContainer != nullptr) {
      contentContainer->updateClippedSubviews(childrenChange);
    }
  }
}

facebook::react::Float
ScrollViewComponentInstance::getFrictionFromDecelerationRate(
    facebook::react::Float decelerationRate) {
  // default deceleration rate and friction values differ between ArkUI and RN
  // so we adapt the decelerationRate accordingly to resemble iOS behaviour
  // iOS's UIScrollView supports only two values of decelerationRate
  // called 'normal' and 'fast' and maps all other to the nearest of those two
  facebook::react::Float IOS_NORMAL = 0.998;
  facebook::react::Float IOS_FAST = 0.99;
  facebook::react::Float ARKUI_FAST = 2;
  facebook::react::Float ARKUI_NORMAL = 0.75;
  if (decelerationRate < (IOS_NORMAL + IOS_FAST) / 2) {
    return ARKUI_FAST;
  } else {
    return ARKUI_NORMAL;
  }
}

void ScrollViewComponentInstance::scrollToEnd(bool animated) {
  bool horizontal = false;
  if(m_props){
        horizontal = m_props->alwaysBounceHorizontal ||
      m_contentSize.width > m_containerSize.width;
  }
  auto x = horizontal ? m_contentSize.width : 0.0;
  auto y = horizontal ? 0.0 : m_contentSize.height;
  m_scrollNode.scrollTo(x, y, animated);
}

ArkUI_ScrollBarDisplayMode ScrollViewComponentInstance::getScrollBarDisplayMode(
    bool horizontal,
    bool persistentScrollBar,
    bool showsVerticalScrollIndicator,
    bool showsHorizontalScrollIndicator) {
  if (horizontal && !showsHorizontalScrollIndicator ||
      !horizontal && !showsVerticalScrollIndicator) {
    return ArkUI_ScrollBarDisplayMode::ARKUI_SCROLL_BAR_DISPLAY_MODE_OFF;
  }
  return persistentScrollBar
      ? ArkUI_ScrollBarDisplayMode::ARKUI_SCROLL_BAR_DISPLAY_MODE_ON
      : ArkUI_ScrollBarDisplayMode::ARKUI_SCROLL_BAR_DISPLAY_MODE_AUTO;
}

void ScrollViewComponentInstance::setScrollSnap(
    bool snapToStart,
    bool snapToEnd,
    const std::vector<facebook::react::Float>& snapToOffsets,
    facebook::react::Float snapToInterval,
    facebook::react::ScrollViewSnapToAlignment snapToAlignment) {
  if (!snapToOffsets.empty()) {
    m_snapToOffsets = snapToOffsets;
    m_snapToOffsets.erase(
        std::remove_if(
            m_snapToOffsets.begin(),
            m_snapToOffsets.end(),
            [](facebook::react::Float x) { return std::isnan(x); }),
        m_snapToOffsets.end());
    std::sort(m_snapToOffsets.begin(), m_snapToOffsets.end());

    if (m_snapToOffsets.size() == 1) {
      m_snapToOffsets.push_back(
          std::numeric_limits<facebook::react::Float>::infinity());
    }
    m_scrollNode.setScrollSnap(
        ArkUI_ScrollSnapAlign::ARKUI_SCROLL_SNAP_ALIGN_START,
        snapToStart,
        snapToEnd,
        m_snapToOffsets);
  } else if (snapToInterval > 0) {
    const std::vector<facebook::react::Float> snapPoints = {snapToInterval};
    m_scrollNode.setScrollSnap(
        getArkUI_ScrollSnapAlign(snapToAlignment),
        snapToStart,
        snapToEnd,
        snapPoints);
  } else {
    m_scrollNode.resetScrollSnap();
  }
}
bool ScrollViewComponentInstance::scrollMovedBySignificantOffset(
    facebook::react::Point newOffset) {
  return std::abs(newOffset.x - m_currentOffset.x) >= 0.01 ||
      std::abs(newOffset.y - m_currentOffset.y) >= 0.01;
}

void ScrollViewComponentInstance::onFinalizeUpdates() {
  ComponentInstance::onFinalizeUpdates();

  // when parent isn't refresh node, set the position
  auto parent = this->getParent().lock();
  if (parent && !parent->isRefreshControlComponentInstance()) {
    this->getLocalRootArkUINode().setLayoutRect(
      m_layoutMetrics.frame.origin, m_layoutMetrics.frame.size, m_layoutMetrics.pointScaleFactor);
  }
  if (m_props && m_props->maintainVisibleContentPosition.has_value()) {
    adjustVisibleContentPosition(
        m_props->maintainVisibleContentPosition.value());
    m_firstVisibleView = getFirstVisibleView(
        m_props->maintainVisibleContentPosition.value().minIndexForVisible);
  }

  if (m_shouldAdjustScrollPositionOnNextRender) {
    auto maybeKeyboardAvoider = m_keyboardAvoider.lock();
    if (maybeKeyboardAvoider != nullptr) {
      auto keyboardAvoider =
          std::dynamic_pointer_cast<KeyboardAvoider>(maybeKeyboardAvoider);
      if (keyboardAvoider != nullptr) {
        auto scrollOffset =
            keyboardAvoider->getBottomEdgeOffsetRelativeToScrollView(
                std::dynamic_pointer_cast<ScrollViewComponentInstance>(
                    this->shared_from_this()));
        auto newScrollOffset = scrollOffset - m_layoutMetrics.frame.size.height;
        if (isHorizontal(m_props)) {
          if (newScrollOffset > m_scrollNode.getScrollOffset().x) {
            m_scrollNode.scrollTo(
                newScrollOffset, m_scrollNode.getScrollOffset().y, true);
          }
        } else {
          if (newScrollOffset > m_scrollNode.getScrollOffset().y) {
            LOG(INFO)
                << "Adjusting scroll position to prevent keyboard avoider being hidden by the keyboard";
            m_scrollNode.scrollTo(
                m_scrollNode.getScrollOffset().x, newScrollOffset, true);
          }
        }
        m_keyboardAvoider.reset();
      }
    }
    m_shouldAdjustScrollPositionOnNextRender = false;
  }
}

folly::dynamic ScrollViewComponentInstance::getScrollEventPayload(
    facebook::react::ScrollViewMetrics const& scrollViewMetrics) {
  using folly::dynamic;

  dynamic contentSize =
      dynamic::object("width", scrollViewMetrics.contentSize.width)(
          "height", scrollViewMetrics.contentSize.height);
  dynamic contentOffset =
      dynamic::object("x", scrollViewMetrics.contentOffset.x)(
          "y", scrollViewMetrics.contentOffset.y);
  dynamic contentInset =
      dynamic::object("left", scrollViewMetrics.contentInset.left)(
          "top", scrollViewMetrics.contentInset.top)(
          "right", scrollViewMetrics.contentInset.right)(
          "bottom", scrollViewMetrics.contentInset.bottom);
  dynamic containerSize =
      dynamic::object("width", scrollViewMetrics.containerSize.width)(
          "height", scrollViewMetrics.containerSize.height);
  dynamic payload = dynamic::object("contentSize", contentSize)(
      "contentOffset", contentOffset)("contentInset", contentInset)(
      "containerSize", containerSize)("zoomScale", scrollViewMetrics.zoomScale)(
      "responderIgnoreScroll", scrollViewMetrics.responderIgnoreScroll);
  return payload;
}

void rnoh::ScrollViewComponentInstance::sendEventForNativeAnimations(
    facebook::react::ScrollViewMetrics const& scrollViewMetrics) {
  auto nativeAnimatedTurboModule = m_nativeAnimatedTurboModule.lock();
  if (nativeAnimatedTurboModule == nullptr) {
    auto instance = m_deps->rnInstance.lock();
    if (instance == nullptr) {
      return;
    }
    nativeAnimatedTurboModule =
        instance->getTurboModule<NativeAnimatedTurboModule>(
            "NativeAnimatedTurboModule");
    m_nativeAnimatedTurboModule = nativeAnimatedTurboModule;
  }
  if (nativeAnimatedTurboModule != nullptr) {
    nativeAnimatedTurboModule->handleComponentEvent(
        m_tag, "onScroll", getScrollEventPayload(scrollViewMetrics));
  }
}

bool ScrollViewComponentInstance::isContentSmallerThanContainer() {
  return isHorizontal(m_props) ? m_contentSize.width <= m_containerSize.width
                               : m_contentSize.height <= m_containerSize.height;
}

bool ScrollViewComponentInstance::isAtEnd(
    facebook::react::Point currentOffset) {
  if (isHorizontal(m_props)) {
    return currentOffset.x <= 0.001 ||
        m_contentSize.width - m_containerSize.width - currentOffset.x < 0.001;
  } else {
    return currentOffset.y <= 0.001 ||
        m_contentSize.height - m_containerSize.height - currentOffset.y < 0.001;
  }
}

bool ScrollViewComponentInstance::isCloseToTargetOffset(
    facebook::react::Point currentOffset) {
  if (m_targetOffsetOfScrollToCommand.has_value()) {
    auto flag = std::abs(m_targetOffsetOfScrollToCommand->x - currentOffset.x) < 0.001 &&
      std::abs(m_targetOffsetOfScrollToCommand->y - currentOffset.y) < 0.001;
    if (flag) {
      m_targetOffsetOfScrollToCommand = std::nullopt;
    }
    return flag;
  }
  return false;
}

bool ScrollViewComponentInstance::isHorizontal(
    SharedConcreteProps const& props) {
  if (props == nullptr) {
    return false;
  }
  return props->alwaysBounceHorizontal ||
      m_contentSize.width > m_containerSize.width;
}

void ScrollViewComponentInstance::disableIntervalMomentum() {
  if (m_props->pagingEnabled) {
    return;
  }
  auto nextSnapTarget = getNextSnapTarget();
  if (nextSnapTarget.has_value()) {
    if (isHorizontal(m_props)) {
      m_scrollNode.scrollTo(
          nextSnapTarget.value(), static_cast<float>(m_currentOffset.y), true, m_scrollToOverflowEnabled);
    } else {
      m_scrollNode.scrollTo(
          static_cast<float>(m_currentOffset.x), nextSnapTarget.value(), true, m_scrollToOverflowEnabled);
    }
  }
}

std::optional<float> ScrollViewComponentInstance::getNextSnapTarget() {
  std::optional<float> nextSnapTarget = std::nullopt;
  auto currentOffset =
      isHorizontal(m_props) ? m_currentOffset.x : m_currentOffset.y;

  if (!m_snapToOffsets.empty()) {
    if (m_recentScrollFrameOffset > 0) {
      auto upper = std::upper_bound(
          m_snapToOffsets.begin(), m_snapToOffsets.end(), currentOffset);
      if (upper != m_snapToOffsets.end() &&
          *upper < std::numeric_limits<facebook::react::Float>::infinity()) {
        nextSnapTarget = static_cast<float>(*upper);
      } else {
        nextSnapTarget =
            isHorizontal(m_props) ? m_contentSize.width : m_contentSize.height;
      }
    } else {
      auto lower = std::lower_bound(
          m_snapToOffsets.begin(), m_snapToOffsets.end(), currentOffset);
      if (lower == m_snapToOffsets.begin()) {
        nextSnapTarget = 0;
      } else {
        nextSnapTarget = static_cast<float>(*(std::prev(lower, 1)));
      }
    }
  } else if (m_props->snapToInterval > 0) {
    auto interval = m_props->snapToInterval;
    auto intervalIndex = (m_recentScrollFrameOffset > 0)
        ? std::ceil(currentOffset / interval)
        : std::floor(currentOffset / interval);
    nextSnapTarget = static_cast<float>(intervalIndex * interval);
  }
  return nextSnapTarget;
}

ScrollViewComponentInstance::ScrollViewRawProps
ScrollViewComponentInstance::ScrollViewRawProps::getFromDynamic(folly::dynamic value) {
  auto overScrollMode = (value.count("overScrollMode") > 0)
      ? std::optional(value["overScrollMode"].asString())
      : std::nullopt;
  auto nestedEnabled = (value.count("nestedScrollEnabled") > 0)
      ? std::optional(value["nestedScrollEnabled"].asBool())
      : std::nullopt;
  auto endFillColor = (value.count("endFillColor") > 0)
      ? std::optional(value["endFillColor"].asInt())
      : std::nullopt;
  
  return {overScrollMode, nestedEnabled, endFillColor};
}

facebook::react::Point ScrollViewComponentInstance::getContentViewOffset()
    const {
  facebook::react::Point contentViewOffset = {0, 0};
  if (m_props && m_props->centerContent) {
    if (m_contentSize.width < m_containerSize.width) {
      contentViewOffset.x = (m_containerSize.width - m_contentSize.width) / 2;
    }
    if (m_contentSize.height < m_containerSize.height) {
      contentViewOffset.y = (m_containerSize.height - m_contentSize.height) / 2;
    }
  }
  return contentViewOffset;
}

void ScrollViewComponentInstance::adjustVisibleContentPosition(
    facebook::react::ScrollViewMaintainVisibleContentPosition const&
        scrollViewMaintainVisibleContentPosition) {
  if (!m_firstVisibleView.has_value() || m_children.empty() ||
      m_children[0] == nullptr) {
    return;
  }

  auto firstVisibleView = m_firstVisibleView.value();
  ComponentInstance::Shared firstVisibleChild = nullptr;
  for (const auto& child : m_children[0]->getChildren()) {
    auto childComponentInstance =
        std::static_pointer_cast<ComponentInstance>(child);
    if (childComponentInstance->getTag() == firstVisibleView.tag) {
      firstVisibleChild = childComponentInstance;
      break;
    }
  }
  if (firstVisibleChild == nullptr) {
    return;
  }
  auto newPosition = firstVisibleChild->getLayoutMetrics().frame.origin;

  if (isHorizontal(m_props)) {
    auto deltaX = newPosition.x - firstVisibleView.offset;
    if (deltaX != 0) {
      auto scrollX = m_currentOffset.x;
      m_scrollNode.scrollTo(scrollX + deltaX, m_currentOffset.y, false);

      if (scrollViewMaintainVisibleContentPosition.autoscrollToTopThreshold
              .has_value() &&
          scrollX <= scrollViewMaintainVisibleContentPosition
                         .autoscrollToTopThreshold.value()) {
        m_scrollNode.scrollTo(0, m_currentOffset.y, true);
      }
    }
  } else {
    auto deltaY = newPosition.y - firstVisibleView.offset;
    if (deltaY != 0) {
      auto scrollY = m_currentOffset.y;
      m_scrollNode.scrollTo(m_currentOffset.x, scrollY + deltaY, false);

      if (scrollViewMaintainVisibleContentPosition.autoscrollToTopThreshold
              .has_value() &&
          scrollY <= scrollViewMaintainVisibleContentPosition
                         .autoscrollToTopThreshold.value()) {
        m_scrollNode.scrollTo(m_currentOffset.x, 0, true);
      }
    }
  }
}

std::optional<ScrollViewComponentInstance::ChildTagWithOffset>
ScrollViewComponentInstance::getFirstVisibleView(int32_t minIndexForVisible) {
  if (!m_props || m_children.empty() || m_children[0] == nullptr) {
    return std::nullopt;
  }

  auto currentScrollPosition =
      isHorizontal(m_props) ? m_currentOffset.x : m_currentOffset.y;
  auto const& scrollViewChildren = m_children[0]->getChildren();

  minIndexForVisible = std::max(minIndexForVisible, 0);
  for (auto it = scrollViewChildren.begin() + minIndexForVisible;
       it < scrollViewChildren.end();
       it++) {
    auto childComponentInstance =
        std::static_pointer_cast<ComponentInstance>(*it);
    auto position = isHorizontal(m_props)
        ? childComponentInstance->getLayoutMetrics().frame.origin.x
        : childComponentInstance->getLayoutMetrics().frame.origin.y;
    if (position >= currentScrollPosition) {
      return std::optional<ScrollViewComponentInstance::ChildTagWithOffset>(
          {childComponentInstance->getTag(), position});
    }
  }

  auto lastChild =
      std::static_pointer_cast<ComponentInstance>(scrollViewChildren.back());
  auto position = isHorizontal(m_props)
      ? lastChild->getLayoutMetrics().frame.origin.x
      : lastChild->getLayoutMetrics().frame.origin.y;
  return std::optional<ScrollViewComponentInstance::ChildTagWithOffset>(
      {lastChild->getTag(), position});
}

void ScrollViewComponentInstance::onAppear() {
  updateContentClippedSubviews(true);
}

bool ScrollViewComponentInstance::setKeyboardAvoider(
    ComponentInstance::Weak keyboardAvoidingComponentInstance) {
  m_keyboardAvoider = keyboardAvoidingComponentInstance;
  return true;
}

} // namespace rnoh
