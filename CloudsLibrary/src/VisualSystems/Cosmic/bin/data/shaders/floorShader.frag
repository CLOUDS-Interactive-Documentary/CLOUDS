#version 110
#extension GL_ARB_texture_rectangle : enable

uniform float size; 
uniform float shadowOpacity; 

varying vec2 texcoord;
varying float posZ; 

void main()
{	
	vec2 pos = vec2(texcoord/size); 

	float dist = distance(pos, vec2(.5, .5)); 
	dist = 1.0 - pow(exp(-dist*dist), 32.0); 

	vec4 newColor = vec4(dist, dist, dist, (1.0 - dist));     
	
	newColor.a *= (1.0 - posZ/size)*shadowOpacity; 

    gl_FragColor = newColor;     
}

