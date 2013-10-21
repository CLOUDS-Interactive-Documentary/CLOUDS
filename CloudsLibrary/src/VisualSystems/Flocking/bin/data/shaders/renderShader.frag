varying vec2 texcoord;
varying vec4 a_color; 
varying vec4 color; 
	
void main()
{	
	float dist = distance(texcoord, vec2(.5, .5)); 
	dist = 1.0 - pow(exp(-dist*dist), 64.0); 	
    gl_FragColor = a_color*color*vec4(dist, dist, dist, (1.0 - dist));  
}