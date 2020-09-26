@ECHO OFF
echo Compiling vertex and fragment shaders....
glslangvalidator -V -H -S vert vertexCubeQuaternion.glsl > scomp.txt
glslangvalidator -V -H -S frag fragmentCube.glsl > fcomp.txt

glslangvalidator -V -o vertexBack.spv -H -S vert vertexBackdrop.glsl > scomp2.txt
glslangvalidator -V -o fragmentBack.spv -H -S frag fragmentBackdrop.glsl > fcomp2.txt