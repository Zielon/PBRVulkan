mkdir Build
cd Build
git clone https://github.com/Microsoft/vcpkg.git vcpkg.windows
cd vcpkg.windows
git checkout 2020.06
call bootstrap-vcpkg.bat

vcpkg.exe install ^
	glfw3:x64-windows-static ^
	glm:x64-windows-static ^
	imgui:x64-windows-static ^
	stb:x64-windows-static ^
	tinyobjloader:x64-windows-static

vcpkg.exe integrate install

cd ../..

exit /b
