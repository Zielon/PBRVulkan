import os
import subprocess

root = os.path.dirname(os.path.abspath(__file__)) 
shaders = root + "/../src/Assets/Shaders/"

for filename in os.listdir(shaders):
    path = shaders + filename
    output = root + "/../Shaders/" + filename.split(".")[0] + ".spv"
    cmd = "glslc " + path + " -o " + output
    subprocess.run(cmd.split())