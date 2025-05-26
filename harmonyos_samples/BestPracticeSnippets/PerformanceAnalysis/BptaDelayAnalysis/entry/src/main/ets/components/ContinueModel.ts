// src/model/ContinueModel.ts
import { AudioPlayerStatus } from "./AudioPlayerStatus";

export class ContinueModel {
  private static instance: ContinueModel;
  public data: AppData;

  // 私有构造方法
  private constructor() {
    this.data = {
      mainTabIndex: 0,
      audioPlaybackInfo: {
        status: AudioPlayerStatus.IDLE,
        currentTrackId: '',
        progress: 0
      },
      userPreferences: {
        darkMode: false,
        playbackSpeed: 1.0
      }
    };

    // 从持久化存储初始化
    this.initFromStorage();
  }

  // 获取单例实例
  public static getInstance(): ContinueModel {
    if (!ContinueModel.instance) {
      ContinueModel.instance = new ContinueModel();
    }
    return ContinueModel.instance;
  }

  // 从AppStorage初始化数据
  private initFromStorage(): void {
    try {
      const savedTabIndex = AppStorage.get('mainTabIndex');
      if (savedTabIndex !== undefined) {
        this.data.mainTabIndex = Number(savedTabIndex);
      }

      const darkMode = AppStorage.get('darkMode');
      if (darkMode !== undefined) {
        this.data.userPreferences.darkMode = Boolean(darkMode);
      }
    } catch (error) {
      console.error('Failed to init from storage:', error);
    }
  }

  // 更新当前Tab索引
  public updateTabIndex(index: number): void {
    this.data.mainTabIndex = index;
    AppStorage.setOrCreate('mainTabIndex', index);
  }

  // 更新音频播放状态
  public updateAudioStatus(status: AudioPlayerStatus, trackId?: string): void {
    this.data.audioPlaybackInfo.status = status;
    if (trackId) {
      this.data.audioPlaybackInfo.currentTrackId = trackId;
    }
    AppStorage.setOrCreate('audioPlayerStatus', status);
  }

  // 切换主题模式
  public toggleDarkMode(): void {
    this.data.userPreferences.darkMode = !this.data.userPreferences.darkMode;
    AppStorage.setOrCreate('darkMode', this.data.userPreferences.darkMode);
  }
}

// 应用数据类型定义
interface AppData {
  mainTabIndex: number;
  audioPlaybackInfo: {
    status: AudioPlayerStatus;
    currentTrackId: string;
    progress: number;
  };
  userPreferences: {
    darkMode: boolean;
    playbackSpeed: number;
  };
}