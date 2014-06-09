#extension GL_EXT_gpu_shader4 : require

// phong shader code pilfered from http://www.ozone3d.net/tutorials/glsl_lighting_phong.php

// we need these for Phong shading

varying vec3 normal, lightDir, eyeVec;
//jason added
varying vec4 globalColor;


// this is where the magic happens. notice 'flat' before varying
// and the compiler instruction for the extension at the top of this file.

flat varying vec3  flatNormal;

// these switches set through oF

uniform float shouldRenderNormals;
uniform float shouldUseFlatShading;


void main( void )
{
	normal = gl_NormalMatrix * gl_Normal;
	flatNormal = gl_NormalMatrix * gl_Normal;
	
	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	lightDir = vec3(gl_LightSource[0].position.xyz - vVertex);
	eyeVec = -vVertex;
    
    //jason added
    globalColor = gl_Color;
	
	gl_Position = ftransform();
}