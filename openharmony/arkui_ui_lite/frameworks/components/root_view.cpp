/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "components/root_view.h"

#include "common/screen.h"
#include "core/render_manager.h"
#include "draw/draw_utils.h"
#include "gfx_utils/graphic_log.h"
#if defined(ENABLE_WINDOW) && ENABLE_WINDOW
#include "window/window_impl.h"
#endif
#include "securec.h"
namespace OHOS {
namespace {
#if defined(LOCAL_RENDER) && LOCAL_RENDER
const constexpr uint8_t MAX_SPLIT_NUM = 32; // split at most 32 parts
// view along with its parents and siblings are at most 128
const constexpr uint8_t VIEW_STACK_DEPTH = COMPONENT_NESTING_DEPTH * 2;
#else
const constexpr uint8_t VIEW_STACK_DEPTH = COMPONENT_NESTING_DEPTH;
const constexpr uint8_t MAX_INVALIDATE_SIZE = 24;
#endif
static Rect g_maskStack[COMPONENT_NESTING_DEPTH];
static UIView* g_viewStack[VIEW_STACK_DEPTH];
} // namespace
RootView::RootView()
{
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_init(&lock_, nullptr);
#endif
    InitDrawContext();
}

RootView* RootView::GetInstance()
{
    static RootView instance;
    return &instance;
}

RootView::~RootView()
{
    DestroyDrawContext();
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_destroy(&lock_);
#endif
}
#if defined(ENABLE_WINDOW) && ENABLE_WINDOW
Window* RootView::GetBoundWindow() const
{
    return boundWindow_;
}
#endif

Rect RootView::GetScreenRect()
{
#if defined(ENABLE_WINDOW) && ENABLE_WINDOW
    Rect screenRect = GetRect();
#else
    Rect screenRect(0, 0, Screen::GetInstance().GetWidth() - 1, Screen::GetInstance().GetHeight() - 1);
#endif
    return screenRect;
}

#if defined(LOCAL_RENDER) && LOCAL_RENDER
using namespace Graphic;
static void DivideInvalidateRect(const Rect& originRect, Rect& leftoverRect, Vector<Rect>& splitRects)
{
    Rect mask;
    if (!mask.Intersect(originRect, leftoverRect)) {
        splitRects.PushBack(leftoverRect);
        return;
    }

    /*
     *   +---+---+---+
     *   |   | A |   |      originRect           :A+B
     *   |   +---+   |      leftoverRect         :A->0
     *   |     B     |      mask                 :A
     *   +-----------+
     */
    if (originRect.IsContains(leftoverRect)) {
        return;
    }

    int16_t reserveCnt = MAX_SPLIT_NUM - splitRects.Size();
    if (reserveCnt <= 0) {
        splitRects.PushBack(leftoverRect);
        return;
    }

    if (mask.GetWidth() == leftoverRect.GetWidth()) {
        /*
         *       +---+
         *       | A |        originRect           :B+C
         *   +-----------+    leftoverRect         :A+B->A
         *   |   | B |   |    mask                 :B
         *   |   +---+   |
         *   |     C     |
         *   +-----------+
         */
        if (mask.GetBottom() == leftoverRect.GetBottom()) {
            leftoverRect.SetBottom(mask.GetTop() - 1);
        } else if (mask.GetTop() == leftoverRect.GetTop()) {
            leftoverRect.SetTop(mask.GetBottom() + 1);
        } else {
            splitRects.PushBack(leftoverRect);
            splitRects.Back().SetBottom(mask.GetTop() - 1);
            leftoverRect.SetTop(mask.GetBottom() + 1);
        }
        splitRects.PushBack(leftoverRect);
        return;
    }
    if (mask.GetHeight() == leftoverRect.GetHeight()) {
        /*
         *      +---------+   originRect           :B+C
         *  +-------+     |   leftoverRect         :A+B->A
         *  | A | B |  C  |   mask                 :B
         *  +-------+     |
         *      +---------+
         */
        if (mask.GetLeft() == leftoverRect.GetLeft()) {
            leftoverRect.SetLeft(mask.GetRight() + 1);
        } else if (mask.GetRight() == leftoverRect.GetRight()) {
            leftoverRect.SetRight(mask.GetLeft() - 1);
        } else {
            splitRects.PushBack(leftoverRect);
            splitRects.Back().SetRight(mask.GetLeft() - 1);
            leftoverRect.SetLeft(mask.GetRight() + 1);
        }
        splitRects.PushBack(leftoverRect);
        return;
    }

    Vector<Rect> copyRect(splitRects);
    if (mask.GetLeft() != leftoverRect.GetLeft()) {
        /*
         *     |
         * +-------+
         * |   +---+   mask                 :A
         * | B | A |   leftoverRect         :A+B
         * |   +---+
         * +-------+
         *     |
         */
        if (reserveCnt-- <= 0) {
            splitRects.Swap(copyRect);
            splitRects.PushBack(leftoverRect);
            return;
        }
        splitRects.PushBack(leftoverRect);
        splitRects.Back().SetRight(mask.GetLeft() - 1);
        leftoverRect.SetLeft(mask.GetLeft());
    }

    if (mask.GetTop() != leftoverRect.GetTop()) {
        /*
         *  +-------+
         *  |   B   |   mask                 :A
         * ---+---+---  leftoverRect         :A+B
         *  | | A | |
         *  +-+---+-+
         */
        if (reserveCnt-- <= 0) {
            splitRects.Swap(copyRect);
            splitRects.PushBack(leftoverRect);
            return;
        }
        splitRects.PushBack(leftoverRect);
        splitRects.Back().SetBottom(mask.GetTop() - 1);
        leftoverRect.SetTop(mask.GetTop());
    }

    if (mask.GetRight() != leftoverRect.GetRight()) {
        /*
         *     |
         * +-------+
         * +---+   |    mask                 :A
         * | A | B |    leftoverRect         :A+B
         * +---+   |
         * +-------+
         *     |
         */
        if (reserveCnt-- <= 0) {
            splitRects.Swap(copyRect);
            splitRects.PushBack(leftoverRect);
            return;
        }
        splitRects.PushBack(leftoverRect);
        splitRects.Back().SetLeft(mask.GetRight() + 1);
        leftoverRect.SetRight(mask.GetRight());
    }

    if (mask.GetBottom() != leftoverRect.GetBottom()) {
        /*
         *  +-+---+-+
         *  | | A | |     mask                 :A
         * ---+---+---    leftoverRect         :A+B
         *  |   B   |
         *  +-------+
         */
        if (reserveCnt-- <= 0) {
            splitRects.Swap(copyRect);
            splitRects.PushBack(leftoverRect);
            return;
        }
        splitRects.PushBack(leftoverRect);
        splitRects.Back().SetTop(mask.GetBottom() + 1);
        leftoverRect.SetBottom(mask.GetBottom());
    }
    return;
}

static void AddRenderedRects(Rect& rect, List<Rect>& renderedRects, ListNode<Rect>* iter)
{
    /* Elements at front have larger area and more relevance */
    for (; iter != renderedRects.End(); iter = iter->next_) {
        Rect& curRect = iter->data_;
        if (!curRect.IsIntersect(rect)) {
            continue;
        }

        if (curRect.IsContains(rect)) {
            return;
        }

        /* Merge two rects */
        if (rect.IsContains(curRect)) {
        } else if (((curRect.GetLeft() == rect.GetLeft()) && (curRect.GetRight() == rect.GetRight())) ||
                   ((curRect.GetTop() == rect.GetTop()) && (curRect.GetBottom() == rect.GetBottom()))) {
            rect.Join(curRect, rect);
        } else {
            continue;
        }
        iter = renderedRects.Remove(iter)->prev_;
        break;
    }
    if (iter == renderedRects.End()) {     // No merge rises
        if (renderedRects.Size() == 128) { // record 128 rendered rects at most
            renderedRects.PopBack();
        }
        renderedRects.PushFront(rect);
    } else { // merge rises, go over the rest nodes
        AddRenderedRects(rect, renderedRects, iter);
    }
}

void RootView::RemoveViewFromInvalidMap(UIView* view)
{
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_lock(&lock_);
#endif

    int16_t stackCount = 0;
    do {
        while (view != nullptr) {
            /* delete node itself */
            auto entry = invalidateMap_.find(view);
            if (entry != invalidateMap_.end()) {
                invalidateMap_.erase(entry);
            }
            /* delete node's children */
            if (view->IsViewGroup() && stackCount < COMPONENT_NESTING_DEPTH) {
                g_viewStack[stackCount++] = view;
                view = static_cast<UIViewGroup*>(view)->GetChildrenRenderHead();
                continue;
            }
            /* only go to child's render sibling */
            view = view->GetNextRenderSibling();
        }
        if (--stackCount >= 0) {
            view = g_viewStack[stackCount]->GetNextRenderSibling();
        }
    } while (stackCount >= 0);

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_unlock(&lock_);
#endif
}

void RootView::OptimizeInvalidView(UIView* curview, UIView* background, List<Rect>& renderedRects)
{
    if (curview == nullptr) {
        return;
    }
    auto mapEntry = invalidateMap_.find(curview);
    if (mapEntry == invalidateMap_.end()) {
        return;
    }

    Rect& invalidRect = mapEntry->second.Front();
    /* Transparent views should draw from background */
    if (((curview->GetStyleConst().bgOpa_ != OPA_OPAQUE) || (curview->GetOpaScale() != OPA_OPAQUE) ||
         (!curview->IsTransInvalid())) &&
        (curview != this)) {
        AddInvalidateRect(invalidRect, background);
        invalidateMap_.erase(mapEntry);
        return;
    }

    /* Remove the rendered parts and split the origin rect into splitInvalidRects
     * For performance reason, split numbers are strictly restrained.
     */
    Vector<Rect> splitInvalidRects(MAX_SPLIT_NUM << 1);
    Rect invalidRectCopy(invalidRect);
    /* Using forward order because entries at the front are closer to the current view and have larger Size */
    for (auto iter = renderedRects.Begin(); iter != renderedRects.End(); iter = iter->next_) {
        for (int8_t i = 0; i < mapEntry->second.Size(); i++) {
            DivideInvalidateRect(iter->data_, mapEntry->second[i], splitInvalidRects);
        }
        mapEntry->second.Swap(splitInvalidRects);
        splitInvalidRects.Clear();
    }

    /* Add new opaque rects */
    Rect preDrawRect(invalidRectCopy);
    if (!curview->OnPreDraw(preDrawRect)) {
        AddInvalidateRect(invalidRectCopy, background);
    }
    AddRenderedRects(preDrawRect, renderedRects, renderedRects.Begin());
}

void RootView::OptimizeInvalidMap()
{
    UIView* curview = this;
    int16_t stackCount = 0;
    int16_t opaStackCount = 0;
    UIView* background[VIEW_STACK_DEPTH];
    bool flags[VIEW_STACK_DEPTH]; // indicate whether stack go back from child
    List<Rect> renderedRects;     // Record rendered areas to avoid rerendering

    do {
        /* push stack */
        if (curview != nullptr) {
            if (stackCount >= VIEW_STACK_DEPTH) {
                return;
            }
            g_viewStack[stackCount] = curview;
            flags[stackCount++] = false;
            curview = curview->GetNextRenderSibling();
            continue;
        }

        curview = g_viewStack[--stackCount];
        Rect rect;
        if (!curview->IsVisible() || !rect.Intersect(curview->GetRect(), GetScreenRect())) {
            curview = nullptr;
            continue;
        }
        if (!flags[stackCount]) { // Back from sibling
            if (curview->IsViewGroup()) {
                /* Set background/topview */
                if (((curview->GetStyleConst().bgOpa_ == OPA_OPAQUE) && (curview->GetOpaScale() == OPA_OPAQUE) &&
                     curview->IsTransInvalid()) ||
                    (curview == this)) {
                    background[opaStackCount] = curview;
                } else {
                    background[opaStackCount] = background[opaStackCount - 1];
                }
                ++opaStackCount;
                if (opaStackCount >= VIEW_STACK_DEPTH) {
                    return;
                }
                flags[stackCount++] = true;
                curview = static_cast<UIViewGroup*>(curview)->GetChildrenRenderHead();
                continue;
            }
        } else { // Back from child
            opaStackCount--;
        }
        OptimizeInvalidView(curview, background[opaStackCount - 1], renderedRects);
        curview = nullptr;
    } while (stackCount > 0);
    renderedRects.Clear();
}

void RootView::DrawInvalidMap(const Rect& buffRect)
{
    OptimizeInvalidMap();
    Rect rect;
    for (auto& viewEntry : invalidateMap_) {
        Vector<Rect>& viewRenderRect = viewEntry.second;
        for (uint16_t i = 0; i < viewRenderRect.Size(); i++) {
            rect.Intersect(viewRenderRect[i], buffRect);
            DrawTop(viewEntry.first, rect);
        }
    }
}
#else
void RootView::OptimizeAddRect(Rect& rect)
{
    Rect joinRect;
    for (ListNode<Rect>* iter = invalidateRects_.Begin(); iter != invalidateRects_.End(); iter = iter->next_) {
        if (iter->data_.IsContains(rect)) {
            return;
        }

        if (iter->data_.IsIntersect(rect)) {
            joinRect.Join(iter->data_, rect);
            if (joinRect.GetSize() < iter->data_.GetSize() + rect.GetSize()) {
                iter->data_ = joinRect;
                return;
            }
        }
    }

    if (invalidateRects_.Size() < MAX_INVALIDATE_SIZE) {
        invalidateRects_.PushBack(rect);
    } else {
        invalidateRects_.Clear();
        invalidateRects_.PushBack(GetScreenRect());
    }
}

void RootView::OptimizeInvalidateRects()
{
    Rect joinRect;
    for (ListNode<Rect>* iter1 = invalidateRects_.Begin(); iter1 != invalidateRects_.End(); iter1 = iter1->next_) {
        for (ListNode<Rect>* iter2 = invalidateRects_.Begin(); iter2 != invalidateRects_.End(); iter2 = iter2->next_) {
            if (iter1 == iter2) {
                continue;
            }

            if (iter2->data_.IsIntersect(iter1->data_)) {
                joinRect.Join(iter1->data_, iter2->data_);
                if (joinRect.GetSize() < (iter1->data_.GetSize() + iter2->data_.GetSize())) {
                    iter2->data_ = joinRect;
                    iter1 = invalidateRects_.Remove(iter1)->prev_;
                    break;
                }
            }
        }
    }
}
#endif

void RootView::AddInvalidateRect(Rect& rect, UIView* view)
{
    Rect commonRect;
    if (commonRect.Intersect(rect, GetScreenRect())) {
#if defined(LOCAL_RENDER) && LOCAL_RENDER
        Vector<Rect>& invalidRects = invalidateMap_[view];
        if (invalidRects.IsEmpty()) {
            invalidRects.PushBack(commonRect);
        } else {
            invalidRects[0].Join(invalidRects[0], commonRect);
        }
#else
        OptimizeAddRect(commonRect);
#endif
    }
}

void RootView::AddInvalidateRectWithLock(Rect& rect, UIView* view)
{
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_lock(&lock_);
#endif

    AddInvalidateRect(rect, view);

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_unlock(&lock_);
#endif
}

void RootView::Measure()
{
#if defined(LOCAL_RENDER) && LOCAL_RENDER
    if (!invalidateMap_.empty()) {
        MeasureView(GetChildrenRenderHead());
    }
#else
    if (invalidateRects_.Size() > 0) {
        MeasureView(GetChildrenRenderHead());
    }
#endif
}

void RootView::MeasureView(UIView* view)
{
    int16_t stackCount = 0;
    UIView* curView = view;
    while (stackCount >= 0) {
        while (curView != nullptr) {
            if (curView->IsVisible()) {
                curView->ReMeasure();
                if (curView->IsViewGroup() && stackCount < COMPONENT_NESTING_DEPTH) {
                    g_viewStack[stackCount++] = curView;
                    curView = static_cast<UIViewGroup*>(curView)->GetChildrenRenderHead();
                    continue;
                }
            }
            curView = curView->GetNextRenderSibling();
        }
        if (--stackCount >= 0) {
            curView = (g_viewStack[stackCount])->GetNextRenderSibling();
        }
    }
}

void RootView::Render()
{
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_lock(&lock_);
#endif
#if defined(LOCAL_RENDER) && (!LOCAL_RENDER)
    OptimizeInvalidateRects();
#endif

    Rect flushRect(GetScreenRect());
#if defined(LOCAL_RENDER) && LOCAL_RENDER
    if (!invalidateMap_.empty()) {
        RenderManager::RenderRect(flushRect, this);
        invalidateMap_.clear();
#else
    if (invalidateRects_.Size() > 0) {
        /* Fully draw whole reacts. If there are two buffers or more to display, buffers could be
        independent between each other, so buffers need to be FULLY_RENDER. */
#if defined(FULLY_RENDER) && (FULLY_RENDER != 1)
        flushRect = invalidateRects_.Begin()->data_;
        // only draw invalid rects. in this case, buffers (if there are two buffers or more to display) should keep
        // same with each others, because only delta changes write to the buffer between each frames, so it fits one
        // buffer to display.
        for (ListNode<Rect>* iter = invalidateRects_.Begin(); iter != invalidateRects_.End(); iter = iter->next_) {
            RenderManager::RenderRect(iter->data_, this);
            flushRect.Join(flushRect, iter->data_);
        }
#else
        RenderManager::RenderRect(flushRect, this);
#endif
        invalidateRects_.Clear();
#endif

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
        pthread_mutex_unlock(&lock_);
#endif

#if defined(ENABLE_WINDOW) && ENABLE_WINDOW
        if (boundWindow_) {
            boundWindow_->Flush();
            boundWindow_->Update();
        }
#endif
        BaseGfxEngine::GetInstance()->Flush(flushRect);
    } else {
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_unlock(&lock_);
#endif
    }
}

void RootView::BlitMapBuffer(Rect& curViewRect, TransformMap& transMap, const Rect& invalidatedArea)
{
    Rect invalidRect = curViewRect;
    transMap.SetTransMapRect(curViewRect);
    invalidRect.Join(invalidRect, transMap.GetBoxRect());
    if (invalidRect.Intersect(invalidRect, invalidatedArea)) {
        uint8_t pxSize = DrawUtils::GetPxSizeByColorMode(dc_.mapBufferInfo->mode);
        ImageInfo imageInfo;
        imageInfo.header.colorMode = dc_.mapBufferInfo->mode;
        imageInfo.dataSize = dc_.mapBufferInfo->width * dc_.mapBufferInfo->height * (pxSize >> 3);
        imageInfo.header.width = dc_.mapBufferInfo->width;
        imageInfo.header.height = dc_.mapBufferInfo->height;
        imageInfo.header.reserved = 0;
        imageInfo.data = reinterpret_cast<uint8_t*>(dc_.mapBufferInfo->virAddr);
        TransformDataInfo imageTranDataInfo = {imageInfo.header, imageInfo.data, pxSize, LEVEL0, BILINEAR};
        BaseGfxEngine::GetInstance()->DrawTransform(*dc_.bufferInfo, invalidRect, {0, 0}, Color::Black(), OPA_OPAQUE,
                                                    transMap, imageTranDataInfo);
    }
}

void RootView::ClearMapBuffer()
{
    uint8_t pxSize = DrawUtils::GetPxSizeByColorMode(dc_.mapBufferInfo->mode);
    uint32_t dataSize = dc_.mapBufferInfo->width * dc_.mapBufferInfo->height * (pxSize >> 3);
    if (memset_s(dc_.mapBufferInfo->virAddr, dataSize, 0, dataSize) != EOK) {
        GRAPHIC_LOGE("animator buffer memset failed.");
    }
}

void RootView::UpdateMapBufferInfo(Rect& invalidatedArea)
{
    int16_t width = invalidatedArea.GetWidth();
    int16_t height = invalidatedArea.GetHeight();
    invalidatedArea.SetWidth(height);
    invalidatedArea.SetHeight(width);
    dc_.mapBufferInfo->width = height;
    dc_.mapBufferInfo->height = width;
    dc_.mapBufferInfo->stride =
        dc_.mapBufferInfo->width * (DrawUtils::GetPxSizeByColorMode(dc_.mapBufferInfo->mode) >> 3); // 3: Shift 3 bits
}

void RootView::RestoreMapBufferInfo()
{
    dc_.mapBufferInfo->width = dc_.bufferInfo->width;
    dc_.mapBufferInfo->height = dc_.bufferInfo->height;
    dc_.mapBufferInfo->stride =
        dc_.mapBufferInfo->width * (DrawUtils::GetPxSizeByColorMode(dc_.mapBufferInfo->mode) >> 3); // 3: Shift 3 bits
}

bool RootView::CheckNeedRootViewTransView(UIView *curView) {
    if ((curView->GetViewType() == UI_IMAGE_VIEW) && (curView->GetStyle(STYLE_BORDER_WIDTH) == 0)) {
        return false;
    }
    if (curView->GetViewType() == UI_TEXTURE_MAPPER) {
        return false;
    }
    if (curView->IsTransInvalid()) {
        return false;
    }
    return true;
}

void RootView::DrawTop(UIView* view, const Rect& rect)
{
    if (view == nullptr) {
        return;
    }

    int16_t stackCount = 0;
    UIView* par = view->GetParent();
    if (par == nullptr) {
        par = view;
    }
    UIView* curView = view;
    UIView* transViewGroup = nullptr;
    Rect curViewRect;
    Rect mask = rect;
    Rect origRect;
    Rect relativeRect;
    bool enableAnimator = false;
    TransformMap curTransMap;
    bool updateMapBufferInfo = false;

#if defined(ENABLE_WINDOW) && ENABLE_WINDOW
    WindowImpl* boundWin = static_cast<WindowImpl*>(GetBoundWindow());
    BufferInfo* gfxDstBuffer = boundWin->GetBufferInfo();
    UpdateBufferInfo(gfxDstBuffer);
#endif

    while (par != nullptr) {
        if (curView != nullptr) {
            if (curView->IsVisible()) {
                if (curViewRect.Intersect(curView->GetMaskedRect(), mask) || enableAnimator) {
                    if (CheckNeedRootViewTransView(curView) && !enableAnimator) {
                        origRect = curView->GetOrigRect();
                        relativeRect = curView->GetRelativeRect();
                        curView->GetTransformMap().SetInvalid(true);
                        curView->SetPosition(
                            relativeRect.GetX() - origRect.GetX() - curView->GetStyle(STYLE_MARGIN_LEFT),
                            relativeRect.GetY() - origRect.GetY() - curView->GetStyle(STYLE_MARGIN_TOP));

                        ClearMapBuffer();
                        curTransMap = curView->GetTransformMap();
                        enableAnimator = true;
                    }

                    if (enableAnimator) {
                        Rect invalidatedArea;
                        invalidatedArea.SetWidth(dc_.mapBufferInfo->width);
                        invalidatedArea.SetHeight(dc_.mapBufferInfo->height);
                        if (invalidatedArea.GetWidth() < curView->GetWidth()) {
                            UpdateMapBufferInfo(invalidatedArea);
                            updateMapBufferInfo = true;
                        }
                        curView->OnDraw(*dc_.mapBufferInfo, invalidatedArea);
                        curViewRect = invalidatedArea;
                    } else {
                        curView->OnDraw(*dc_.bufferInfo, curViewRect);
                    }

                    if ((curView->IsViewGroup()) && (stackCount < COMPONENT_NESTING_DEPTH)) {
                        if (enableAnimator && (transViewGroup == nullptr)) {
                            transViewGroup = curView;
                        }
                        par = curView;
                        g_viewStack[stackCount] = curView;
                        g_maskStack[stackCount] = mask;
                        stackCount++;
                        curView = static_cast<UIViewGroup*>(curView)->GetChildrenRenderHead();
                        mask = par->GetContentRect();
                        mask.Intersect(mask, curViewRect);
                        continue;
                    }

                    if (enableAnimator) {
                        curView->OnPostDraw(*dc_.mapBufferInfo, curViewRect);
                    } else {
                        curView->OnPostDraw(*dc_.bufferInfo, curViewRect);
                    }

                    if (enableAnimator && (transViewGroup == nullptr)) {
                        BlitMapBuffer(origRect, curTransMap, mask);
                        if (updateMapBufferInfo) {
                            RestoreMapBufferInfo();
                            updateMapBufferInfo = false;
                        }
                        curView->GetTransformMap().SetInvalid(false);
                        enableAnimator = false;
                        curView->SetPosition(relativeRect.GetX() - curView->GetStyle(STYLE_MARGIN_LEFT),
                                             relativeRect.GetY() - curView->GetStyle(STYLE_MARGIN_TOP));
                    }
                }
            }
            curView = curView->GetNextRenderSibling();
            continue;
        }
        if (--stackCount >= 0) {
            curViewRect = par->GetMaskedRect();
            mask = g_maskStack[stackCount];
            if (enableAnimator) {
                par->OnPostDraw(*dc_.mapBufferInfo, curViewRect);
            } else if (curViewRect.Intersect(curViewRect, mask)) {
                par->OnPostDraw(*dc_.bufferInfo, curViewRect);
            }

            if (enableAnimator && transViewGroup == g_viewStack[stackCount]) {
                BlitMapBuffer(origRect, curTransMap, mask);
                if (updateMapBufferInfo) {
                    RestoreMapBufferInfo();
                    updateMapBufferInfo = false;
                }
                transViewGroup->GetTransformMap().SetInvalid(false);
                enableAnimator = false;
                transViewGroup->SetPosition(relativeRect.GetX() - transViewGroup->GetStyle(STYLE_MARGIN_LEFT),
                                            relativeRect.GetY() - transViewGroup->GetStyle(STYLE_MARGIN_TOP));
                transViewGroup = nullptr;
            }
            curView = g_viewStack[stackCount]->GetNextRenderSibling();
            par = par->GetParent();
            continue;
        }
        stackCount = 0;
        curView = par->GetNextRenderSibling();
        if (enableAnimator) {
            par->OnPostDraw(*dc_.mapBufferInfo, rect);
        } else {
            par->OnPostDraw(*dc_.bufferInfo, rect);
        }
        par = par->GetParent();
    }
}

UIView* RootView::GetTopUIView(const Rect& rect)
{
    int16_t stackCount = 0;
    UIView* currentView = this;
    UIView* topView = currentView;
    Rect copyRect(rect);
    while (stackCount >= 0) {
        while (currentView != nullptr) {
            if (currentView->GetOrigRect().IsContains(rect) && currentView->IsVisible()) {
                if (currentView->GetStyle(STYLE_BACKGROUND_OPA) == OPA_OPAQUE && currentView->OnPreDraw(copyRect) &&
                    currentView->GetOpaScale() == OPA_OPAQUE) {
                    topView = currentView;
                }
                if (currentView->IsViewGroup() && stackCount < COMPONENT_NESTING_DEPTH) {
                    g_viewStack[stackCount++] = currentView;
                    currentView = static_cast<UIViewGroup*>(currentView)->GetChildrenRenderHead();
                    continue;
                }
            }
            currentView = currentView->GetNextRenderSibling();
        }
        if (--stackCount >= 0) {
            currentView = (g_viewStack[stackCount])->GetNextRenderSibling();
        }
    }
    UIView* parentView = topView;
    while (parentView->GetParent() != nullptr) {
        UIView* tempView = parentView;
        parentView = parentView->GetParent();
        if (!tempView->IsTransInvalid()) {
            topView = parentView;
        }
    }
    return topView;
}

bool RootView::FindSubView(const UIView& parentView, const UIView* subView)
{
    const UIView* root = &parentView;
    if (root == subView) {
        return true;
    } else if (!root->IsViewGroup() || (subView == nullptr)) {
        return false;
    }

    UIView* currentView = static_cast<const UIViewGroup*>(root)->GetChildrenRenderHead();
    const UIView* parent = root;
    int8_t deep = 1;
    while (deep > 0) {
        if (parent == nullptr) {
            return false;
        }
        if (currentView == subView) {
            return true;
        }
        if (currentView == nullptr) {
            currentView = parent->GetNextRenderSibling();
            parent = parent->GetParent();
            deep--;
        } else if (currentView->IsViewGroup()) {
            parent = currentView;
            currentView = static_cast<UIViewGroup*>(currentView)->GetChildrenRenderHead();
            deep++;
        } else {
            currentView = currentView->GetNextRenderSibling();
        }
    }
    return false;
}

void RootView::InitMapBufferInfo(BufferInfo* bufferInfo)
{
    dc_.mapBufferInfo = new BufferInfo();
    if (dc_.mapBufferInfo == nullptr) {
        return;
    }

    if (memcpy_s(dc_.mapBufferInfo, sizeof(BufferInfo), bufferInfo, sizeof(BufferInfo)) != EOK) {
        delete dc_.mapBufferInfo;
        dc_.mapBufferInfo = nullptr;
        return;
    }
    dc_.mapBufferInfo->mode = ARGB8888;
    dc_.mapBufferInfo->stride = dc_.mapBufferInfo->width * (DrawUtils::GetPxSizeByColorMode(dc_.mapBufferInfo->mode) >>
                                                            3); // 3: Shift right 3 bits
    
    BaseGfxEngine* baseGfxEngine = BaseGfxEngine::GetInstance();
    baseGfxEngine->AdjustLineStride(*dc_.mapBufferInfo);
    uint32_t bufferSize = dc_.mapBufferInfo->stride * dc_.mapBufferInfo->height;
    dc_.mapBufferInfo->virAddr = dc_.mapBufferInfo->phyAddr =
        baseGfxEngine->AllocBuffer(bufferSize, BUFFER_MAP_SURFACE);
}

void RootView::DestroyMapBufferInfo()
{
    if (dc_.mapBufferInfo != nullptr) {
        BaseGfxEngine::GetInstance()->FreeBuffer(static_cast<uint8_t*>(dc_.mapBufferInfo->virAddr), BUFFER_MAP_SURFACE);
        dc_.mapBufferInfo->virAddr = dc_.mapBufferInfo->phyAddr = nullptr;
        delete dc_.mapBufferInfo;
        dc_.mapBufferInfo = nullptr;
    }
}

void RootView::InitDrawContext()
{
    dc_.bufferInfo = BaseGfxEngine::GetInstance()->GetFBBufferInfo();
    if (dc_.bufferInfo != nullptr) {
        InitMapBufferInfo(dc_.bufferInfo);
    }

    bakDc_.bufferInfo = nullptr;
    bakDc_.mapBufferInfo = nullptr;
}

void RootView::DestroyDrawContext()
{
    DestroyMapBufferInfo();
}

void RootView::UpdateBufferInfo(BufferInfo* fbBufferInfo)
{
    if (dc_.bufferInfo == nullptr) {
        dc_.bufferInfo = fbBufferInfo;
        InitMapBufferInfo(fbBufferInfo);
    } else {
        if (dc_.bufferInfo->width != fbBufferInfo->width || dc_.bufferInfo->height != fbBufferInfo->height ||
            dc_.bufferInfo->mode != fbBufferInfo->mode) {
            DestroyMapBufferInfo();
            InitMapBufferInfo(fbBufferInfo);
        }
        dc_.bufferInfo = fbBufferInfo;
    }
}

void RootView::SaveDrawContext()
{
    bakDc_.bufferInfo = dc_.bufferInfo;
    dc_.bufferInfo = nullptr;

    bakDc_.mapBufferInfo = dc_.mapBufferInfo;
    dc_.mapBufferInfo = nullptr;
}

void RootView::RestoreDrawContext()
{
    DestroyDrawContext();

    dc_.bufferInfo = bakDc_.bufferInfo;
    bakDc_.bufferInfo = nullptr;

    dc_.mapBufferInfo = bakDc_.mapBufferInfo;
    bakDc_.mapBufferInfo = nullptr;
}
} // namespace OHOS
