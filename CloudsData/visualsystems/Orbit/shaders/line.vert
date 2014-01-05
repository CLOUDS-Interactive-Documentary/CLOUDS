#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect particles0;

uniform float fogStart;
uniform float fogEnd;
uniform float hueMin;
uniform float hueMax;

varying float fogAmount;

vec3 hsvToRgb(vec3 hsv) { return mix(vec3(1.),clamp((abs(fract(hsv.x+vec3(3.,2.,1.)/3.)*6.-3.)-1.),0.,1.),hsv.y)*hsv.z; }

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    
    vec3 vWorld = texture2DRect(particles0, gl_TexCoord[0].st).xyz;
    
    float hueFactor = clamp(((abs(vWorld.x) + abs(vWorld.y)) / 50.0), 0.0, 1.0);
    
    float hue = mix(hueMin, hueMax, hueFactor);
    
    gl_FrontColor = vec4(hsvToRgb(vec3(hue, 0.5, 1.0)), gl_Color.a);
    
    vec3 vEye = (gl_ModelViewMatrix * vec4(vWorld, 1.0)).xyz;
    fogAmount = clamp((length(vEye) - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * vec4(vWorld, 1.0);
}