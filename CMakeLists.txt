cmake_minimum_required(VERSION 3.5)
project(topper)
project(toppertest)

set(CMAKE_CXX_STANDARD 14)

#find_package(Boost 1.61.0 REQUIRED) #or version must be >= 1.61.0
#find_package(GTest 1.26.2 REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

include(conan)
conan_cmake_run(CONANFILE conanfile.txt
    BASIC_SETUP NO_OUTPUT_DIRS CMAKE_TARGETS
    BUILD missing)

IF( PLATFORM_ARM AND NOT CROSS_COMPILE )
    message(STATUS "PLATFORM_ARM AND NOT CROSS_COMPILE")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -fexpensive-optimizations -pipe -fexceptions -Wpointer-arith -Wcast-align -Wsign-compare -pedantic -march=native -fsanitize=address")
ELSE( )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -fexpensive-optimizations -pipe -fexceptions -Wpointer-arith -Wcast-align -Wsign-compare -pedantic -march=native -mmmx -msse -msse2 -msse3 -mmmx -m3dnow ")#-fsanitize=address")
ENDIF( PLATFORM_ARM AND NOT CROSS_COMPILE )
#-fomit-frame-pointer -funroll-loops - убрал

add_executable(topper main.cpp user.cpp user.h woker.cpp woker.h )
add_executable(toppertest wokertests.cpp user.cpp user.h woker.cpp woker.h )

target_link_libraries(topper CONAN_PKG::boost CONAN_PKG::gtest)
target_link_libraries(toppertest CONAN_PKG::boost CONAN_PKG::gtest)
