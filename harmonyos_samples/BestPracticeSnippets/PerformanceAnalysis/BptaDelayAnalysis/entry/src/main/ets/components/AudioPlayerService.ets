/*
* Copyright (C) 2024 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/*
* 最佳实践: 点击完成时延分析
*/

// [Start audio_player_service]
// commons\audioplayer\src\main\ets\service\AudioPlayerService.ets

export class AudioPlayerService {
  private static instance: AudioPlayerService | null = null;

  // [StartExclude audio_player_service]
  // private constructor
  private constructor() {
    this.initAudioPlayer();
  }
  // [EndExclude audio_player_service]

  public static getInstance(): AudioPlayerService {
    if (!AudioPlayerService.instance) {
      AudioPlayerService.instance = new AudioPlayerService();
    }
    return AudioPlayerService.instance;
  }

  public static destroy(): void {
    if (AudioPlayerService.instanceIsNotNull()) {
      AudioPlayerService.getInstance().releaseAudioPlayer();
      AudioPlayerService.instance = null;
    }
  }

  // [StartExclude audio_player_service]
  public static instanceIsNotNull(): boolean {
    return AudioPlayerService.instance !== null;
  }

  // [StartExclude audio_player_service]
  private initAudioPlayer(): void {
  }

  // Release audio resources
  public releaseAudioPlayer(): Promise<void> {
    return new Promise((resolve, reject) => {
      resolve();
    });
  }

  public stop(): Promise<void> {
    return new Promise((resolve, reject) => {
      resolve();
    });
  }
  // [EndExclude audio_player_service]
  // [EndExclude audio_player_service]
}
// [End audio_player_service]