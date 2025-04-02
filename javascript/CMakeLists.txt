cmake_minimum_required(VERSION 3.14)
project(IrisCodecWasm VERSION 2025.1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add necessary definitions
add_compile_definitions(IFE_EXPORT=)
add_compile_definitions(IFE_EXPORT_API=false)
add_compile_definitions(__EMSCRIPTEN__=1)

# Include FetchContent for dependencies
include(FetchContent)

# Fetch Iris Headers
FetchContent_Declare(
    IrisHeaders
    GIT_REPOSITORY https://github.com/IrisDigitalPathology/Iris-Headers.git
    GIT_TAG "origin/main"
    GIT_SHALLOW ON
)

# Fetch Iris File Extension
FetchContent_Declare(
    IrisFileExtension
    GIT_REPOSITORY https://github.com/IrisDigitalPathology/Iris-File-Extension.git
    GIT_TAG "origin/main"
    GIT_SHALLOW ON
)

# Make the content available
FetchContent_MakeAvailable(
    IrisHeaders
    IrisFileExtension
)

# Print version information
message(STATUS "BUILDING IRIS CODEC WASM MODULE VERSION v${PROJECT_VERSION}")

# Set source files - use our custom Emscripten versions
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/IrisCodecEmscripten.cpp
    ${CMAKE_SOURCE_DIR}/src/IrisCodecContext.cpp  # Use our custom version
    ${CMAKE_SOURCE_DIR}/src/IrisCodecFile.cpp     # Use our custom version
    ${CMAKE_SOURCE_DIR}/../src/IrisCodecSlide.cpp
    ${CMAKE_SOURCE_DIR}/../src/IrisCodecCache.cpp
    ${irisheaders_SOURCE_DIR}/src/IrisBuffer.cpp
)

# Create the WebAssembly module
add_executable(iris_codec ${SOURCES})

# Add include directories
target_include_directories(iris_codec PRIVATE
    ${CMAKE_SOURCE_DIR}/../src
    ${CMAKE_SOURCE_DIR}/src
    ${irisheaders_SOURCE_DIR}/include
    ${irisheaders_SOURCE_DIR}/priv
    ${irisfileextension_SOURCE_DIR}/src
)

# Link with the Iris File Extension library
target_link_libraries(iris_codec PRIVATE
    IrisFileExtensionStatic
)

# Set Emscripten-specific options
set_target_properties(iris_codec PROPERTIES
    SUFFIX ".js"
    LINK_FLAGS "--bind -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s MODULARIZE=1 -s EXPORT_NAME='IrisCodec' -s EXPORTED_RUNTIME_METHODS=['FS'] -s ENVIRONMENT='web' -s ERROR_ON_UNDEFINED_SYMBOLS=0"
)
