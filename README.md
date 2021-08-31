# Ray tracer sandbox in Vulkan

This project is an engine where you can create/test new shaders in real-time physically based ray tracing system. The main idea is that users can test their own integrators in predefined environment with assets like: meshes, materials, lights and textures. The Vulkan pipeline is abstracted in a shaders form and the only concern for a user is providing either their own scene description or using already existing one.

 ![discord](https://img.shields.io/discord/774330363810938890) ![issues](https://img.shields.io/github/issues/Zielon/PBRVulkan) ![release](https://img.shields.io/github/v/release/Zielon/PBRVulkan) ![closed](https://img.shields.io/github/issues-closed-raw/Zielon/PBRVulkan) ![Linux](https://img.shields.io/badge/system-Linux-green) ![Windwos](https://img.shields.io/badge/system-Windows-green)

Check out the discord [#general](https://discord.gg/365bNPbFTM) for more info or [discussions](https://github.com/Zielon/PBRVulkan/discussions). The current development is available under [project](https://github.com/Zielon/PBRVulkan/projects/1).  

<p align="center">
  <img src="https://github.com/Zielon/PBRVulkan/blob/readme-assets/gifs/bedroom.gif?raw=true" width=auto height=auto>
</p>

#### Sandbox

As an example of creating your own shader please take a look at [Ambient Occlusion](https://github.com/Zielon/PBRVulkan/blob/master/PBRVulkan/RayTracer/src/Assets/Shaders/Raytracer/Integrators/AO.glsl) or [Path Tracer](https://github.com/Zielon/PBRVulkan/blob/master/PBRVulkan/RayTracer/src/Assets/Shaders/Raytracer/Integrators/PathTracer.glsl). Those files are automatically included in the `hit shader`. At this level you have many resources available to create your own implementation. For now, to make them available for the engine, you would need to add them in the same way I added aforementioned files.

#### Features
```
1) Ray tracing pipeline
    scene/assets description with:
      materials
      meshes
      lights (area, spherical)
    HDR
    path tracer integral
    Disney BRDF
2) Rasterizer pipeline     
    Phong-Blinn model
3) Compute shader pipeline 
    denoiser
    egde detection
    edge sharpner
```
#### How to run

1) Download [VulkanSDK](https://vulkan.lunarg.com/sdk/home#windows) and install it following the instuctions.
2) Run `windows_install.bat` or `sudo linux_install.sh` script depending on the system to download all necessary libraries. 
3) `git-lfs` must be installed in order to fetch scenes repository.

C++17 is needed to compile the project.

#### Assets

The scene description and assets are adapted from [GLSL-PathTracer](https://github.com/knightcrawler25/GLSL-PathTracer) project [4]. The [scenes repository](https://github.com/Zielon/PBRScenes) will be automatically cloned during the installation process.

```
PBRVulkan/Assets/PBRScenes/
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
