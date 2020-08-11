@ECHO OFF
echo Compiling vertex and fragment shaders....
glslangvalidator -V -H -S vert vertexShader.glsl > scomp.txt
glslangvalidator -V -H -S frag fragTexture.glsl > fcomp.txt