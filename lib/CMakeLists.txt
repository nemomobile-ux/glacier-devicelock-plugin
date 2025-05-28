set(PACKAGE glacierdevicepassword${LIBGLACIER_PKG_VER})

set(SRC glacierdevicepassword.cpp)
set(HEADERS glacierdevicepassword.h)

add_library(${PACKAGE} SHARED ${SRC} ${HEADERS} ${PUBLIC_HEADERS}
    libglacierdevicepassword.pc.in)
add_library(Glacier::DevicePassword ALIAS ${PACKAGE})

target_include_directories(${PACKAGE} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/lib${PACKAGE}>)

set_target_properties(${PACKAGE} PROPERTIES
    EXPORT_NAME App
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION})

target_link_libraries(${PACKAGE}
    Qt6::Core)

# Configure and install the pkgconfig file
configure_file(libglacierdevicepassword.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}.pc
    @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Install the library
install(TARGETS ${PACKAGE}
    EXPORT GlacierTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Install headers
install(FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/glacierdevicepassword
    COMPONENT Devel)
