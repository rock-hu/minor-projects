// [Start article_card_view]
import { router } from '@kit.ArkUI';
import { common } from '@kit.AbilityKit';
import { LearningResource, ActionButtonView, CommonConstants, ArticleCardButtonView } from '@ohos/common';
// Introduction of power module
import { power } from '@kit.BasicServicesKit';

@Component
export struct ArticleCardView {
  @Prop isLiked: boolean = false;
  @Prop isCollected: boolean = false;
  @ObjectLink articleItem: LearningResource;
  onCollected?: () => void;
  onLiked?: () => void;

  build() {
    Row({ space: CommonConstants.SPACE_16 }) {
      Column() {
        // ...
        Row() {
          ActionButtonView({
            imgResource: $r('app.media.ic_eye_open'),
            count: this.articleItem.viewsCount,
            textWidth: $r('app.float.view_count_icon_width')
          })
          // Use getPowerMode to get the power mode of the current system and determine whether it is currently a power-saving mode.
          if (power.getPowerMode() == power.DevicePowerMode.MODE_POWER_SAVE) {
            ActionButtonView({
              imgResource: this.isLiked ? $r('app.media.btn_good_on') : $r('app.media.btn_good_normal'),
              count: this.articleItem.likesCount,
              textWidth: $r('app.float.like_icon_width')
            })
              .onClick(() => {
                this.onLiked?.();
              })
            // ...
          } else {
            ArticleCardButtonView({
              clickAnimationPath: 'common/lottie/liked_lottie.json',
              cancelAnimationPath: 'common/lottie/cancel_liked_lottie.json',
              isClicked: this.isLiked,
              count: this.articleItem.likesCount,
              articleId: this.articleItem.id,
              textWidth: $r('app.float.like_icon_width'),
              type: 'like',
              onClicked: this.onLiked,
              normalImage: $r('app.media.btn_like_normal'),
              onImage: $r('app.media.btn_like_on')
            })
            // ...
          }
        }
        .width(CommonConstants.FULL_PERCENT)
        .justifyContent(FlexAlign.SpaceBetween)
      }
      .layoutWeight(1)
      .height(CommonConstants.FULL_PERCENT)
      .justifyContent(FlexAlign.SpaceAround)
    }
  }
}
// [End article_card_view]