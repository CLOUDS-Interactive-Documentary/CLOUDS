#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect velocity;
uniform sampler2DRect acceleration;
uniform float dT;
//uniform float velLimit;
//uniform float damping;

varying vec2 texcoord;	

void main()
{        
    vec3 vel =  texture2DRect( velocity,  texcoord ).xyz;
    vec3 acc =  texture2DRect( acceleration,  texcoord ).xyz;
    vel += acc * dT; 
    
//    if(length(vel) > velLimit)
//    {
//    	vel = normalize(vel)*velLimit; 
//    }
//    vel *= damping;

   	gl_FragColor = vec4(vel, 1.0);   	
}