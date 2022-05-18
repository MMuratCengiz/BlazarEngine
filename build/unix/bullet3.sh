#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

PROJECT_PATH="${SCRIPT_DIR}/../../"
BULLET_PATH="${PROJECT_PATH}/external/bullet3"
BUILD_PATH="${PROJECT_PATH}/build/bullet3/unix/"

cmake -DINSTALL_LIBS=ON -G "CodeBlocks - Unix Makefiles" -DBUILD_UNIT_TESTS=OFF -DBUILD_OPENGL3_DEMOS=OFF -DBUILD_BULLET2_DEMOS=OFF -S "${BULLET_PATH}" -B "${BUILD_PATH}" -DCMAKE_INSTALL_PREFIX:path="${BUILD_PATH}"
cmake --build "${BUILD_PATH}"
cmake --install "${BUILD_PATH}"