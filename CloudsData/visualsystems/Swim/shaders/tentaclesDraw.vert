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
    vec3 v0World = texture2DRect(particles0, vec2(0.0, gl_TexCoord[0].t)).xyz;
    vWorld.z += step(fogEnd, camZ - v0World.z) * fogEnd * floor((camZ - v0World.z) / fogEnd);
    vWorld.z -= step(camZ, v0World.z) * fogEnd * ceil((v0World.z - camZ) / fogEnd);
    
    vec3 vEye = (gl_ModelViewMatrix * vec4(vWorld, 1.0)).xyz;
    
    fogAmount = clamp((length(vEye) - fogStart) / (fogEnd - fogStart), 0.0, 1.0);

    gl_Position = gl_ModelViewProjectionMatrix * vec4(vWorld, 1.0);
}