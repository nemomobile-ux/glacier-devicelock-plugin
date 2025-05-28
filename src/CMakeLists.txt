set(SRC main.cpp
    devicelocking.cpp)

set(HEADERS devicelocking.h)

add_executable(glacier-devicelock ${SRC})

target_link_libraries(glacier-devicelock
        Qt6::Core
        Qt6::DBus
        Glacier::DevicePassword
        ${GLIB_LDFLAGS})

target_include_directories(glacier-devicelock
    PRIVATE ${GLIB_INCLUDE_DIRS}
    Glacier::DevicePassword)

install(TARGETS glacier-devicelock RUNTIME
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt6/plugins/devicelock/)
