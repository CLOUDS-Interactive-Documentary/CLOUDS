
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

uniform float edgeClip;

uniform vec3 pointoffset;
uniform vec2 simplify;
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
    
    depthPos = vec2(depthPos.x + offset, depthPos.y + 480. * scale);
    
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
    
    float depth = depthValueFromSample( gl_Vertex.xy );
    
    float right = depthValueFromSample(gl_Vertex.xy + vec2(simplify.x,0.0) );
    float down  = depthValueFromSample(gl_Vertex.xy + vec2(0.0,simplify.y) );
    float left  = depthValueFromSample(gl_Vertex.xy + vec2(-simplify.x,0.0));
    float up    = depthValueFromSample(gl_Vertex.xy + vec2(0.0,-simplify.y));
    float bl    = depthValueFromSample(vec2(gl_Vertex.x - simplify.x, gl_Vertex.y + simplify.y) );
    float ur    = depthValueFromSample(vec2(gl_Vertex.x  + simplify.x,gl_Vertex.y - simplify.y) );
    
	texSample = vec2(gl_Vertex.x + offset, gl_Vertex.y);
    
    
    positionValid = (depth < maxDepth &&
                        right < maxDepth &&
                        down < maxDepth &&
                        left < maxDepth &&
                        up   < maxDepth &&
                        bl   < maxDepth &&
                        ur   < maxDepth &&
                        
                        depth > minDepth &&
                        right > minDepth &&
                        down > minDepth &&
                        left > minDepth &&
                        up > minDepth &&
                        bl > minDepth &&
                        ur > minDepth &&
                        
                        abs(down - depth) < edgeClip &&
                        abs(right - depth) < edgeClip &&
                        abs(up - depth) < edgeClip &&
                        abs(left - depth) < edgeClip &&
                        abs(ur - depth) < edgeClip &&
                        abs(bl - depth) < edgeClip
						) ? 1.0 : 0.0;
	
	// Reconstruct the 3D point position
    vec4 pos = vec4((gl_Vertex.x - depthPP.x*scale) * depth / (depthFOV.x*scale),
                    (gl_Vertex.y - depthPP.y*scale) * depth / (depthFOV.y*scale),
                    depth, 1.0);

	pos.xyz += pointoffset;
	
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
    
    gl_FrontColor = gl_Color;
}
