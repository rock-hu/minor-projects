/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
export const BuildProfile = {
  apiType: "stageMode",
  targets: [
    {
      name: "default",
      runtimeOS: "HarmonyOS",
    }
  ]
};

export const BuildOptionSetOfRelease = [
  {
    name: "release",
    externalNativeOptions: {
      path: "./src/main/cpp/CMakeLists.txt",
      arguments: "",
      cppFlags: ""
    },
    nativeLib: {
      headerPath: ["./src/main/cpp/include"],
      librariesInfo: [
        {
          name: "librnoh.so",
          linkLibraries: [
            "libace_napi.z.so",
            "libace_ndk.z.so",
            "librawfile.z.so",
            "libhilog_ndk.z.so",
            "libnative_vsync.so",
            "libnative_drawing.so",
            "libhitrace_ndk.z.so",
            "libqos.so",
            "react-native-openharmony::folly_runtime",
            "react-native-openharmony::glog",
            "react-native-openharmony::jsi",
            "react-native-openharmony::react_debug",
            "react-native-openharmony::react_render_attributedstring",
            "react-native-openharmony::react_nativemodule_core",
            "react-native-openharmony::react_codegen_rncore",
            "react-native-openharmony::react_render_componentregistry",
            "react-native-openharmony::react_render_core",
            "react-native-openharmony::react_render_debug",
            "react-native-openharmony::react_render_graphics",
            "react-native-openharmony::react_render_imagemanager",
            "react-native-openharmony::react_render_mapbuffer",
            "react-native-openharmony::rrc_image",
            "react-native-openharmony::rrc_text",
            "react-native-openharmony::rrc_textinput",
            "react-native-openharmony::rrc_scrollview",
            "react-native-openharmony::rrc_view",
            "react-native-openharmony::runtimeexecutor",
            "react-native-openharmony::yoga"
          ]
        }
      ]
    }
  }
];

export const BuildOptionSetOfABC = BuildOptionSetOfRelease.map(item => {
  return {
    ...item,
    arkOptions: {
      byteCodeHar: true,
    }
  };
});

export const CMakeContent = `set(REACT_COMMON_PATCH_DIR "\${RNOH_CPP_DIR}/patches/react_native_core")

# folly的编译选项
set(folly_compile_options
    -DFOLLY_NO_CONFIG=1
    -DFOLLY_MOBILE=1
    -DFOLLY_USE_LIBCPP=1
    -DFOLLY_HAVE_RECVMMSG=1
    -DFOLLY_HAVE_PTHREAD=1
    -Wno-comma
    -Wno-shorten-64-to-32
    -Wno-documentation
    -faligned-new
)

if(DEFINED PACKAGE_FIND_FILE)
    include(\${PACKAGE_FIND_FILE})
endif()

# 添加rnoh
target_compile_definitions(react-native-openharmony::rnoh INTERFACE C_API_ARCH)
target_compile_options(react-native-openharmony::rnoh INTERFACE \${folly_compile_options} -DRAW_PROPS_ENABLED -DNDEBUG -std=c++17)
#JSExecutorFactory
if(USE_HERMES)
    target_link_libraries(react-native-openharmony::rnoh INTERFACE react-native-openharmony::hermes_executor)
    target_compile_definitions(react-native-openharmony::rnoh INTERFACE USE_HERMES=1)
else()
    target_link_libraries(react-native-openharmony::rnoh INTERFACE react-native-openharmony::jsvm_executor)
    target_compile_definitions(react-native-openharmony::rnoh INTERFACE USE_HERMES=0)
endif()

add_library(rnoh ALIAS react-native-openharmony::rnoh)

# 添加头文件目录
include_directories(\${RNOH_APP_DIR}
                    \${RNOH_CPP_DIR}
                    \${REACT_COMMON_PATCH_DIR}
                    \${RNOH_CPP_DIR}/third-party/folly
                    \${RNOH_CPP_DIR}/third-party/rn/ReactCommon
                    \${RNOH_CPP_DIR}/third-party/rn/ReactCommon/react/nativemodule/core
                    \${RNOH_CPP_DIR}/third-party/rn/ReactCommon/jsi
                    \${RNOH_CPP_DIR}/third-party/rn/ReactCommon/callinvoker
                    \${RNOH_CPP_DIR}/third-party/boost/libs/utility/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/stacktrace/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/predef/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/array/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/throw_exception/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/config/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/core/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/preprocessor/include
                    \${RNOH_CPP_DIR}/third-party/double-conversion
                    \${RNOH_CPP_DIR}/third-party/rn/ReactCommon/react/renderer/graphics/platform/cxx
                    \${RNOH_CPP_DIR}/third-party/rn/ReactCommon/runtimeexecutor
                    \${RNOH_CPP_DIR}/third-party/glog/src
                    \${RNOH_CPP_DIR}/third-party/boost/libs/mpl/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/type_traits/include
                    \${RNOH_CPP_DIR}/third-party/rn/ReactCommon/yoga
                    \${RNOH_CPP_DIR}/third-party/boost/libs/intrusive/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/assert/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/move/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/static_assert/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/container_hash/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/describe/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/mp11/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/iterator/include
                    \${RNOH_CPP_DIR}/third-party/boost/libs/detail/include
                    \${RNOH_CPP_DIR}/patches/react_native_core/react/renderer/textlayoutmanager/platform/harmony
                    )
`;
