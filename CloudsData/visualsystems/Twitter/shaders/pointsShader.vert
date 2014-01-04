
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

float map(float value, float inputMin, float  inputMax, float  outputMin,float  outputMax ){
    
    float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
	return clamp(outVal,outputMin,outputMax);
}

void main(void){
//    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
//	gl_FrontColor = gl_Color;
//	//gl_PointSize = 1.0 + (gl_Normal.y * pointSize);
//	gl_PointSize = gl_Normal.y*pointSize;

//    gl_PointSize = map(gl_Normal.x,0.,1.,5.,15.) ;
	gl_PointSize = map(gl_Normal.x,0.,1.,minSize    ,maxSize) ;
//    activityValue = 1.;
    vec4 mixed = mix(spriteBaseColor, spritePopColor, gl_Normal.y)  ;

    //gl_PointSize = gl_Normal.x *sizeMultiplier;//7.0;
    gl_FrontColor = mixed ;
//    gl_FrontColor.rgb =vec4( 1.0,1.0,1.0,1.0);
//    gl_FrontColor.rgb =vec3( gl_Normal.y,gl_Normal.y,gl_Normal.y);
//	gl_FrontColor = vec4(1.0,0.0,1.0,1.0);
//    gl_FrontColor = spritePop;//*nearAttenuate;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}
