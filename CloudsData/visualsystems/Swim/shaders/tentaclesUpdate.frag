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
    vec3 childPos = texture2DRect(particles0, vec2(gl_TexCoord[0].s + 1.0, gl_TexCoord[0].t)).xyz;
    
    vec3 springForce0 = step(0.5, gl_TexCoord[0].s) * springForce(pos, parentPos);
    //vec3 springForce1 = step(gl_TexCoord[0].t, numSections - 1.5) * springForce(pos, childPos);
    
    vec3 oldVel = vel;
    
    vel += springForce0 * elapsed;
    
    vel *= 1.0 - (0.15 * elapsed * 60.0);
    
    // first particle stays locked to jelly
    pos += step(0.5, gl_TexCoord[0].s) * 0.5 * (vel + oldVel) * elapsed;
    
    gl_FragData[0] = vec4(pos, 1.0);
    gl_FragData[1] = vec4(vel, 0.0);
}