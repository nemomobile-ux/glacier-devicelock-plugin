set(TESTNAME devicepassword)

set(SRC
    ut_${TESTNAME}.cpp
    ut_${TESTNAME}.h
    ${CMAKE_SOURCE_DIR}/lib/glacierdevicepassword.cpp)

add_executable(ut_${TESTNAME} ${SRC})

include_directories(${CMAKE_SOURCE_DIR}/lib/)

target_link_libraries(ut_${TESTNAME}
    Qt6::Core
    Qt6::Test)

add_test(ut_${TESTNAME} ut_${TESTNAME})
