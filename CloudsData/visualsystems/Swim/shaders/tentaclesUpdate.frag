#version 120
#extension GL_ARB_texture_rectangle : enable

// ping pong inputs
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;

uniform float restLength;
uniform float elapsed;

void main()
{
    vec4 pos = texture2DRect(particles0, gl_TexCoord[0].st);
    vec4 vel = texture2DRect(particles1, gl_TexCoord[0].st);
    
    vec4 parentPos = texture2DRect(particles0, vec2(gl_TexCoord[0].s - 1.0, gl_TexCoord[0].t));
    
    vec3 springForceDirection = (parentPos - pos).xyz;
    
    float springConstant = 100.0;
    
    float magnitude = springConstant * (length(springForceDirection) - restLength);
    
    vec4 force = vec4(normalize(springForceDirection), 0.0) * magnitude;
    force += vec4(0.0, -10, 0.0, 0.0);
    
    vel += step(0.5, gl_TexCoord[0].s) * force * elapsed;
    vel *= 0.6;
    
    pos += step(0.5, gl_TexCoord[0].s) * vel * elapsed;
    
    gl_FragData[0] = pos;
    gl_FragData[1] = vel;
}