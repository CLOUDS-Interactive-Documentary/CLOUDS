
#version 120
#extension GL_ARB_texture_rectangle : enable

//This shader takes a combined video / depth / normal texture and projects it onto the given geometry
//The video files are generated using the RGBDCombinedVideoExporter

//TEXTURE INFORMATION
//
uniform sampler2DRect rgbdTexture;
uniform vec2 textureSize;

//COLOR
uniform vec4 colorRect;
uniform vec2 colorScale;

uniform vec2 colorFOV;
uniform vec2 colorPP;
uniform vec3 dK;
uniform vec2 dP;

uniform mat4 extrinsics;
uniform vec2 scale;

//DEPTH 
uniform vec4 depthRect;
uniform vec2 depthPP;
uniform vec2 depthFOV;

//NORMAL
uniform vec4 normalRect;

//GEOMETRY
uniform vec2  simplify;
uniform float farClip;
uniform float nearClip;
uniform float edgeClip;

uniform float minDepth;
uniform float maxDepth;

uniform float triangleExtend;
uniform vec3 headPosition;
uniform float headMinRadius;
uniform float headFalloff;

uniform float edgeAttenuateBase;
uniform float edgeAttenuateExponent;
uniform float meshRetractionFalloff;
//uniform float forceGeoRectraction;

varying float positionValid;

const float epsilon = 1e-6;

vec3 rgb2hsl( vec3 _input ){
	float h = 0.0;
	float s = 0.0;
	float l = 0.0;
	float r = _input.r;
	float g = _input.g;
	float b = _input.b;
	float cMin = min( r, min( g, b ) );
	float cMax = max( r, max( g, b ) );
	
	l = ( cMax + cMin ) / 2.0;
	if ( cMax > cMin ) {
		float cDelta = cMax - cMin;
        
		s = l < .05 ? cDelta / ( cMax + cMin ) : cDelta / ( 2.0 - ( cMax + cMin ) );

		// hue
		if ( r == cMax ) {
			h = ( g - b ) / cDelta;
		} else if ( g == cMax ) {
			h = 2.0 + ( b - r ) / cDelta;
		} else {
			h = 4.0 + ( r - g ) / cDelta;
		}
		
		if ( h < 0.0) {
			h += 6.0;
		}
		h = h / 6.0;
	}
	return vec3( h, s, l );
}

float depthValueFromSample( vec2 depthPos){

	depthPos.x = clamp(depthPos.x,depthRect.x+1.,depthRect.x+depthRect.z-2.);
	depthPos.y = clamp(depthPos.y,depthRect.y+1.,depthRect.y+depthRect.w-2.);
	
	vec2  halfvec = vec2(.5,.5);
	
    float depth = rgb2hsl( texture2DRect(rgbdTexture, floor(depthPos) + halfvec ).xyz ).r;
    return depth * ( maxDepth - minDepth ) + minDepth;
}

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return clamp( (value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin, outputMin, outputMax);
}

//ENCODING:
//gl_Position (x,y)  -> texture coord (center)
//gl_TexCoords (x,y) -> offset extend to base position

void main(void){
	
	
	vec2 samplePos = gl_Vertex.xy;
	// Reconstruct the 3D point position at the base point
	float baseDepth = depthValueFromSample( samplePos + depthRect.xy );
    vec4 basePos = vec4((samplePos.x - depthPP.x) * baseDepth / depthFOV.x,
						(samplePos.y - depthPP.y) * baseDepth / depthFOV.y,
						baseDepth, 1.0);

	//get the attenutation coefficient for this position
	float headPositionAttenuation = map( distance(basePos.xyz,headPosition), headMinRadius+headFalloff, headMinRadius, .0, 1.0);
	
	float headRetraction = pow(map(headPositionAttenuation, 0.0, meshRetractionFalloff, 0.0, 1.0), 2.0);// * (1.0-forceGeoRectraction);
	
	vec2 normalPos = samplePos.xy + normalRect.xy;
	vec4 normalColor = texture2DRect(rgbdTexture, floor(normalPos) + vec2(.5,.5));
    
    float accumulatedExtendAttenuation = triangleExtend * headRetraction;
	vec2 samplePosExtended = samplePos + gl_Normal.xy * accumulatedExtendAttenuation;
	
    vec2 depthPos = samplePosExtended + depthRect.xy;
    float depth = depthValueFromSample( depthPos );
	
	// Reconstruct the 3D point position at the extended point
    vec4 pos = vec4((samplePosExtended.x - depthPP.x) * depth / depthFOV.x,
                    (samplePosExtended.y - depthPP.y) * depth / depthFOV.y,
                    depth, 1.0);
    
    float neighborA = depthValueFromSample( depthRect.xy + samplePos + gl_Color.xy * depthRect.zw * accumulatedExtendAttenuation );
    float neighborB = depthValueFromSample( depthRect.xy + samplePos + gl_Color.zw * depthRect.zw * accumulatedExtendAttenuation );
	
	positionValid = (depth < farClip &&
					 neighborA < farClip &&
					 neighborB < farClip &&

					 depth > nearClip &&
					 neighborA > nearClip &&
					 neighborB > nearClip &&
					 
					 abs(neighborA - depth) < edgeClip &&
					 abs(neighborB - depth) < edgeClip) ? 1.0 : 0.0;
	
	

    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
}
