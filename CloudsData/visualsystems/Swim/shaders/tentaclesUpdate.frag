#version 120
#extension GL_ARB_texture_rectangle : enable

// ping pong inputs
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;

uniform float restLength;

void main()
{
    vec4 pos = texture2DRect(particles0, gl_TexCoord[0].st);
    vec4 vel = texture2DRect(particles1, gl_TexCoord[0].st);
    
    vec4 parentPos = texture2DRect(particles0, vec2(gl_TexCoord[0].s - 1.0, gl_TexCoord[0].t));
    
    vec3 springForceDirection = (parentPos - pos).xyz;
    float springConstant = 0.2;
    float restLength = 10.0;
    
    float magnitude = springConstant * (length(springForceDirection) - restLength);
    
    vec4 force = vec4(normalize(springForceDirection), 0.0) * magnitude;
    force += vec4(0.0, 0.8, 0.0, 0.0);
    
    vel += step(0.5, gl_TexCoord[0].s) * force;
    vel *= 0.5;
    
    pos += step(0.5, gl_TexCoord[0].s) * vel;
    
    gl_FragData[0] = pos;
    gl_FragData[1] = vel;
}