#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect velData;
uniform sampler2DRect accData;
uniform float timestep;
uniform float velLimit; 
uniform float damping; 

varying vec2 texcoord;	

void main()
{        
    vec3 vel =  texture2DRect( velData,  texcoord ).xyz;
    vec3 acc =  texture2DRect( accData,  texcoord ).xyz;
    vel += acc * timestep; 
    
    if(length(vel) > velLimit)
    {
    	vel = normalize(vel)*velLimit; 
    }

    vel *= damping; 

   	gl_FragColor = vec4(vel, 1.0);   	
}