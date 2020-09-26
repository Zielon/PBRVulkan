import os
import subprocess
from shutil import copyfile

# The root is \PBRVulkan\Vulkan\

root = os.path.dirname(os.path.abspath(__file__)) 
shaders = root + "/../src/Assets/Shaders/"
assets = "/../../Assets/Shaders/"

mk = root + assets
if not os.path.exists(mk):
    os.makedirs(mk)

for filename in os.listdir(shaders):
    path = shaders + filename
    output = root + assets + filename.split(".")[0] + ".spv"
    cmd = "glslc " + path + " -o " + output
    subprocess.run(cmd.split())