cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE on)

set(rnoh_sample_package_generated_dir "${RNOH_GENERATED_DIR}/react_native_harmony_sample_package")
file(GLOB_RECURSE rnoh_sample_package_generated_SRC "${rnoh_sample_package_generated_dir}/**/*.cpp")
file(GLOB rnoh_sample_package_SRC CONFIGURE_DEPENDS *.cpp)
add_library(rnoh_sample_package SHARED ${rnoh_sample_package_SRC} ${rnoh_sample_package_generated_SRC})
target_include_directories(rnoh_sample_package PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${rnoh_sample_package_generated_dir})
target_link_libraries(rnoh_sample_package PUBLIC rnoh)
