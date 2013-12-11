#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect accData;
uniform sampler2DRect electroData;

uniform float resolution; 	//64
uniform float limit; 

varying vec2 texcoord;	

void main()
{    
 	float electroRes = resolution*resolution; 
 	vec2 eCoord = texcoord*2.0;

	vec3 force = vec3(0.0); 

 // 	float idX = eCoord.x; 
	// float idY = eCoord.y; 

 // 	float x = 0.0; 
 // 	for(x = 0.0; x < idX; x++)
 // 	{
 // 		force += texture2DRect(electroData, vec2(x, idX)).xyz; 
 // 	} 

 // 	float y = idY; 
 // 	for(y; y < electroRes; y++)
 // 	{
 // 		force += texture2DRect(electroData, vec2(y, idX)).xyz; 
 // 	} 






















































 	float id = (texcoord.x + texcoord.y*resolution); 

	float y = id+1.; 
 	for(y; y < electroRes; y++)
 	{
 		force += texture2DRect( electroData, vec2(id, y) ).xyz; 
 	}	 	

 	float x = 0.0; 
 	for(x; x < id; x++)
 	{
 		force -= texture2DRect( electroData, vec2(x, id) ).xyz; 
 	}

	float m = length(force); 
	if(m > limit)
	{
		force = normalize(force)*limit; 
	}
	vec3 acc = texture2DRect( accData, texcoord ).xyz; 	
  	gl_FragColor = vec4(acc+force, 1.0); 	
}








