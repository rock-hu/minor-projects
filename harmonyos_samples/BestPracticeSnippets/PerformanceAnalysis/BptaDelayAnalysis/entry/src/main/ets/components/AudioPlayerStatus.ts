// src/components/AudioPlayerStatus.ts
/**
 * 音频播放器状态枚举
 */
export enum AudioPlayerStatus {
  /** 初始空闲状态 */
  IDLE = 'idle',

  /** 正在准备中 */
  PREPARING = 'preparing',

  /** 准备就绪（可播放） */
  READY = 'ready',

  /** 正在播放 */
  PLAYING = 'playing',

  /** 已暂停 */
  PAUSED = 'paused',

  /** 播放完成 */
  COMPLETED = 'completed',

  /** 发生错误 */
  ERROR = 'error',

  /** 正在缓冲 */
  BUFFERING = 'buffering',

  /** 正在跳转进度 */
  SEEKING = 'seeking'
}

// 可选：状态检查工具函数
export class AudioStatusUtils {
  /**
   * 检查是否正在播放状态
   */
  static isPlaying(status: AudioPlayerStatus): boolean {
    return status === AudioPlayerStatus.PLAYING;
  }

  /**
   * 检查是否可恢复播放
   */
  static canResume(status: AudioPlayerStatus): boolean {
    return [
      AudioPlayerStatus.PAUSED,
      AudioPlayerStatus.READY,
      AudioPlayerStatus.COMPLETED
    ].includes(status);
  }

  /**
   * 检查是否处于活动状态（非空闲/错误）
   */
  static isActive(status: AudioPlayerStatus): boolean {
    return ![
      AudioPlayerStatus.IDLE,
      AudioPlayerStatus.ERROR
    ].includes(status);
  }
}