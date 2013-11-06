#version 120
#extension GL_ARB_texture_rectangle : enable

#define kMaxAttractors 500

uniform vec2 attractors[kMaxAttractors];
uniform int numAttractors;

uniform float factor;
uniform float maxDist;

uniform sampler2DRect posData;
uniform float timestep;

void main()
{
    // Get the current vertex position.
    vec2 st = gl_TexCoord[0].st;
    vec2 pos = texture2DRect(posData, st).xy; 
    
    // Apply all the attractor forces to it.
    for (int i = 0; i < numAttractors; i++) {
    	vec2 att = attractors[i];
    	float dist = distance(att, pos);
    	if (dist < maxDist) {
    		vec2 to = vec2(pos.x - att.x, pos.y - att.y);
    		float df = length(to);
    		to = normalize(to) * 0.00000001;
    		to.x *= factor / df;
    		to.y *= factor / df;
    		pos.x += to.x * timestep;
    		pos.y += to.y * timestep;
    	}
    }

    // Store the new position in the destination FBO.
    gl_FragColor.rgba = vec4(pos.x, pos.y, 1.0, 1.0);
}
