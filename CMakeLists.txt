cmake_minimum_required(VERSION 3.20)

option(BUILD_SHARED_LIBS FALSE)
set(CMAKE_CXX_CLANG_FORMAT "clang-format -style=clang-format.json -fallback-style=Google")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if (WIN32)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

project(pesto)

file(GLOB_RECURSE SRC_FILES src/*.cpp vendor/lodepng/lodepng.cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # #######################
    # DEBUG         #
    # #######################
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -fsanitize=address,leak,undefined -fno-omit-frame-pointer -fasynchronous-unwind-tables -no-pie -ggdb3 -Wall -flto=auto")
    set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address,leak,undefined -fno-omit-frame-pointer -no-pie -Wall -fno-omit-frame-pointer")

    # #######################
    # RELEASE       #
    # #######################
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIE -D_FORTIFY_SOURCE=2 -fexceptions -Wall -O2 -flto=auto -funroll-loops")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_LINKER_FLAGS_RELEASE} -fPIE  -s")

# -static -static-libgcc -static-libstdc++
elseif(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox /GL")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")
endif()

# include(FetchContent)

find_package(libheif)


add_executable(pesto ${SRC_FILES})

target_include_directories(pesto PRIVATE "vendor/" "src/")
target_include_directories(pesto PRIVATE ${libheif_SOURCE_DIR} ${libheif_BINARY_DIR})


target_link_libraries(pesto PRIVATE ${CMAKE_CXX_STANDARD_LIBRARIES} heif)