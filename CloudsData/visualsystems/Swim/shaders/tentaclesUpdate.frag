#version 120
#extension GL_ARB_texture_rectangle : enable

// ping pong inputs
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;

uniform float restLength;
uniform float elapsed;
uniform float numSections;
uniform float tentacleSpringForce;
const float EPSILON = 1e-6;

float SPRING_CONSTANT = 100.0;

vec3 springForce(vec3 pos, vec3 otherPos)
{
    vec3 forceDirection = otherPos - pos;
    
    float forceLength = length(forceDirection);
    float forceMagnitude = SPRING_CONSTANT * (forceLength - restLength);
    
    vec3 force = abs(forceLength) < EPSILON ? vec3(0.0) : (forceDirection / forceLength) * forceMagnitude;
    
    return force;
}

void main()
{
    vec3 pos = texture2DRect(particles0, gl_TexCoord[0].st).xyz;
    vec3 vel = texture2DRect(particles1, gl_TexCoord[0].st).xyz;
    
    vec3 parentPos = texture2DRect(particles0, vec2(gl_TexCoord[0].s - 1.0, gl_TexCoord[0].t)).xyz;
    
    //vec3 oldVel = vel;
    
    vel += elapsed * springForce(pos, parentPos);

    vel *= clamp(1.0 - (9.0 * elapsed), 0.0, 1.0);
    
    pos += vel * elapsed;//step(0.5, gl_TexCoord[0].s) * 0.5 * (vel + oldVel) * elapsed;
    
    gl_FragData[0] = vec4(pos, 1.0);
    gl_FragData[1] = vec4(vel, 0.0);
}