CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

project(pepper_test)

include_directories(${MY_ROOT}/include
    ${GTEST_INCLUDE}
    )

link_directories(${GTEST_LIB})

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} 
    gtest
    pthread
    )
