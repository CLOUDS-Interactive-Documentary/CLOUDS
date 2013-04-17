
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

uniform mat3 colorRotate;
uniform vec3 colorTranslate;

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

uniform vec2 shift;
uniform vec2 scale;

varying float VZPositionValid0;
varying vec3 normal;

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
        
		// saturation
		if ( l < 0.5 ) {
			s = cDelta / ( cMax + cMin );
		} else {
			s = cDelta / ( 2.0 - ( cMax + cMin ) );
		}
		
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
    return depth * ( farClip - nearClip ) + nearClip;
}

void main(void){
    vec2  depthPos = gl_Vertex.xy + depthRect.xy;
    float depth = depthValueFromSample( depthPos );
    
    vec4 pos = vec4((gl_Vertex.x - depthPP.x) * depth / depthFOV.x,
                    (gl_Vertex.y - depthPP.y) * depth / depthFOV.y,
                    depth,
                    1.0);
    
    vec2  normalPos = gl_Vertex.xy + normalRect.xy;
    normal = texture2DRect(texture, floor(normalPos) + vec2(.5,.5)).xyz * 2.0 - 1.0;
    
    //gl_Normal = normal;
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
    gl_FrontColor = gl_Color;
    
    float right = depthValueFromSample( depthPos + vec2(simplify.x,0.0) );
    float down  = depthValueFromSample( depthPos + vec2(0.0,simplify.y) );
    float left  = depthValueFromSample( depthPos + vec2(-simplify.x,0.0) );
    float up    = depthValueFromSample( depthPos + vec2(0.0,-simplify.y) );
    float bl    = depthValueFromSample( vec2(floor(depthPos.x - simplify.x),floor( depthPos.y + simplify.y)) );
    float ur    = depthValueFromSample( vec2(floor(depthPos.x  + simplify.x),floor(depthPos.y - simplify.y)) );
    
   VZPositionValid0 =  (depth < farClip &&
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
    vec3 projection = colorRotate * pos.xyz + colorTranslate + vec3(shift * colorRect.zw / colorScale,0);

    if(projection.z != 0.0) {
        vec2 xyp = projection.xy / projection.z;
        float r2 = pow(xyp.x, 2.0) + pow(xyp.y, 2.0);
        float r4 = r2*r2;
        float r6 = r4*r2;
        vec2 xypp = xyp;
        xypp.x = xyp.x * (1.0 + dK.x*r2 + dK.y*r4 + dK.z*r6) + 2.0*dP.x * xyp.x * xyp.y + dP.y*(r2 + 2.0 * pow(xyp.x,2.0) );
        xypp.y = xyp.y * (1.0 + dK.x*r2 + dK.y*r4 + dK.z*r6) + dP.x * (r2 + 2.0*pow(xyp.y, 2.0) ) + 2.0*dP.y*xyp.x*xyp.y;
        vec2 uv = (colorFOV * xypp + colorPP) * colorScale;

        gl_TexCoord[0].xy = ((uv-textureSize/2.0) * scale) + textureSize/2.0;
	}
}
