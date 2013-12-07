#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect posData;
uniform sampler2DRect velData;
uniform float timestep;
uniform float size; 

varying vec2 texcoord;	

void main()
{    
    vec3 pos =  texture2DRect( posData,  texcoord ).xyz;
    vec3 vel =  texture2DRect( velData,  texcoord ).xyz;
    pos += vel * timestep; 
    if(pos.z < -size)
    {
    	pos.z = -size; 
    }
   	gl_FragColor = vec4(pos, 1.0);      	
}