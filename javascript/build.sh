#!/bin/bash
# Build script for Iris Codec WebAssembly module

# Ensure Emscripten is available
if ! command -v emcmake &> /dev/null; then
    echo "Error: Emscripten toolchain not found. Please source emsdk_env.sh first."
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "configure: cmake .. -DCMAKE_BUILD_TYPE=Release"
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release

# Fix include paths by copying IrisCore.hpp to the priv directory
if [ -d _deps/irisheaders-src ]; then
    echo "Copying IrisCore.hpp to the priv directory..."
    cp _deps/irisheaders-src/include/IrisCore.hpp _deps/irisheaders-src/priv/
fi

# Apply compatibility changes to IrisFileExtension
if [ -d _deps/irisfileextension-src ]; then
    echo "Applying compatibility changes to IrisFileExtension..."
    
    # Create EmscriptenCompat.hpp if it doesn't exist
    cat > _deps/irisfileextension-src/src/EmscriptenCompat.hpp << 'EOF'
#ifndef EMSCRIPTEN_COMPAT_HPP
#define EMSCRIPTEN_COMPAT_HPP

#include <cstring>
#include <type_traits>

// Provide a bit_cast implementation if std::bit_cast is not available
namespace std {
    template <typename To, typename From>
    typename std::enable_if<
        sizeof(To) == sizeof(From) &&
        std::is_trivially_copyable<From>::value &&
        std::is_trivially_copyable<To>::value,
        To>::type
    bit_cast(const From& src) noexcept {
        To dst;
        std::memcpy(&dst, &src, sizeof(To));
        return dst;
    }
}

#endif // EMSCRIPTEN_COMPAT_HPP
EOF

    # Add include for EmscriptenCompat.hpp at the top of IrisCodecExtension.cpp
    sed -i '1i\#include "EmscriptenCompat.hpp"' _deps/irisfileextension-src/src/IrisCodecExtension.cpp
    
    # Fix IFE_EXPORT issues directly with sed
    sed -i 's/namespace IFE_EXPORT Abstraction/namespace Abstraction/g' _deps/irisfileextension-src/src/IrisCodecExtension.hpp
    sed -i 's/struct IFE_EXPORT Header/struct Header/g' _deps/irisfileextension-src/src/IrisCodecExtension.hpp
    sed -i 's/IFE_EXPORT //g' _deps/irisfileextension-src/src/IrisCodecExtension.hpp
fi

# Copy our custom files
echo "Copying custom Emscripten-compatible files..."
cp ../src/EmscriptenCompat.hpp .
cp ../src/IrisCodecContext.cpp .
cp ../src/IrisCodecFile.cpp .

# Build
echo "make: make -j$(nproc)"
emmake make -j$(nproc)

# Copy output files to examples directory
mkdir -p ../examples/js
cp iris_codec.js iris_codec.wasm ../examples/js/ 2>/dev/null || echo "Warning: Could not copy output files"

echo "Build complete. WebAssembly module is available in examples/js/"
cd ..
