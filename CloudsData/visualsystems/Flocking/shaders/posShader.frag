#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect posData;
uniform sampler2DRect velData;
uniform float timestep;
uniform float border; 


varying vec2 texcoord;	

void main()
{    
    vec3 pos =  texture2DRect( posData,  texcoord ).xyz;
    vec3 vel =  texture2DRect( velData,  texcoord ).xyz;
    pos += vel * timestep; 
    
    if(pos.x > border)
    {
        pos.x = -border+1.;
    }
    else if(pos.x < -border)
    {   
        pos.x = border-1.;
    }

    if(pos.y > border)
    {
        pos.y = -border+1.;
    }
    else if(pos.y < -border)
    {
        pos.y = border-1.;
    }
    
    if(pos.z > border)
    {
        pos.z = -border+1.;
    }
    else if(pos.z < -border)
    {
        pos.z = border-1.;
    }

   	gl_FragColor = vec4(pos, 1.0);      	
}