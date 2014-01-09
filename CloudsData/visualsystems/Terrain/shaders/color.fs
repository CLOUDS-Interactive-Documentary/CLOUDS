int NumEnabledLights = 1;

varying vec4 fsColor;

varying vec3 normal;
varying vec3 ecPosition3;
varying vec4 ambientGlobal;
const float LOG2 = 1.442695;

uniform vec4 highColor;
uniform vec4 lowColor;
uniform float _atten;
uniform float balance;
uniform vec4 traceColor;

varying float camDelta;
uniform vec4 fogColor;
uniform float fogDist;
uniform float fogExpo;
uniform float texMix;

uniform float doDraw;

uniform sampler2DRect drawMap;
uniform sampler2DRect noiseMap;

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


void main()
{
	vec4 color = ambientGlobal;
	vec3 eye = normalize( -ecPosition3 );
	
	// Clear the light intensity accumulators
	vec4 amb  = vec4( 0.0 );
	vec4 diff = vec4( 0.0 );
	vec4 spec = vec4( 0.0 );
	
	float ppDiffuse			= abs( dot( normal, eye ) );
	float ppFresnell		= 1.0 - ppDiffuse;
	
	float atten = _atten;
	
	for( int i=0; i<NumEnabledLights; i++ )
	{
		PointLight( i, eye, ecPosition3, normalize( normal ), amb, diff, spec, atten );
	}
	
	color += amb * gl_FrontMaterial.ambient +
    diff * gl_FrontMaterial.diffuse;
    
    if(doDraw==0.){
        vec4 noise = texture2DRect(noiseMap, gl_TexCoord[0].st);
        color = mix(color,
                    vec4(
                         vec3(
                              sampleBrightness(rand(gl_TexCoord[0].st,
                                                    gl_TexCoord[0].yx,
                                                    noise.rg,
                                                    noise.ba
                                                    ).rgb
                                               )
                              )
                         ,1.)
                    ,.05
                    );
        color = mix(color,vec4(vec3(sampleBrightness(fsColor.rgb)),1.),.05);
    }
    
	color += spec * gl_FrontMaterial.specular;
	
	color += atten * gl_FrontMaterial.emission * ppFresnell * 1.0;
	color += pow( atten, 3.0 ) * gl_FrontMaterial.emission;
    
    color *= mix( highColor*balance, lowColor*(1.-balance) ,fsColor.r);
    
    if(doDraw==1.){
        vec4 tc = texture2DRect(drawMap, gl_TexCoord[0].st);
        color += mix(vec4(0.),traceColor, clamp(map(tc.r,0.2,1.,0.,1.),0.,1.) );
       // color.g -= mix(traceColor.g ,1., clamp(map(tc.r,0.2,1.,0.,1.),0.,1.) );
       // color.b -= mix(traceColor.b ,1., clamp(map(tc.r,0.2,1.,0.,1.),0.,1.) );
    }
    
    gl_FragColor = color;
	gl_FragColor = mix( gl_FragColor, fogColor, min(1., pow( 1.25 * camDelta / (fogDist*fogDist), fogExpo) ) );;
   // gl_FragColor = fsColor;
}



