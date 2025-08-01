# 共享音频输入

音频输入的音频源通常是由内置麦克风、外接设备或远端分布式设备的采集的音频数据，但也可能是其他应用发送到系统以供播放的音频数据。根据音频源的不同，音频输入可分为两类：内录型音频输入和外录型音频输入。

- 内录型音频输入：是指以系统内部音频数据作为音频源的输入类型，简称为内录，对应的流称为内录流。

  常见的内录音频包括录屏时录制的其他应用播放的音频数据、投播到其他设备上播放的音频数据。

- 外录型音频输入：是指通过音频接口将系统外部的声音采集进来作为音频源的输入类型，简称为外录，对应的流称为外录流。

  常见的外录示例有录音机的音频录制、语音助手的语音唤醒和识别、以及在VOIP通话中传输给对端的音频录制。

通常，当应用请求录音时，它是独占音频输入的。然而，在某些情况下，可能会有两个或多个应用同时或先后请求录音，在同一时间段内都希望从音频输入中获得数据，这种情况称为录音并发，录音并发又细分为：录音并存和录音并行。

- 录音并存是指一个或多个应用创建的不同录音客户端（AudioCapturer）在同一时间段内同时存在的状态。
- 录音并行是指在录音并存的基础上，不同录音客户端（AudioCapturer）启动的录音流在同一时间段内同时处于运行（Running）状态，即同时进行音频录制。

当录音并发发生时，每个请求录音的应用都希望能够获得音频输入数据，但此时会面临将同一音频源同时传输给多个应用的问题。

## 录音并发

此前，系统不支持不同应用程序间不同录音流类型的录音并发，会受到严格的[音频焦点策略管控](audio-playback-concurrency.md#音频焦点策略)。

内录型与外录型音频输入之间的并发录音在安全隐私策略允许下不受此限制，例如在录屏应用中启用内录流时，可以同时使用录音机录音或语音助手进行语音识别。

当前系统版本虽已放宽了部分录音流类型之间的并行录音限制，允许多个录音流同时获取音频数据，但由于这些数据来源于同一音频输入，音频效果相同，可能只能满足部分并行录音需求。

### 优先级调控方案

大多数情况下，音频数据的效果依赖于系统对音频输入数据的优化处理策略。

应用发起录音时，系统会依据应用下发的录音流类型等相关参数识别音频场景，并选择合适的处理策略处理输入数据。例如，当应用发起VOIP通话时，系统会对VOIP录音流进行降噪、人声增强等优化处理。

一些录音流类型的音频录制仅需获取音频输入数据即可，但另一些录音流类型的音频录制则高度依赖于系统的优化处理，不当的处理可能会导致不良体验。因此，对于这类录音流类型的音频录制，在并发录音时需确保系统仍能配置适当的优化处理策略。为此，系统为这些录音流类型配置了优先级，并在原有的[音频焦点策略](audio-playback-concurrency.md#音频焦点策略)管控方案上增加了优先级调控方案。

其调控原则是**优先采用高优先级录音流类型对应的优化策略处理音频输入数据**。

### 录音并发策略

当前录音并发策略整体遵循如下原则：

1. 在并发录音时，是否能同时获取音频输入数据取决于各录音流类型的[音频焦点策略](audio-playback-concurrency.md#音频焦点策略)，开发人员需确保焦点适配良好。
2. 音频输入数据的效果将根据系统依据当前优先级调控方案选择的优化处理策略来决定，建议开发人员在并发录音时告知用户录音数据的效果可能受到影响。

## 使用建议

当前系统通过[音频焦点策略](audio-playback-concurrency.md#音频焦点策略)与优先级调控的双重机制，针对录音并发场景做了初步管理。尽管系统已经允许个别不同录音流类型的流实现有限并行录音，但仍需注意以下关键点：

### 录音并发的局限性

- 音频效果不可控：并发录音时，多条录音流的音频输入数据来自同一音频输入，可能影响音频效果。
- 系统开销：并发录音可能增加系统开销。

### 对开发者的建议

- 了解[音频焦点策略](audio-playback-concurrency.md#音频焦点策略)，做好焦点适配，及时[处理焦点变化](audio-playback-concurrency.md#处理音频焦点变化)。
- 尽量避免并发录音场景，在应用设计时尽量避免与其他录音任务重叠。如需要，可以在界面中提示用户“当前存在其他录音任务”。

  如果必须进行并发录音，建议在界面上明确告知用户音频质量可能会受到影响。
- [选择合适的录音流类型](using-right-streamusage-and-sourcetype.md)，不同的录音流类型对应着不同的系统优化处理策略，建议开发者根据需求选择合适的[录音流类型](../../reference/apis-audio-kit/arkts-apis-audio-e.md#sourcetype8)。
- 在没有录音需求时，应及时停止并释放录音资源，避免对其他录音流产生影响，并减少不必要的系统开销。
- 应用实现不应过度依赖固定的录音并发规则，而应根据系统接口返回的状态进行自适应处理。
