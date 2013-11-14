#version 120

uniform samplerCube cubeMap;

varying vec3 normal;

void main()
{
    vec4 col = textureCube(cubeMap, normal);
    gl_FragColor = vec4(col.rgb, 0.4);
}