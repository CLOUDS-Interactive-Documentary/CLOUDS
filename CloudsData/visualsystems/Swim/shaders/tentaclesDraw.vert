#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect particles0;
uniform float camZ;
uniform float fogStart;
uniform float fogEnd;

varying float fogAmount;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
    
    vec3 vWorld = texture2DRect(particles0, gl_TexCoord[0].st).xyz;
    vWorld.z += step(fogEnd, camZ - vWorld.z) * fogEnd * floor((camZ - vWorld.z) / fogEnd);
    vWorld.z -= step(camZ, vWorld.z) * fogEnd * ceil((vWorld.z - camZ) / fogEnd);
    
    vec3 vEye = (gl_ModelViewMatrix * vec4(vWorld, 1.0)).xyz;
    
    fogAmount = clamp((length(vEye) - fogStart) / (fogEnd - fogStart), 0.0, 1.0);

    gl_Position = gl_ModelViewProjectionMatrix * vec4(vWorld, 1.0);
}