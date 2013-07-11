
#version 120
#extension GL_ARB_texture_rectangle : enable

//This shader takes a combined video / depth / normal texture and projects it onto the given geometry
//The video files are generated using the RGBDCombinedVideoExporter

//TEXTURE INFORMATION
//
uniform sampler2DRect texture;
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
uniform float flowPosition;
uniform float farClip;
uniform float nearClip;
uniform float edgeClip;

uniform float minDepth;
uniform float maxDepth;

uniform int useFaces;
//FACE FEATURE
uniform vec4 faceFeatureRect;
uniform vec4 deltaChangeRect;

varying float positionValid;
varying vec4 faceFeatureSample;
varying vec4 deltaChangeSample;

//LIGHT
varying vec3 eye;
varying vec3 normal;
varying vec3 diffuseLightDirection;
varying float diffuseAttenuate;

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
    vec2  halfvec = vec2(.5,.5);
    float depth = rgb2hsl( texture2DRect(texture, floor(depthPos) + halfvec ).xyz ).r;
    return depth * ( maxDepth - minDepth ) + minDepth;
}

void main(void){
	
	// Here we get the position, and account for the vertex position flowing
	vec2 samplePos = vec2(gl_Vertex.x, + mod(gl_Vertex.y + flowPosition, depthRect.w));
    vec2 depthPos = samplePos + depthRect.xy;
    float depth = depthValueFromSample( depthPos );
	
	// Reconstruct the 3D point position
    vec4 pos = vec4((samplePos.x - depthPP.x) * depth / depthFOV.x,
                    (samplePos.y - depthPP.y) * depth / depthFOV.y,
                    depth,
                    1.0);
    
	//extract the normal and pass it along to the fragment shader
    vec2  normalPos = samplePos + normalRect.xy;
//    normal = texture2DRect(texture, floor(normalPos) + vec2(.5,.5)).xyz * 2.0 - 1.0;
	vec4 normalColor = texture2DRect(texture, floor(normalPos) + vec2(.5,.5));
	vec3 surfaceNormal = normalColor.xyz * 2.0 - 1.0;
    normal = -normalize(gl_NormalMatrix * surfaceNormal);
	vec3 vert = vec3(gl_ModelViewMatrix * pos);
	eye = normalize(-vert);
	
    float right = depthValueFromSample( depthPos + vec2(simplify.x,0.0)  );
    float down  = depthValueFromSample( depthPos + vec2(0.0,simplify.y)  );
    float left  = depthValueFromSample( depthPos + vec2(-simplify.x,0.0) );
    float up    = depthValueFromSample( depthPos + vec2(0.0,-simplify.y) );
    float bl    = depthValueFromSample( vec2(floor(depthPos.x - simplify.x),floor( depthPos.y + simplify.y)) );
    float ur    = depthValueFromSample( vec2(floor(depthPos.x  + simplify.x),floor(depthPos.y - simplify.y)) );
    
    positionValid = (depth < farClip &&
                        right < farClip &&
                        down < farClip &&
                        left < farClip &&
                        up < farClip &&
                        bl < farClip &&
                        ur < farClip &&
                        
                        depth > nearClip &&
                        right > nearClip &&
                        down > nearClip &&
                        left > nearClip &&
                        up > nearClip &&
                        bl > nearClip &&
                        ur > nearClip &&
                        
                        abs(down - depth) < edgeClip &&
                        abs(right - depth) < edgeClip &&
                        abs(up - depth) < edgeClip &&
                        abs(left - depth) < edgeClip &&
                        abs(ur - depth) < edgeClip &&
                        abs(bl - depth) < edgeClip
						) ? 1.0 : 0.0;
    
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
		gl_TexCoord[0].xy = clamp(uv,vec2(0.0,0.0), colorRect.zw);
	}
	
	//DIFFUSE LIGHT
	vec3 diffuseLightDirectionFull = vec3(gl_LightSource[0].position.xyz - vert);
    float d = length(diffuseLightDirectionFull);
	diffuseAttenuate = 1.0 /(gl_LightSource[0].constantAttenuation +
							 gl_LightSource[0].linearAttenuation	* d +
							 gl_LightSource[0].quadraticAttenuation * d * d);
	
	diffuseLightDirection = diffuseLightDirectionFull / d;
	
	// now that we have the texture coordinate we can sample the face feature and movement map which correlate to the video texture
	if(useFaces == 1){
		vec2 faceFeatureScale = faceFeatureRect.zw / colorRect.zw / colorScale;
		vec2 faceFeaturePos = faceFeatureRect.xy + gl_TexCoord[0].xy * faceFeatureScale;
		faceFeatureSample = texture2DRect(texture, faceFeaturePos);
		
		//extract the delta video change
		vec2 deltaChangeScale = deltaChangeRect.zw / colorRect.zw / colorScale;
		vec2 deltaChangePos = deltaChangeRect.xy + gl_TexCoord[0].xy * deltaChangeScale;
		deltaChangeSample = texture2DRect(texture, deltaChangePos);
	}
	else {
		faceFeatureSample = vec4(0.);
		deltaChangeSample = vec4(0.);
	}
		
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
    gl_FrontColor = gl_Color;
}
