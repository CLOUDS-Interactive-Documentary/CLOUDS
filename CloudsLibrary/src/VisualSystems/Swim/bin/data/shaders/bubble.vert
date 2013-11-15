#version 120

uniform samplerCube cubeMap;
uniform vec4 trans;
uniform float size;

varying vec3 normal;

void main()
{
    vec4 vertex = vec4(gl_Vertex.x * size, gl_Vertex.y * size, gl_Vertex.z * size, 1.0) + trans;
    gl_TexCoord[0] = gl_MultiTexCoord0; 
    normal = gl_NormalMatrix * gl_Normal; 
    gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
