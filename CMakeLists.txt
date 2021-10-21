cmake_minimum_required(VERSION 3.6.0)

project(glacier-devicelock-plugin
    VERSION 0.1
    DESCRIPTION "The Glacier devicelock plugin")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FeatureSummary)
include(GNUInstallDirs)

set(QT_MIN_VERSION "5.12.0")
find_package(Qt5 ${QT_MIN_VERSION} COMPONENTS Core DBus REQUIRED)

add_subdirectory(src)

install(FILES devicelock.conf
    DESTINATION /usr/share/lipstick/devicelock/)

install(FILES devicelock_settings.conf
    DESTINATION /usr/share/lipstick/devicelock/)
