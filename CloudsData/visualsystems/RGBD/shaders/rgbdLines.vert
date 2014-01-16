
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
uniform float flowPosition;
uniform float farClip;
uniform float nearClip;
uniform float edgeClip;

uniform float minDepth;
uniform float maxDepth;

//distance between each adjoining line so we can tell to cull them
uniform float lineExtend; //0. = 1. value that extend the lines out to their base positions

uniform vec3 headPosition;
uniform float headMinRadius;
uniform float headFalloff;
uniform float headOverlap;

uniform float edgeAttenuateBase;
uniform float edgeAttenuateExponent;

//FACE FEATURE
varying float positionValid;

//LIGHT
uniform vec3 actuatorDirection;
uniform float maxActuatorRetract;

varying vec3 normal;
varying float actuatorAttenuation;
varying float headPositionAttenuation;
varying float edgeAttenuate;

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
//gl_Position (x,y)  -> center coordinate (center)
//gl_TexCoords (x,y) -> offset extend to base position
//

vec2 flowCoord(vec2 basePosition){
	return vec2(basePosition.x, + mod(basePosition.y + flowPosition, depthRect.w));
}

void main(void){
	
	// Here we get the position, and account for the vertex position flowing
	vec2 vertexPos = gl_Vertex.xy;
	vec2 samplePos = flowCoord(vertexPos);
	
	float baseDepth = depthValueFromSample( samplePos + depthRect.xy );
    vec4 basePos = vec4((samplePos.x - depthPP.x) * baseDepth / depthFOV.x,
						(samplePos.y - depthPP.y) * baseDepth / depthFOV.y,
						baseDepth, 1.0);

	//soften near the bottom edge
//	edgeAttenuate = (1.0 - max( 0.0, pow( samplePos.y / depthRect.w, edgeAttenuateExponent) + edgeAttenuateBase ));
//	//but allow parts closer in z to get bright still
//	edgeAttenuate += (1.0 - edgeAttenuate) * pow(map(basePos.z,maxDepth,minDepth,0.0,1.0), 4.);
//    edgeAttenuate = smoothstep();
    
    edgeAttenuate = 1.0 - smoothstep(edgeAttenuateBase, 1.0, map(samplePos.y, 0.0, depthRect.w, 0.0, 1.0) );
    
	//extract the normal
	vec2 normalPos   = samplePos + normalRect.xy;
	vec4 normalColor = texture2DRect(rgbdTexture, floor(normalPos) + vec2(.5,.5));
  	vec3 surfaceNormal = normalColor.xyz * 2.0 - 1.0;
//    normal = -normalize(gl_NormalMatrix * surfaceNormal);
  
//    float actuatorExtendAttenuate = smoothstep(.3, .35, dot(normal,actuatorDirection) );
    actuatorAttenuation = max(maxActuatorRetract, dot(surfaceNormal,actuatorDirection));

    
	vec2 extendedSamplePos = samplePos + gl_Normal.xy*lineExtend*actuatorAttenuation;
	vec2 neighborSamplePos = samplePos - gl_Normal.xy*lineExtend*actuatorAttenuation;
	
	//edgeAttenuate = 1.0;
    vec2 depthPos = extendedSamplePos + depthRect.xy;
    float depth = depthValueFromSample( depthPos );
	float neighborDepth = depthValueFromSample( neighborSamplePos + depthRect.xy );
	
	// Reconstruct the 3D point position
    vec4 pos = vec4((extendedSamplePos.x - depthPP.x) * depth / depthFOV.x,
                    (extendedSamplePos.y - depthPP.y) * depth / depthFOV.y,
                    depth, 1.0);
    
	//attenuate the lines away from the face, mixing them back in a bit
	headPositionAttenuation = mix(0.0, map(distance(basePos.xyz,headPosition), headMinRadius+headFalloff, headMinRadius, .0, 1.0),
								  1.-headOverlap);
	

//	vec3 vert = vec3(gl_ModelViewMatrix * pos);
//	eye = normalize(-vert);

	positionValid = (depth < farClip &&
					 neighborDepth < farClip &&
					 depth > nearClip &&
					 neighborDepth > nearClip &&
					 abs(neighborDepth - depth) < edgeClip) ? 1.0 : 0.0;

    vec4 projection = extrinsics * pos;
    if(projection.z != 0.0) {
        vec2 xyp = projection.xy / projection.z;
        float r2 = pow(xyp.x, 2.0) + pow(xyp.y, 2.0);
        float r4 = r2*r2;
        float r6 = r4*r2;
        vec2 xypp = xyp;
        xypp.x = xyp.x * (1.0 + dK.x*r2 + dK.y*r4 + dK.z*r6) + 2.0*dP.x * xyp.x * xyp.y + dP.y*(r2 + 2.0 * pow(xyp.x,2.0) );
        xypp.y = xyp.y * (1.0 + dK.x*r2 + dK.y*r4 + dK.z*r6) + dP.x * (r2 + 2.0*pow(xyp.y, 2.0) ) + 2.0*dP.y*xyp.x*xyp.y;
        vec2 uv = (colorFOV * xypp + colorPP) * colorScale;
		gl_TexCoord[0].xy = clamp(uv,vec2(0.0), colorRect.zw * colorScale);
	}
	
//	//DIFFUSE LIGHT
//	vec3 diffuseLightDirectionFull = vec3(lightPosition.xyz - vert);
//    float d = length(diffuseLightDirectionFull);
//	diffuseAttenuate = 1.0 /(gl_LightSource[0].constantAttenuation  +
//							 gl_LightSource[0].linearAttenuation	* d +
//							 gl_LightSource[0].quadraticAttenuation * d * d);
//	diffuseLightDirection = diffuseLightDirectionFull / d;
		
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
//    gl_FrontColor = gl_Color;
}
