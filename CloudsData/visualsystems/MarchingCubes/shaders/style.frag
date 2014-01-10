uniform bool bRenderNormals;
uniform vec4 fgColor;

varying vec3 normal;
varying vec3 ecPosition3;
varying vec4 ambientGlobal;
const float LOG2 = 1.442695;

varying float camDelta;
uniform vec4 fogColor;
uniform float fogDist;
uniform float fogExpo;
uniform float bUseGrain;
uniform float grainAmnt;
uniform float bUseColorGrain;


varying vec3 norm;

void PointLight(in int i,
                in vec3 eye,
                in vec3 ecPosition3,
                in vec3 normal,
                inout vec4 ambient,
                inout vec4 diffuse,
                inout vec4 specular,
				inout float attenuation )
{
    float nDotVP;         // normal . light direction
    float nDotHV;         // normal . light half vector
    float pf;             // power factor
    float d;              // distance from surface to light source
    vec3  VP;             // direction from surface to light position
    vec3  halfVector;     // direction of maximum highlights
    
    // Compute vector from surface to light position
    VP = vec3( gl_LightSource[0].position ) - ecPosition3;
    
    // Compute distance between surface and light position
    d = length( VP );
    
    // Normalize the vector from surface to light position
    VP = normalize( VP );
    
    // Compute attenuation
    attenuation = 1.0 / ( gl_LightSource[0].constantAttenuation +
                         gl_LightSource[0].linearAttenuation * d +
                         gl_LightSource[0].quadraticAttenuation * d * d );
    
    halfVector = normalize( VP + eye );
	
	nDotVP = max( 0.0, dot( normal, VP ) );
    nDotHV = max( 0.0, dot( normal, halfVector ) );
    
    if (nDotVP == 0.0)
        pf = 0.0;
    else
        pf = pow( nDotVP, gl_FrontMaterial.shininess );
    
    ambient += gl_LightSource[0].ambient * attenuation;
    diffuse += gl_LightSource[0].diffuse * nDotVP * attenuation;
	specular += gl_LightSource[0].specular * pf * attenuation;
}

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax){
    return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

float sampleBrightness(vec3 color){
	vec3 grayWeight = vec3(0.3, 0.59, 0.11);
	return dot(grayWeight, color);
}

vec4 rand(vec2 A,vec2 B,vec2 C,vec2 D){
	vec2 s=vec2(12.9898,78.233);
	vec4 tmp=vec4(dot(A,s),dot(B,s),dot(C,s),dot(D,s));
	return fract(sin(tmp) * 43758.5453)* 2.0 - 1.0;
}

void main(void)
{
    
    
    vec4 color = ambientGlobal;
	vec3 eye = normalize( -ecPosition3 );
	
	// Clear the light intensity accumulators
	vec4 amb  = vec4( 0.0 );
	vec4 diff = vec4( 0.0 );
	vec4 spec = vec4( 0.0 );
	
	float ppDiffuse			= abs( dot( normal, eye ) );
	float ppFresnell		= 1.0 - ppDiffuse;
	
	float atten = 0.0;
	
	for( int i=0; i<1; i++ )
	{
		PointLight( i, eye, ecPosition3, normalize( normal ), amb, diff, spec, atten );
	}
	
	color += amb * gl_FrontMaterial.ambient +
    diff * gl_FrontMaterial.diffuse;

    color += spec * gl_FrontMaterial.specular;
	
	color += atten * gl_FrontMaterial.emission * ppFresnell * 1.0;
	color += pow( atten, 3.0 ) * gl_FrontMaterial.emission;
    
	if (bRenderNormals) {
		gl_FragColor = vec4(norm, 1.0);	
	}
	else {
        
        if(bUseGrain == 1.){
            
            vec4 noiseGrain = rand(gl_TexCoord[0].st, color.xy, eye.st, normal.yz);
            
            if(bUseColorGrain == 0.) noiseGrain = vec4(vec3(clamp(sampleBrightness(noiseGrain.xyz),0.,1.)), 1.);
            
            color  = mix(color, noiseGrain, grainAmnt);
        }
        
        gl_FragColor = color * fgColor;
        gl_FragColor = mix( gl_FragColor, fogColor, min(1., pow( 1.25 * camDelta / (fogDist*fogDist), fogExpo) ) );;
	}
}
