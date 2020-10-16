import os
import glob
import subprocess
from shutil import copyfile

# The root is \PBRVulkan\Vulkan\

output_folder = os.path.dirname(os.path.abspath(__file__)) 
shaders = output_folder + "/../src/Assets/Shaders/"
assets = "/../../Assets/Shaders/"

mk = output_folder + assets
if not os.path.exists(mk):
    os.makedirs(mk)

extensionsToCheck = ('.frag', '.vert', '.rchit', '.rgen', '.rmiss')

for path in glob.iglob(shaders + '**/*', recursive=True):

    if not path.endswith(extensionsToCheck):
        continue

    shader_name = os.path.split(path)[-1] + ".spv"
    output = output_folder + assets + shader_name
    cmd = "glslc " + path + " -o " + output
    subprocess.run(cmd.split())
    print("[INFO] {} has been processed".format(shader_name))
