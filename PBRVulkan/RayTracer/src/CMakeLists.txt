set(exe_name ${MAIN_PROJECT})

set(src_files_assets
        Assets/Light.h
        Assets/Material.h
        Assets/Mesh.cpp
        Assets/Mesh.h
        Assets/Texture.h
        Assets/Texture.cpp
    )

set(src_files_geometry
        Geometry/Compute.h
        Geometry/Global.h
        Geometry/Vertex.h
    )

set(src_files_3rdParty
        3rdParty/HDRLoader.cpp
        3rdParty/HDRLoader.h
    )

set(src_files_imgui
        ImGui/imgui_impl_glfw.cpp
        ImGui/imgui_impl_glfw.h
        ImGui/imgui_impl_vulkan.cpp
        ImGui/imgui_impl_vulkan.h
    )

set(src_files_loader
        Loader/Loader.cpp
        Loader/Loader.h
        Loader/RenderOptions.h
    )

set(src_files_tracer
        Tracer/Widgets/CinemaWidget.cpp
        Tracer/Widgets/CinemaWidget.h
        Tracer/Widgets/RendererWidget.cpp
        Tracer/Widgets/RendererWidget.h
        Tracer/Widgets/SceneWidget.h
        Tracer/Widgets/SceneWidget.cpp
        Tracer/Widgets/Widget.h

        Tracer/Application.cpp
        Tracer/Application.h
        Tracer/Camera.cpp
        Tracer/Camera.h
        Tracer/Compiler.h
        Tracer/Compiler.cpp
        Tracer/Menu.cpp
        Tracer/Menu.h
        Tracer/Scene.cpp
        Tracer/Scene.h
        Tracer/Settings.h
        Tracer/TextureImage.cpp
        Tracer/TextureImage.h
    )

set(src_files_vulkan
        Vulkan/AccelerationStructure.cpp
        Vulkan/CommandBuffers.h
        Vulkan/Computer.h
        Vulkan/DescriptorsManager.cpp
        Vulkan/Fence.h
        Vulkan/Instance.cpp
        Vulkan/Rasterizer.h
        Vulkan/Semaphore.cpp
        Vulkan/Surface.h
        Vulkan/Vulkan.cpp
        Vulkan/AccelerationStructure.h
        Vulkan/Command.cpp
        Vulkan/Core.cpp
        Vulkan/DescriptorsManager.h
        Vulkan/Framebuffer.cpp
        Vulkan/Instance.h
        Vulkan/Raytracer.cpp
        Vulkan/Semaphore.h
        Vulkan/SwapChain.cpp
        Vulkan/Vulkan.h
        Vulkan/BLAS.cpp
        Vulkan/CommandPool.cpp
        Vulkan/Core.h
        Vulkan/Device.cpp
        Vulkan/Framebuffer.h
        Vulkan/Memory.cpp
        Vulkan/RaytracerGraphicsPipeline.cpp
        Vulkan/ShaderBindingTable.cpp
        Vulkan/SwapChain.h
        Vulkan/Window.cpp
        Vulkan/BLAS.h
        Vulkan/CommandPool.h
        Vulkan/DepthBuffer.cpp
        Vulkan/Device.h
        Vulkan/Image.cpp
        Vulkan/Memory.h
        Vulkan/RaytracerGraphicsPipeline.h
        Vulkan/ShaderBindingTable.h
        Vulkan/TextureSampler.cpp
        Vulkan/Window.h
        Vulkan/Buffer.cpp
        Vulkan/ComputePipeline.cpp
        Vulkan/DepthBuffer.h
        Vulkan/Extensions.cpp
        Vulkan/Image.h
        Vulkan/Rasterizer.cpp
        Vulkan/Raytracer.h
        Vulkan/Shader.cpp
        Vulkan/TextureSampler.h
        Vulkan/Buffer.h
        Vulkan/ComputePipeline.h
        Vulkan/DescriptorSetLayout.cpp
        Vulkan/Extensions.h
        Vulkan/ImageView.cpp
        Vulkan/RasterizerGraphicsPipeline.cpp
        Vulkan/RenderPass.cpp
        Vulkan/Shader.h
        Vulkan/TLAS.cpp
        Vulkan/CommandBuffers.cpp
        Vulkan/Computer.cpp
        Vulkan/DescriptorSetLayout.h
        Vulkan/Fence.cpp
        Vulkan/ImageView.h
        Vulkan/RasterizerGraphicsPipeline.h
        Vulkan/RenderPass.h
        Vulkan/Surface.cpp
        Vulkan/TLAS.h
    )

set(src_files
        main.cpp
    )

source_group("3rdParty" FILES ${src_files_3rdParty})
source_group("Assets" FILES ${src_files_assets})
source_group("ImGui" FILES ${src_files_imgui})
source_group("Geometry" FILES ${src_files_geometry})
source_group("Loader" FILES ${src_files_loader})
source_group("Tracer" FILES ${src_files_tracer})
source_group("Vulkan" FILES ${src_files_vulkan})
source_group("Main" FILES ${src_files})

add_executable(${exe_name}
        ${src_files_3rdParty}
        ${src_files_assets}
        ${src_files_imgui}
        ${src_files_geometry}
        ${src_files_loader}
        ${src_files_tracer}
        ${src_files_vulkan}
        ${src_files}
    )

if (UNIX)
    # GCC8 needs an extra lib for <filesystem>.
    # This is not needed with GCC9 or higher.
    set(extra_libs -lstdc++fs ${Backtrace_LIBRARIES})
endif()

set_target_properties(${exe_name} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_include_directories(${exe_name} PRIVATE . ${Boost_INCLUDE_DIRS} ${glfw3_INCLUDE_DIRS} ${glm_INCLUDE_DIRS} ${STB_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
target_link_directories(${exe_name} PRIVATE ${Vulkan_LIBRARY})
target_link_libraries(${exe_name} PRIVATE ${Boost_LIBRARIES} freetype glfw glm::glm imgui::imgui tinyobjloader::tinyobjloader ${Vulkan_LIBRARIES} ${extra_libs})
