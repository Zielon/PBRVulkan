mkdir Build
cd Build
git clone https://github.com/Microsoft/vcpkg.git vcpkg.windows
cd vcpkg.windows
git checkout 2020.06
call bootstrap-vcpkg.bat

vcpkg.exe install ^
	glfw3:x64-windows ^
	glm:x64-windows ^
	imgui:x64-windows ^
	stb:x64-windows ^
	tinyobjloader:x64-windows

vcpkg.exe integrate install

cd ../..

exit /b
