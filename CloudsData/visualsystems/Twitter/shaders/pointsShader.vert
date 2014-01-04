
#version 120
#extension GL_ARB_texture_rectangle : enable

uniform float maxSize;
uniform float minSize;
//uniform float sizeMultiplier;
//uniform vec3 attractor;
//uniform vec4 spriteColor;
//varying float activityValue;
uniform vec4 spriteBaseColor;
uniform vec4 spritePopColor;
uniform bool animateSpriteSize;
float map(float value, float inputMin, float  inputMax, float  outputMin,float  outputMax ){
    
    float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
	return clamp(outVal,outputMin,outputMax);
}

void main(void){

    if(animateSpriteSize){
        gl_PointSize = map(gl_Normal.y,0.,1.,minSize,maxSize) ;
    }
    else{
        gl_PointSize = map(gl_Normal.x,0.,1.,minSize,maxSize) ;
    }
	

    vec4 mixed = mix(spriteBaseColor, spritePopColor, gl_Normal.y)  ;
    gl_FrontColor = mixed ;

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}
