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

varying float camDelta;
uniform vec4 fogColor;
uniform float fogDist;
uniform float fogExpo;
uniform float texMix;

uniform sampler2DRect drawMap;

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
    
	color += spec * gl_FrontMaterial.specular;
	
	color += atten * gl_FrontMaterial.emission * ppFresnell * 1.0;
	color += pow( atten, 3.0 ) * gl_FrontMaterial.emission;
	
    color *= mix( highColor*balance, lowColor*(1.-balance) ,fsColor.r);
    
    vec4 tc = texture2DRect(drawMap, gl_TexCoord[0].st);
    
    gl_FragColor = color * mix(vec4(1.,1.,1.,1.), tc*2. ,texMix);
	gl_FragColor = mix( gl_FragColor, fogColor, min(1., pow( 1.25 * camDelta / (fogDist*fogDist), fogExpo) ) );;
}



