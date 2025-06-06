
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
uniform float forceGeoRetraction;

uniform vec3 actuatorDirection;
uniform float maxActuatorRetract;

varying vec3 normal;
varying float positionValid;


varying float headPositionAttenuation;
varying float actuatorExtendAttenuate;
varying float edgeAttenuate;

varying vec4 normalcolor;
varying vec4 depthcolor;

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

const vec2 halfvec = vec2(.5,.5);
float depthValueFromSample( vec2 depthPos){

	depthPos.x = clamp(depthPos.x,depthRect.x+1.,depthRect.x+depthRect.z-2.);
	depthPos.y = clamp(depthPos.y,depthRect.y+1.,depthRect.y+depthRect.w-2.);
	

	vec3 hsl = rgb2hsl( texture2DRect(rgbdTexture, floor(depthPos) + halfvec ).xyz );
    float depth = hsl.r;
	if(hsl.g < .5 || hsl.b < .02){
		depth = 0;	
	}
    return depth * ( maxDepth - minDepth ) + minDepth;
}

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return clamp( (value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin, outputMin, outputMax);
}

//ENCODING:
//gl_Position (x,y)  -> texture coord (center)
//gl_TexCoords (x,y) -> offset extend to base position
//

void main(void){
	
	
	vec2 samplePos = gl_Vertex.xy;
	// Reconstruct the 3D point position at the base point
	float baseDepth = depthValueFromSample( samplePos + depthRect.xy );
    vec4 basePos = vec4((samplePos.x - depthPP.x) * baseDepth / depthFOV.x,
						(samplePos.y - depthPP.y) * baseDepth / depthFOV.y,
						baseDepth, 1.0);

    

	//darken near the bottom of the frame
    //old edge way
//	edgeAttenuate = (1.0 - max( 0.0, pow( samplePos.y / depthRect.w, edgeAttenuateExponent) + edgeAttenuateBase ));
//	//but allow parts closer in z to get bright still
//	edgeAttenuate += (1.0 - edgeAttenuate) * pow(map(basePos.z,maxDepth,minDepth,0.0,1.0), 4.);

    //new edge way
    edgeAttenuate = 1.0 - smoothstep(edgeAttenuateBase, 1.0, map(samplePos.y, 0.0, depthRect.w, 0.0, 1.0) );
    
	//get the attenutation coefficient for this position
	headPositionAttenuation = map( distance(basePos.xyz,headPosition), headMinRadius+headFalloff, headMinRadius, .0, 1.0);
	
	//float headRetraction = mix(edgeAttenuate, 1.0, meshRetractionFalloff);
	float headRetraction = pow(map(headPositionAttenuation, 0.0, meshRetractionFalloff, 0.0, 1.0), 2.0) * (1.0-forceGeoRetraction);
	
	vec2 normalPos = samplePos.xy + normalRect.xy;
	normalcolor = texture2DRect(rgbdTexture, floor(normalPos) + halfvec);
	vec3 surfaceNormal = normalcolor.xyz * 2.0 - 1.0;
    normal = -normalize(gl_NormalMatrix * surfaceNormal);
    
    //actuatorExtendAttenuate = max(maxActuatorRetract, dot(normal,actuatorDirection) );
    actuatorExtendAttenuate = max(maxActuatorRetract, dot(surfaceNormal,actuatorDirection) );
    
    float accumulatedExtendAttenuation = triangleExtend * headRetraction * actuatorExtendAttenuate;
	vec2 samplePosExtended = samplePos + gl_Normal.xy * accumulatedExtendAttenuation;
	
    vec2 depthPos = samplePosExtended + depthRect.xy;
    float depth = depthValueFromSample( depthPos );
    depthcolor  = texture2DRect(rgbdTexture, floor(depthPos) + halfvec);

	// Reconstruct the 3D point position at the extended point
    vec4 pos = vec4((samplePosExtended.x - depthPP.x) * depth / depthFOV.x,
                    (samplePosExtended.y - depthPP.y) * depth / depthFOV.y,
                    depth, 1.0);
    
    
    float neighborA = depthValueFromSample( depthRect.xy + samplePos + gl_Color.xy * depthRect.zw * accumulatedExtendAttenuation );
    float neighborB = depthValueFromSample( depthRect.xy + samplePos + gl_Color.zw * depthRect.zw * accumulatedExtendAttenuation );
	
	positionValid = (depth < farClip &&
					 neighborA < farClip &&
					 neighborB < farClip &&

                     abs(neighborA - depth) < edgeClip &&
					 abs(neighborB - depth) < edgeClip &&
                     
					 depth > nearClip &&
					 neighborA > nearClip &&
					 neighborB > nearClip ) ? 1.0 : 0.0;
	
	
    // http://opencv.willowgarage.com/documentation/camera_calibration_and_3d_reconstruction.html
    //
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
		
		gl_TexCoord[0].xy = clamp(uv,vec2(0.0), colorRect.zw*colorScale);
	}

	
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
//    gl_FrontColor = gl_Color;
}
