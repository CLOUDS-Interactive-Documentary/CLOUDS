#version 120
#extension GL_ARB_texture_rectangle : enable

// ping pong inputs
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;

uniform float restLength;
uniform float elapsed;
uniform float numSections;

float SPRING_CONSTANT = 100.0;

vec3 springForce(vec3 pos, vec3 otherPos)
{
    vec3 forceDirection = otherPos - pos;
    
    float forceMagnitude = SPRING_CONSTANT * (length(forceDirection) - restLength);
    
    vec3 force = normalize(forceDirection) * forceMagnitude;
    
    return force;
}

void main()
{
    vec3 pos = texture2DRect(particles0, gl_TexCoord[0].st).xyz;
    vec3 vel = texture2DRect(particles1, gl_TexCoord[0].st).xyz;
    
    vec3 parentPos = texture2DRect(particles0, vec2(gl_TexCoord[0].s - 1.0, gl_TexCoord[0].t)).xyz;
    
    vec3 forceDirection = parentPos - pos;
    
    vec3 oldVel = vel;
    
    // for some reason nvidia 9600 osx driver is freaking
    // out if I try and get the length of or normalize forceDirection
    vel += 120.0 * elapsed * forceDirection;
    
    vel *= clamp(1.0 - (9.0 * elapsed), 0.0, 1.0);
    
    pos += 0.5 * (oldVel + vel) * elapsed;//step(0.5, gl_TexCoord[0].s) * 0.5 * (vel + oldVel) * elapsed;
    
    gl_FragData[0] = vec4(pos, 1.0);
    gl_FragData[1] = vec4(vel, 0.0);
}