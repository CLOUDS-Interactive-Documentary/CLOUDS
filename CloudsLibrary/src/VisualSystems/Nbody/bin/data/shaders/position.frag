#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect position;
uniform sampler2DRect velocity;
uniform float dT;
uniform float size; 

varying vec2 texcoord;	

void main()
{    
    vec3 pos =  texture2DRect( position,  texcoord ).xyz;
    vec3 vel =  texture2DRect( velocity,  texcoord ).xyz;
    pos += vel * dT; 
    //if(pos.z < -size)
    //{
    //	pos.z = -size;
    //}
   	gl_FragColor = vec4(pos, 1.0);      	
}