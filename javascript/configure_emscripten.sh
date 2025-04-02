#!/bin/bash

# This script sets up the Emscripten environment for building the Iris Codec

# Install dependencies
apt-get update
apt-get install -y git cmake build-essential python3 wget

# Install Emscripten SDK
# Download the Emscripten SDK
cd /tmp && wget https://github.com/emscripten-core/emsdk/archive/master.tar.gz
tar -xvf master.tar.gz
cd emsdk-master

# Install the latest version of Emscripten
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh