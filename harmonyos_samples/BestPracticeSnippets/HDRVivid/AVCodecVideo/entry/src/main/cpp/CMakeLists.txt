# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(videoCodecSample)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/capbilities/include
                    ${NATIVERENDER_ROOT_PATH}/common
                    ${NATIVERENDER_ROOT_PATH}/common/dfx/err
                    ${NATIVERENDER_ROOT_PATH}/common/dfx/log
                    ${NATIVERENDER_ROOT_PATH}/render/include
                    ${NATIVERENDER_ROOT_PATH}/sample/player
                    ${NATIVERENDER_ROOT_PATH}/sample/recorder
)

set(BASE_LIBRARY
    libace_napi.z.so libEGL.so libGLESv3.so libace_ndk.z.so libuv.so libhilog_ndk.z.so
    libnative_media_codecbase.so libnative_media_core.so libnative_media_vdec.so libnative_window.so
    libnative_media_venc.so libnative_media_acodec.so libnative_media_avdemuxer.so libnative_media_avsource.so libnative_media_avmuxer.so
    libohaudio.so
)
add_library(player SHARED sample/player/PlayerNative.cpp
                          sample/player/Player.cpp
                          capbilities/Demuxer.cpp
                          capbilities/VideoDecoder.cpp
                          capbilities/AudioDecoder.cpp
                          render/EglCore.cpp
                          render/PluginRender.cpp
                          render/PluginManager.cpp
                          common/SampleCallback.cpp
)

add_library(recorder SHARED sample/recorder/RecorderNative.cpp
                            sample/recorder/Recorder.cpp
                            capbilities/Muxer.cpp
                            capbilities/VideoEncoder.cpp
                            common/SampleCallback.cpp
)

target_link_libraries(player PUBLIC ${BASE_LIBRARY})
target_link_libraries(recorder PUBLIC ${BASE_LIBRARY})