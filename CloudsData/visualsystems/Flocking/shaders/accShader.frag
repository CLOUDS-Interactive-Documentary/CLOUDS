#version 110
#extension GL_ARB_texture_rectangle : enable

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647693

uniform sampler2DRect posData;
uniform sampler2DRect velData;

uniform float resolution; 
uniform float time; 

//Pass in
uniform float zoneRadius; 
uniform float threshLow; 
uniform float threshHigh; 
uniform float repelForceConstant; 
uniform float alignForceConstant; 
uniform float attractForceConstant; 
uniform float cohesionForceConstant;
uniform float accLimit; 

uniform float centerForceConstant; 
varying vec2 texcoord;	

void main()
{    
    vec3 center = vec3(0.); 
	vec3 pos =  texture2DRect( posData,  texcoord ).xyz;    
    vec3 vel =  texture2DRect( velData,  texcoord ).xyz;   

    vec3 acc =  vec3(0.0, 0.0, 0.0);        	     

    acc -= (pos-center)*centerForceConstant; 
    
    float numNeighbors = 0.0; 
    vec3 neighborCentroid = vec3(0.); 
    vec3 flockCentroid = vec3(0.); 
    
    float i = 0.0; 
    float j = 0.0; 
    float total = resolution*resolution; 
    
    for(i = 0.0; i < resolution; i++)
    {
        for(j = 0.0; j < resolution; j++)
        {   
        	vec2 coord = vec2(i, j); 
            vec3 otherPos = texture2DRect( posData, coord ).xyz; 
            vec3 otherVel = texture2DRect( velData, coord ).xyz; 

            vec3 direction = otherPos - pos; 
            float dist = length(direction);            
            if(dist > 0.00001)
            {
                if(dist < zoneRadius)
                {                
                    float percent = dist/zoneRadius;
                    neighborCentroid +=otherPos;                     
                    numNeighbors+=1.0; 
                    if(percent < threshLow)        
                    {
                        float F = ( threshLow / percent - 1.0 ) * repelForceConstant;
                        direction = normalize(direction); 
                        direction*=F;
                        acc -= direction;                        
                    }
                    else if(percent < threshHigh)
                    {
                        float threshDelta = threshHigh - threshLow; 
                        float adjustedPercent = ( percent - threshLow ) / threshDelta;
                        float F = ( 0.5 - cos( adjustedPercent * PI * 2.0 ) * 0.5 + 0.5 ) * alignForceConstant;
                        acc += normalize(otherVel)*F;                    
                    }
                    else
                    {                    
                        float threshDelta = 1.0 - threshHigh; 
                        float adjustedPercent = ( percent - threshHigh ) / threshDelta;
                        float F = ( 1.0 - ( cos( adjustedPercent * TWO_PI ) * -0.5 + 0.5 ) ) * attractForceConstant; 
                        direction = normalize(direction); 
                        direction *= F;
                        acc += direction;                    
                    }                
                }
            }
            flockCentroid += otherPos;             
        }
    }

    if(numNeighbors > 0.0)
    { 
         vec3 neighborAveragePos = ( neighborCentroid / numNeighbors );
         acc += ( neighborAveragePos - pos ) * cohesionForceConstant;
    }
    else
    {
        flockCentroid = flockCentroid/(total - 1.0); 
        acc += ( flockCentroid - pos );   
    }
            
    if(length(acc) > accLimit)
    {
        acc = normalize(acc)*accLimit; 
    }

  	gl_FragColor = vec4(acc, 10.0*numNeighbors/total); 
}