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

import { camera } from '@kit.CameraKit';
import Logger from './Logger';
import { CameraDataModel } from '../../model/CameraDateModel';
import { CommonConstants as Const } from '../CommonConstants';

const TAG = 'CAMERA_CHECK';

function getPreviewProfile(previewProfiles: Array<camera.Profile>, size: camera.Size, isHDRVivid: number): undefined | camera.Profile {
  let previewProfile: undefined | camera.Profile = previewProfiles.find((profile: camera.Profile) => {
    if(isHDRVivid) {
      return profile.format === 2001 &&
        profile.size.width === 1920 && profile.size.height == 1080
    } else {
      return profile.format === camera.CameraFormat.CAMERA_FORMAT_YUV_420_SP &&
        profile.size.width === size.width && profile.size.height == size.height
    }
  });
  return previewProfile;
}

export function previewProfileCameraCheck(cameraManager: camera.CameraManager,
  cameraData: CameraDataModel): undefined | camera.Profile {
  let cameraDevices = cameraManager.getSupportedCameras();
  if (cameraDevices !== undefined && cameraDevices.length <= 0) {
    Logger.error(TAG, 'cameraManager.getSupportedCameras error!');
    return;
  }

  let profiles: camera.CameraOutputCapability =
    cameraManager.getSupportedOutputCapability(cameraDevices[0], camera.SceneMode.NORMAL_VIDEO);
  if (!profiles) {
    Logger.error(TAG, 'cameraManager.getSupportedOutputCapability error!');
    return;
  }

  let previewProfilesArray: Array<camera.Profile> = profiles.previewProfiles;
  if (!previewProfilesArray) {
    Logger.error("createOutput previewProfilesArray == null || undefined");
    return;
  }

  let videoSize: camera.Size = {
    width: 1920,
    height: 1080
  }
  let previewProfile: undefined | camera.Profile = getPreviewProfile(previewProfilesArray, videoSize, cameraData.isHDRVivid);
  if (!previewProfile) {
    Logger.error('previewProfile is not found');
    return;
  }
  return previewProfile;
}

// [Start create_video_output2]
export function videoProfileCheck(cameraManager: camera.CameraManager,
  cameraData: CameraDataModel): undefined | camera.VideoProfile {
  let cameraDevices = cameraManager.getSupportedCameras();
  // [StartExclude create_video_output2]
  if (cameraDevices !== undefined && cameraDevices.length <= 0) {
    Logger.error(TAG, 'cameraManager.getSupportedCameras error!');
    return;
  }
  // [EndExclude create_video_output2]

  let profiles: camera.CameraOutputCapability =
    cameraManager.getSupportedOutputCapability(cameraDevices[0], camera.SceneMode.NORMAL_VIDEO);
  // [StartExclude create_video_output2]
  if (!profiles) {
    Logger.error(TAG, 'cameraManager.getSupportedOutputCapability error!');
    return;
  }
  // [EndExclude create_video_output2]

  let videoProfiles: Array<camera.VideoProfile> = profiles.videoProfiles;
  // [StartExclude create_video_output2]
  if (!videoProfiles) {
    Logger.error(TAG, 'Get videoProfiles error!');
    return;
  }
  // [EndExclude create_video_output2]

  let videoProfile: undefined | camera.VideoProfile = videoProfiles.find((profile: camera.VideoProfile) => {
    if (cameraData.isHDRVivid) {
      // [StartExclude create_video_output2]
      if (cameraData.frameRate === Const.FRAMERATE_VIDEO_30FPS) {
        // [EndExclude create_video_output2]
        return profile.size.width === cameraData.cameraWidth &&
          profile.size.height === cameraData.cameraHeight &&
          profile.format === camera.CameraFormat.CAMERA_FORMAT_YCRCB_P010 &&
          profile.frameRateRange.min === 1 &&
          profile.frameRateRange.max === 30;
        // [StartExclude create_video_output2]
      } else {
        return profile.size.width === cameraData.cameraWidth &&
          profile.size.height === cameraData.cameraHeight &&
          profile.format === camera.CameraFormat.CAMERA_FORMAT_YCRCB_P010 &&
          profile.frameRateRange.min === cameraData.frameRate &&
          profile.frameRateRange.max === cameraData.frameRate;
      }
      // [EndExclude create_video_output2]
    } else {
      // [StartExclude create_video_output2]
      if (cameraData.frameRate === Const.FRAMERATE_VIDEO_30FPS) {
        return profile.size.width === cameraData.cameraWidth &&
          profile.size.height === cameraData.cameraHeight &&
          profile.format === camera.CameraFormat.CAMERA_FORMAT_YUV_420_SP &&
          profile.frameRateRange.min === 1 &&
          profile.frameRateRange.max === 30;
      } else {
        return profile.size.width === cameraData.cameraWidth &&
          profile.size.height === cameraData.cameraHeight &&
          profile.format === camera.CameraFormat.CAMERA_FORMAT_YUV_420_SP &&
          profile.frameRateRange.min === cameraData.frameRate &&
          profile.frameRateRange.max === cameraData.frameRate;
      }
      // [EndExclude create_video_output2]
    }
  });
  return videoProfile;
}
// [End create_video_output2]