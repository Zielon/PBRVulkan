#!/bin/sh
set -e

mkdir Build
cd Build
git clone https://github.com/Microsoft/vcpkg.git vcpkg.linux
cd vcpkg.linux
git checkout 2021.05.12
./bootstrap-vcpkg.sh

# glfw3:x64-linux requires:
# sudo apt install libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev

./vcpkg install \
	glfw3:x64-linux \
	glm:x64-linux \
	imgui:x64-linux \
	stb:x64-linux \
	tinyobjloader:x64-linux

cd ../..

cd PBRVulkan/Assets
git clone https://github.com/Zielon/PBRScenes

cd ../..

mkdir --parents Build/linux
cd Build/linux
cmake -D CMAKE_BUILD_TYPE=Release -D VCPKG_TARGET_TRIPLET=x64-linux -D CMAKE_TOOLCHAIN_FILE=../vcpkg.linux/scripts/buildsystems/vcpkg.cmake ../..
make -j