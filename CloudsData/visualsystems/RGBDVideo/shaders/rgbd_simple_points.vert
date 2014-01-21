
#version 120
#extension GL_ARB_texture_rectangle : enable

//This shader takes a combined video / depth / normal texture and projects it onto the given geometry
//The video files are generated using the RGBDCombinedVideoExporter

//TEXTURE INFORMATION
//
uniform sampler2DRect texture;

//DEPTH
uniform vec2 depthPP;
uniform vec2 depthFOV;

uniform float minDepth;
uniform float maxDepth;
uniform float scale;
uniform float offset;

uniform float pointoffset;

const float epsilon = 1e-6;

varying vec2 texSample;
varying float positionValid;

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
	vec3 hsl = rgb2hsl( texture2DRect(texture, floor(depthPos) + halfvec ).xyz );
    float depth = hsl.r;
	if(hsl.b > .2){
		return depth * ( maxDepth - minDepth ) + minDepth;
	}
	else {
		return 0.0;
	}
}

void main(void){
	
	// Here we get the position, and account for the vertex position flowing
	vec2 samplePos = vec2(gl_Vertex.x + offset, gl_Vertex.y + 480. * scale);
    float depth = depthValueFromSample( samplePos );

	texSample = vec2(gl_Vertex.x + offset, gl_Vertex.y);
	positionValid = depth > minDepth+10. && depth < maxDepth-10. ? 1.0 : 0.0;
	
	// Reconstruct the 3D point position
    vec4 pos = vec4((gl_Vertex.x - depthPP.x*scale) * depth / (depthFOV.x*scale),
                    (gl_Vertex.y - depthPP.y*scale) * depth / (depthFOV.y*scale),
                    depth, 1.0);
	
	pos.z += pointoffset;
	
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
    gl_FrontColor = gl_Color;
}
