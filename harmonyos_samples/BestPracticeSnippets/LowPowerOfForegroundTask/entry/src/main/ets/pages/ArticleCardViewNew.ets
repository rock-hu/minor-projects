// [Start article_card_view_new]
import { router } from '@kit.ArkUI';
import common from '@ohos.app.ability.common';
import { LearningResource, ActionButtonView, CommonConstants, ArticleCardButtonView } from '@ohos/common';
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
          // Determine whether it is a power-saving mode
          if (power.getPowerMode() == power.DevicePowerMode.MODE_EXTREME_POWER_SAVE) {
            // Set up likes and favorite pictures
            ActionButtonView({
              imgResource: this.isLiked ? $r('app.media.btn_good_on') : $r('app.media.btn_good_normal'),
              count: this.articleItem.likesCount,
              textWidth: $r('app.float.like_icon_width')
            })
              .onClick(() => {
                this.onLiked?.();
              })

            ActionButtonView({
              imgResource: this.isCollected ? $r('app.media.btn_favorites_on') : $r('app.media.btn_favorites_normal'),
              count: this.articleItem.collectionCount,
              textWidth: $r('app.float.star_icon_width')
            })
              .onClick(() => {
                this.onCollected?.()
              })
          } else {
            // Set the lottie animation of likes and collections
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

            ArticleCardButtonView({
              clickAnimationPath: 'common/lottie/collected_lottie.json',
              cancelAnimationPath: 'common/lottie/cancel_collect_lottie.json',
              isClicked: this.isCollected,
              count: this.articleItem.collectionCount,
              articleId: this.articleItem.id,
              textWidth: $r('app.float.star_icon_width'),
              type: 'collect',
              onClicked: this.onCollected,
              normalImage: $r('app.media.btn_collect_normal'),
              onImage: $r('app.media.btn_collect_on')
            })
          }
        }
        .width(CommonConstants.FULL_PERCENT)
        .justifyContent(FlexAlign.SpaceBetween)
      }
      .layoutWeight(1)
      .height(CommonConstants.FULL_PERCENT)
      .justifyContent(FlexAlign.SpaceAround)
      // ...
    }
    // ...
  }
}
// [End article_card_view_new]