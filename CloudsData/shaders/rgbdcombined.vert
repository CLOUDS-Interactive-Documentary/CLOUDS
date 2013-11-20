
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
uniform float isMeshed;
uniform vec2  simplify;
uniform float flowPosition;
uniform float farClip;
uniform float nearClip;
uniform float edgeClip;

uniform float minDepth;
uniform float maxDepth;

uniform float triangleContract;

uniform int useFaces;
uniform vec3 headPosition;

//FACE FEATURE
uniform vec4 faceFeatureRect;
uniform vec4 deltaChangeRect;

//uniform float sideAttenuate;
//uniform float bottomAttenuate;

varying float positionValid;
varying vec4 faceFeatureSample;
varying vec4 deltaChangeSample;

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
	
	float bottomAttenuate = 0.;
	float sideAttenuate   = 0.;
	// Here we get the position, and account for the vertex position flowing
	vec2 vertexPos = gl_Vertex.xy;


	
	vec2 samplePos = vec2(vertexPos.x, + mod(vertexPos.y + flowPosition, depthRect.w));
	samplePos = mix(samplePos, gl_Normal.xy, (triangleContract * isMeshed) ); // don't blend if we aren't discarding
	//edgeAttenuate = 1.0;
	
    vec2 depthPos = samplePos + depthRect.xy;
    float depth = depthValueFromSample( depthPos );
	
	//edgeAttenuate = 1.0;
	
	// Reconstruct the 3D point position
    vec4 pos = vec4((samplePos.x - depthPP.x) * depth / depthFOV.x,
                    (samplePos.y - depthPP.y) * depth / depthFOV.y,
                    depth, 1.0);
    
	//HEAD POSITION
	headPositionAttenuation = map(distance(pos.xyz,headPosition), 400, 50, 0.0, 1.0);
	
	//extract the normal and pass it along to the fragment shader
	
	vec2 normalPos = mix(samplePos.xy, gl_Normal.xy, isMeshed) + normalRect.xy;
//    normal = texture2DRect(texture, floor(normalPos) + vec2(.5,.5)).xyz * 2.0 - 1.0;
	vec4 normalColor = texture2DRect(rgbdTexture, floor(normalPos) + vec2(.5,.5));

	vec3 surfaceNormal = normalColor.xyz * 2.0 - 1.0;
    normal = -normalize(gl_NormalMatrix * surfaceNormal);
	vec3 vert = vec3(gl_ModelViewMatrix * pos);
	eye = normalize(-vert);
	
    float neighborA = depthValueFromSample( depthRect.xy + mix(gl_Color.xy*depthRect.zw,gl_Normal.xy,triangleContract) );
    float neighborB = depthValueFromSample( depthRect.xy + mix(gl_Color.zw*depthRect.zw,gl_Normal.xy,triangleContract) );
	
	if(isMeshed == 1.0){
		positionValid = (depth < farClip &&
						 neighborA < farClip &&
						 neighborB < farClip &&
    
						 depth > nearClip &&
						 neighborA > nearClip &&
						 neighborB > nearClip &&
						 
						 abs(neighborA - depth) < edgeClip &&
						 abs(neighborB - depth) < edgeClip
						) ? 1.0 : 0.0;
	}
	else {
		positionValid = (depth < farClip && depth > nearClip) ? 1.0 : 0.0;
	}
	
	
	edgeAttenuate = (1.0 - max( 0.0, pow( abs(320. - samplePos.x) / 320., 1.5) - sideAttenuate) ) *
					(1.0 - max( 0.0, pow( samplePos.y / 480., 2.0) + bottomAttenuate ));
	
	edgeAttenuate += (1. - edgeAttenuate) * pow(map(pos.z,maxDepth,minDepth,0.0,1.0),2.);

	//positionValid = 1.0;
	
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

        //gl_TexCoord[0].xy = ((uv-textureSize/2.0) * scale) + textureSize/2.0; 
		gl_TexCoord[0].xy = clamp(uv,vec2(0.0,0.0), colorRect.zw*colorScale);
	}
	
	//DIFFUSE LIGHT
	vec3 diffuseLightDirectionFull = vec3(lightPosition.xyz - vert);
    float d = length(diffuseLightDirectionFull);
	diffuseAttenuate = 1.0 /(gl_LightSource[0].constantAttenuation  +
							 gl_LightSource[0].linearAttenuation	* d +
							 gl_LightSource[0].quadraticAttenuation * d * d);
	
	diffuseLightDirection = diffuseLightDirectionFull / d;
	
	// now that we have the texture coordinate we can sample the face feature and movement map which correlate to the video texture
	if(useFaces == 1){
		vec2 faceFeatureScale = faceFeatureRect.zw / colorRect.zw / colorScale;
		vec2 faceFeaturePos = faceFeatureRect.xy + gl_TexCoord[0].xy * faceFeatureScale;
		faceFeatureSample = texture2DRect(rgbdTexture, faceFeaturePos);
		
		//extract the delta video change
		vec2 deltaChangeScale = deltaChangeRect.zw / colorRect.zw / colorScale;
		vec2 deltaChangePos = deltaChangeRect.xy + gl_TexCoord[0].xy * deltaChangeScale;
		deltaChangeSample = texture2DRect(rgbdTexture, deltaChangePos);
	}
	else {
		faceFeatureSample = vec4(0.);
		deltaChangeSample = vec4(0.);
	}
	
	forceFade = max(isMeshed, 1.0 - triangleContract);
	
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
    gl_FrontColor = vec4(1.0);
}
