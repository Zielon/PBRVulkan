# Physically based renderer in Vulkan
####  Vulkan real-time path tracer engine
**[IN DEVELOPMENT]**\
Real-time path tracer based on the extension `VK_KHR_ray_tracing`.

#### How to run:

1) Download [VulkanSDK](https://vulkan.lunarg.com/sdk/home#windows) and place it in `C:/VulkanSDK/x.x.x.x/`.
2) Run `install.bat` script to download all necessary libraries.
3) For shaders compliation you will need `python` and `glslc` available form the shell level. `glslc` can be found for instance in `C:/VulkanSDK/x.x.x.x/Bin32/glslc.exe`.

C++17 is needed to compile the project.

#### Assets
The scene description and assets are taken from GLSL-PathTracer project.

#### References/Credits:
[0] [Vulkan Tutorial](https://vulkan-tutorial.com/)\
[1] [Vulkan Ray Tracing](https://nvpro-samples.github.io/vk_raytracing_tutorial_KHR/)\
[2] [PBR Book](http://www.pbr-book.org/3ed-2018/contents.html)
[4] [GLSL-PathTracer](https://github.com/knightcrawler25/GLSL-PathTracer)
[5] [RayTracingInVulkan](https://github.com/GPSnoopy/RayTracingInVulkan)