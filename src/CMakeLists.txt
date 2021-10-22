set(SRC main.cpp
    devicelocking.cpp)

set(HEADERS devicelocking.h)

add_executable(glacier-devicelock ${SRC})

target_link_libraries(glacier-devicelock
        Qt5::Core
        Qt5::DBus
        ${GLIB_LDFLAGS})

target_include_directories(glacier-devicelock PRIVATE ${GLIB_INCLUDE_DIRS})

install(TARGETS glacier-devicelock RUNTIME
	DESTINATION /usr/lib/qt5/plugins/devicelock/)
