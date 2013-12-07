#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect posData;
uniform sampler2DRect accData;

varying vec2 texcoord;
varying vec4 color; 
varying vec4 a_color; 
uniform vec3 u_color; 

void main()
{
	texcoord = gl_MultiTexCoord0.st;
	vec4 pos = texture2DRect( posData, texcoord );	 
	vec4 acc = texture2DRect( accData, texcoord );	 
	color = vec4(u_color*acc.a, 1.0);
	a_color = vec4(acc*acc); 	
	a_color.a = 1.0; 
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;	
	gl_FrontColor = gl_Color; 
}
