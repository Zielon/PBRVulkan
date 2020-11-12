# Ray tracer sandbox in Vulkan
####  Vulkan real-time path tracer engine

This project is an engine where you can create/test new shaders in real-time physically based ray tracing system. The main idea is that users can test their own integrators in predefined environment with assets like: meshes, materials, lights and textures. The Vulkan pipeline is abstracted in a shaders form and the only concern for a user is providing either their own scene description or using already existing one.

Check out the discord [#general](https://discord.gg/365bNPbFTM) for more info or help ![discord](https://img.shields.io/discord/774330363810938890) \
The current development is available under [project](https://github.com/Zielon/PBRVulkan/projects/1) ![issues](https://img.shields.io/github/issues/Zielon/PBRVulkan)

<p align="center">
  <img src="https://github.com/Zielon/PBRVulkan/blob/readme-assets/gifs/bedroom.gif?raw=true">
</p>

#### Sandbox

As an example of creating your own shader please take a look at [Ambient Occlusion](https://github.com/Zielon/PBRVulkan/blob/master/PBRVulkan/RayTracer/src/Assets/Shaders/Raytracer/Integrators/AO.glsl) or [Path Tracer](https://github.com/Zielon/PBRVulkan/blob/master/PBRVulkan/RayTracer/src/Assets/Shaders/Raytracer/Integrators/PathTracer.glsl). Those files are automatically included in the `hit shader`. At this level you have many resources available to create your own implementation. For now, to make them available for the engine, you would need to add them in the same way I added aforementioned files.

#### How to run

1) Download [VulkanSDK](https://vulkan.lunarg.com/sdk/home#windows) and place it in `C:/VulkanSDK/x.x.x.x/`.
2) Run `install.bat` script to download all necessary libraries.
3) For shaders compliation you will need `python` (Python 3.x.x) and `glslc` available form the shell level. Check out `Compiler.h` and `Compile.py` for more details. `glslc` can be found for instance in `C:/VulkanSDK/x.x.x.x/Bin32/glslc.exe`.

#### Environment variables

Add vulkan path `C:/VulkanSDK/x.x.x.x/Bin32` to the `Path` variable \
System variable needed for the project
1) `VULKAN_SDK`
2) `VK_SDK_PATH`

C++17 is needed to compile the project.

#### Assets

The scene description and assets are taken from [GLSL-PathTracer](https://github.com/knightcrawler25/GLSL-PathTracer) project [4]. The whole dataset can be downloaded from [link](https://drive.google.com/file/d/1UFMMoVb5uB7WIvCeHOfQ2dCQSxNMXluB/view).

Download the assets folder and place it in `PBRVulkan/Assets/Scenes/`. The folder structure has to be as follows:

```
PBRVulkan/Assets/Scenes/
    bedroom/
    coffee_cart/
    HDR/
    bedroom.scene
    coffee_cart.scene
    ...
```

#### References/Credits

[0] [Vulkan Tutorial](https://vulkan-tutorial.com/) \
[1] [Vulkan Ray Tracing Tutorial](https://nvpro-samples.github.io/vk_raytracing_tutorial_KHR/) \
[2] [PBR Book](http://www.pbr-book.org/3ed-2018/contents.html) \
[4] [GLSL-PathTracer](https://github.com/knightcrawler25/GLSL-PathTracer) \
[5] [RayTracingInVulkan](https://github.com/GPSnoopy/RayTracingInVulkan)

#### Demo

<p align="center">
  <img src="https://github.com/Zielon/PBRVulkan/blob/readme-assets/gifs/stormtroopers.gif?raw=true">
</p>
