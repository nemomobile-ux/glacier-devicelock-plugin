cmake_minimum_required(VERSION 3.6.0)

project(glacier-devicelock-plugin
    VERSION 0.3
    DESCRIPTION "The Glacier devicelock plugin")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FeatureSummary)
include(GNUInstallDirs)

set(CMAKE_INSTALL_PREFIX /usr)

set(QT_MIN_VERSION "6.0.0")
find_package(Qt6 ${QT_MIN_VERSION} COMPONENTS Core DBus REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLIB REQUIRED glib-2.0)

add_subdirectory(lib)
add_subdirectory(src)

configure_file(devicelock.conf.in
    ${CMAKE_CURRENT_BINARY_DIR}/devicelock.conf
    @ONLY)

install(FILES  ${CMAKE_CURRENT_BINARY_DIR}/devicelock.conf
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/lipstick/devicelock/)
