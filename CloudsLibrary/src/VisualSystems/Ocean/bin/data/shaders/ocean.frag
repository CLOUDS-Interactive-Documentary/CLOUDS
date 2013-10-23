

uniform vec3 uAmbient;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uSpecIntensity;

uniform vec3 uSunPos;
uniform vec3 uEyePos;

varying float fogFactor;
varying vec3 eyeVec;
varying vec3 normal;

float calculateSpecular(){
	vec3 specularLightDirection = normalize(uSunPos);
	if(dot(normal, specularLightDirection) < 0.0){
		return 0.0;
	}
	return pow(max(0.0, dot(reflect(specularLightDirection, normal), eyeVec)), uSpecIntensity);
}

void main (void)
{
	
//    vec3 N = varEyeNormal;
    vec3 L = normalize(uSunPos);
//    vec3 H = normalize(L + uEyePos);	
//    float df = max(0.0, dot(N, L));
//    float sf = max(0.0, dot(N, H));
	
    //sf = pow(sf, uSpecIntensity);
	float sf = calculateSpecular();
	
    vec3 col = uAmbient + /*uDiffuse * df +*/ uSpecular * sf;
	
	gl_FragColor = vec4(col*gl_Color.xyz,fogFactor);
	//gl_FragColor = vec4(varEyeNormal*.5 + .5,fogFactor);
	//gl_FragColor = vec4(normal,fogFactor);
	//gl_FragColor = vec4(normalize(uSunPos), fogFactor);
}

