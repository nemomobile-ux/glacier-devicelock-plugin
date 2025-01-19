cmake_minimum_required(VERSION 3.6.0)

project(glacier-devicelock-plugin
    VERSION 0.2.1
    DESCRIPTION "The Glacier devicelock plugin")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FeatureSummary)
include(GNUInstallDirs)

set(QT_MIN_VERSION "6.0.0")
find_package(Qt6 ${QT_MIN_VERSION} COMPONENTS Core DBus REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLIB REQUIRED glib-2.0)

add_subdirectory(src)

install(FILES devicelock.conf
    DESTINATION ${SHARE_INSTALL_PREFIX}/lipstick/devicelock/)
