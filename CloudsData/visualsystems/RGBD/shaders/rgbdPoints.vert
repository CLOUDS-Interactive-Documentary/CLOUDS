
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
uniform float pointSizeMin;
uniform float pointSizeMax;

uniform vec3 headPosition;
uniform float headMinRadius;
uniform float headFalloff;
uniform float headOverlap;

uniform float edgeAttenuateBase;
uniform float edgeAttenuateExponent;
varying float positionValid;

//LIGHT
uniform vec3 lightPosition;

varying vec3 eye;
varying vec3 normal;
varying vec3 diffuseLightDirection;
varying float diffuseAttenuate;

varying float headPositionAttenuation;
varying float edgeAttenuate;
varying float forceFade;

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


void main(void){
	
	// Here we get the position, and account for the vertex position flowing
	vec2 vertexPos = gl_Vertex.xy;
	vec2 samplePos = vec2(vertexPos.x, + mod(vertexPos.y + flowPosition, depthRect.w));
	
    vec2 depthPos = samplePos + depthRect.xy;
    float depth = depthValueFromSample( depthPos );
	
	positionValid = depth <= nearClip || depth >= farClip ? 0.0 : 1.0;
	
	// Reconstruct the 3D point position
    vec4 pos = vec4((samplePos.x - depthPP.x) * depth / depthFOV.x,
                    (samplePos.y - depthPP.y) * depth / depthFOV.y,
                    depth, 1.0);
    
	//HEAD POSITION
	headPositionAttenuation = mix(0.0,
								  map(distance(pos.xyz,headPosition), headMinRadius+headFalloff, headMinRadius, .0, 1.0),
								  1.-headOverlap);
	
	//NORMAL
	vec2 normalPos = samplePos.xy + normalRect.xy;
	vec4 normalColor = texture2DRect(rgbdTexture, floor(normalPos) + vec2(.5,.5));
	vec3 surfaceNormal = normalColor.xyz * 2.0 - 1.0;
    normal = -normalize(gl_NormalMatrix * surfaceNormal);
	//EYE DIRECTION FOR LIGHTING
	vec3 vert = vec3(gl_ModelViewMatrix * pos);
	eye = normalize(-vert);
	
	//soften near the bottom edge
	edgeAttenuate = (1.0 - max( 0.0, pow( samplePos.y / depthRect.w, edgeAttenuateExponent) + edgeAttenuateBase ));
	//but allow parts closer in z to get bright still
	edgeAttenuate += (1.0 - edgeAttenuate) * pow(map(pos.z,maxDepth,minDepth,0.0,1.0), 4.);
	
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
		gl_TexCoord[0].xy = clamp(uv,vec2(0.0,0.0), colorRect.zw*colorScale);
	}
	
	//DIFFUSE LIGHT
	vec3 diffuseLightDirectionFull = vec3(lightPosition.xyz - vert);
    float d = length(diffuseLightDirectionFull);
	diffuseAttenuate = 1.0 /(gl_LightSource[0].constantAttenuation  +
							 gl_LightSource[0].linearAttenuation	* d +
							 gl_LightSource[0].quadraticAttenuation * d * d);
	
	diffuseLightDirection = diffuseLightDirectionFull / d;
		
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
	gl_PointSize = mix(pointSizeMin, pointSizeMax, 1.0-headPositionAttenuation);
	
}
