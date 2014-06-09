#extension GL_EXT_gpu_shader4 : require

// we need these for Phong shading

// phong shader code pilfered from http://www.ozone3d.net/tutorials/glsl_lighting_phong.php

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
	vec4 final_color =
	(gl_FrontLightModelProduct.sceneColor *  globalColor) +
	(gl_LightSource[0].ambient * globalColor);
	//gl_FrontMaterial.ambient
	vec3 selectedNormal;
	
	if (shouldUseFlatShading==1.0){
		selectedNormal = flatNormal;
	} else {
		selectedNormal = normal;
	}
	
	vec3 N = normalize(selectedNormal);
	vec3 L = normalize(lightDir);
	
	float lambertTerm = dot(N,L);
	
	if(lambertTerm > 0.0)
	{
		final_color += gl_LightSource[0].diffuse *
		gl_FrontMaterial.diffuse *
		lambertTerm;
		
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		float specular = pow( max(dot(R, E), 0.0),
		gl_FrontMaterial.shininess );
		final_color += gl_LightSource[0].specular *
		gl_FrontMaterial.specular *
		specular;
	}
	
	
	gl_FragColor = final_color;
	
	if (shouldRenderNormals == 1.0){
		gl_FragColor = vec4((N + vec3(1.0, 1.0, 1.0)) / 2.0,1.0);
	}

}