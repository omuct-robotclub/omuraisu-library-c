cmake_minimum_required(VERSION 3.14)
project(omuraisu-library
    VERSION 1.0.0
    DESCRIPTION "omuraisu Library - Robot development C library"
    LANGUAGES C
)

# C17を使用
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# コンパイルオプション
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ライブラリのインクルードディレクトリ
set(OMURAISU_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# coordinateライブラリ（実装ファイルあり）
add_library(omuraisu_coordinate
    src/coordinate/coordinate.c
)
target_include_directories(omuraisu_coordinate PUBLIC
    $<BUILD_INTERFACE:${OMURAISU_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

add_library(omuraisu_pid
    src/pid/pid.c
)
target_include_directories(omuraisu_pid PUBLIC
    $<BUILD_INTERFACE:${OMURAISU_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

add_library(omuraisu_chassis INTERFACE)
target_include_directories(omuraisu_chassis INTERFACE
    $<BUILD_INTERFACE:${OMURAISU_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(omuraisu_chassis INTERFACE omuraisu_coordinate)

add_library(omuraisu_can INTERFACE)
target_include_directories(omuraisu_can INTERFACE
    $<BUILD_INTERFACE:${OMURAISU_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

add_library(omuraisu_dji INTERFACE)
target_include_directories(omuraisu_dji INTERFACE
    $<BUILD_INTERFACE:${OMURAISU_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(omuraisu_dji INTERFACE omuraisu_can)

add_library(omuraisu_controller INTERFACE)
target_include_directories(omuraisu_controller INTERFACE
    $<BUILD_INTERFACE:${OMURAISU_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(omuraisu_controller INTERFACE omuraisu_can)

add_library(omuraisu_servo INTERFACE)
target_include_directories(omuraisu_servo INTERFACE
    $<BUILD_INTERFACE:${OMURAISU_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(omuraisu_servo INTERFACE omuraisu_can)

# メインライブラリ（全てをまとめる）
add_library(omuraisu INTERFACE)
target_link_libraries(omuraisu INTERFACE
    omuraisu_coordinate
    omuraisu_pid
    omuraisu_chassis
    omuraisu_can
    omuraisu_dji
    omuraisu_controller
    omuraisu_servo
)

# サンプルのビルドオプション
option(BUILD_EXAMPLES "Build example programs" OFF)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# テストのビルドオプション
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# インストール設定
include(GNUInstallDirs)

install(TARGETS omuraisu_coordinate omuraisu_pid omuraisu_chassis omuraisu_can omuraisu_dji omuraisu_controller omuraisu_servo omuraisu
    EXPORT omuraisu-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/omuraisu
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT omuraisu-targets
    FILE omuraisu-targets.cmake
    NAMESPACE omuraisu::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/omuraisu
)

# パッケージ設定ファイルの生成
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/omuraisu-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/omuraisu-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/omuraisu
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/omuraisu-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/omuraisu-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/omuraisu-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/omuraisu
)
