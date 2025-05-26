/**
 * 最佳实践：合理使用音频资源
 * 场景三：播音场景（OpenSL ES）
 */

// [Start open_sl_es]
//The foreground scene starts to play
SLPlayItfplayItf=nullptr;
(*pcmPlayerObject)->GetInterface(pcmPlayerObject,SL_IID_PLAY,&playItf);
(*playItf)->SetPlayState(playItf,SL_PLAYSTATE_PLAYING);
// Stop playing the background scene
(*playItf)->SetPlayState(playItf,SL_PLAYSTATE_STOPPED);
(*pcmPlayerObject)->Destroy(pcmPlayerObject);
(*engineObject)->Destroy(engineObject);
// [End open_sl_es]

/**
 * 最佳实践：合理使用音频资源
 * 场景四：播音场景（OHAudio）
 */

// [Start oh_audio]
//Construct the audio stream to play
OH_AudioRenderer*audioRenderer;
ret=OH_AudioStreamBuilder_GenerateRenderer(builder,&audioRenderer);

//The foreground scene starts to play
ret=OH_AudioRenderer_Start(audioRenderer);
// Stop playing the background scene
ret=OH_AudioRenderer_Stop(audioRenderer);
// [End oh_audio]