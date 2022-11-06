cmake_minimum_required(VERSION 3.00)
project(Xinlog)

set(CMAKE_CXX_STANDARD 99)

add_subdirectory(depend/fmt EXCLUDE_ALL)


include_directories(./include)
include_directories(./depend/fmt/include)

if (DEFINED TEST_MAIN)
    add_subdirectory(tests)
endif()


file (GLOB SRC ./src/*.cc)


# 生成动态库
add_library(logger SHARED ${SRC})

# 链接依赖的动态链接库
target_link_libraries(logger fmt-header-only pthread)



