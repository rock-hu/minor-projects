/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { ViewDescriptorWrapperBase } from '.';
import { Descriptor, RawPropsBase } from '../../RNOH';
import { RNOHContext } from '../../RNOH';

export interface PullToRefreshProps {}

export interface PullToRefreshRawProps extends RawPropsBase {
  refreshing?: boolean
  progressViewOffset?: number
  progressBackgroundColor?: string
  tintColor?: string
  title?: string
  titleColor?: string
}

export interface PullToRefreshState {}

export interface RefreshLoadingProgressBuilderParams {
  progressOffsetView?: number
  progressBackgroundColor?: ResourceColor
  tintColor?: ResourceColor
  title?: string
  titleColor?: ResourceColor
}

export const RN_PULL_TO_REFRESH_VIEW_NAME: string = "PullToRefreshView"

export type PullToRefreshDescriptor = Descriptor<"PullToRefreshView", PullToRefreshProps, PullToRefreshState, PullToRefreshRawProps>

@Component
export struct RNPullToRefreshView {
  ctx!: RNOHContext
  tag: number = 0
  @BuilderParam renderDescriptors: () => void
  @State private descriptor: PullToRefreshDescriptor = Object() as PullToRefreshDescriptor
  @State descriptorWrapper: ViewDescriptorWrapperBase | undefined = undefined
  private cleanUpCallbacks: (() => void)[] = []
  @State private isRefreshing: boolean = false // ignore linter, this variable is read
  private prevAndCurrentRefreshStatus: [RefreshStatus, RefreshStatus] = [RefreshStatus.Inactive, RefreshStatus.Inactive]

  aboutToAppear() {
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<PullToRefreshDescriptor>(this.tag)
    this.cleanUpCallbacks.push(this.ctx.componentCommandReceiver.registerCommandCallback(
      this.tag,
      (command, args: [boolean]) => {
        if (command === 'setNativeRefreshing') {
          this.isRefreshing = args[0]
        }
      }));
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.onDescriptorChange(newDescriptor as PullToRefreshDescriptor)
      }
    ));
  }

  aboutToDisappear() {
    this.cleanUpCallbacks.forEach(cleanUp => cleanUp())
  }

  onDescriptorChange(descriptor: PullToRefreshDescriptor) {
    this.descriptor = descriptor
    this.isRefreshing = !!this.descriptor.rawProps.refreshing
  }

  handleRefresh() {
    /**
     * We set this here to true and leave the responsibility of controlling it to the react-native side. If the developer
     * doesn't set the refreshing prop of RefreshControl in react-native to true, then this will be turned off when the
     * descriptor updates.
     */
    this.isRefreshing = true;

    // Emit this event only when a user initiated pull to refresh, not when RN changed `refreshing` prop.
    if ([RefreshStatus.Drag, RefreshStatus.OverDrag].includes(this.prevAndCurrentRefreshStatus[0])) {

      this.ctx.rnInstance.emitComponentEvent(
        this.descriptor.tag,
        "refresh",
        {}
      )
    }
  }

  @Builder
  RefreshLoadingProgress(params: RefreshLoadingProgressBuilderParams) {
    Stack() {
      Stack() {
        LoadingProgress()
          .width('112px')
          .height('112px')
          .offset({x: 0, y: -(params.progressOffsetView ?? 0)})
          .color(params.tintColor)
      }
      .backgroundColor(params.progressBackgroundColor ?? Color.Transparent)
      .borderRadius(1000)
    }
    Text(params.title)
      .fontColor(params.titleColor)
      .margin({ top: '224px' })
  }

  build() {
    Refresh({
      refreshing: this.isRefreshing,
      builder: this.RefreshLoadingProgress({
        progressOffsetView: this.descriptor.rawProps.progressViewOffset,
        progressBackgroundColor: this.descriptor.rawProps.progressBackgroundColor,
        tintColor: this.descriptor.rawProps.tintColor,
        title: this.descriptor.rawProps.title,
        titleColor: this.descriptor.rawProps.titleColor
      }),
    }) {
      this.renderDescriptors();
    }
    .backgroundColor(Color.Transparent)
    .onStateChange((refreshStatus) => {
      this.prevAndCurrentRefreshStatus = [this.prevAndCurrentRefreshStatus[1], refreshStatus]

      // React Native `refreshing` prop should be the only source of truth
      const isRefreshing = !!this.descriptor.rawProps.refreshing;
      if (refreshStatus === RefreshStatus.Refresh && !isRefreshing && this.isRefreshing) {
        this.isRefreshing = isRefreshing;
      }
    })
    .onRefreshing(() => this.handleRefresh())
  }
}
