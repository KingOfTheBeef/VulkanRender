@ECHO OFF
echo Compiling vertex and fragment shaders....
glslangvalidator -V -H -S vert vertexCube.glsl > scomp.txt
glslangvalidator -V -H -S frag fragmentCube.glsl > fcomp.txt