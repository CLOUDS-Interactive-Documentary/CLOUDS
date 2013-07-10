#version 120

uniform sampler2DRect positions;
uniform float sizeThreshold;
//varying vec2 mapCoordinate;

void main() {
    gl_Position = vec4( texture2DRect(positions, floor(gl_Vertex.xy) + vec2(.5,.5) ).xyz, 1.0);

    //gl_Position.xy = gl_TexCoord[0].xy;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Position;

    gl_FrontColor = gl_Color;
//	gl_PointSize = gl_Normal.z < sizeThreshold ? 2.0 : 1.0;
	gl_PointSize = pow(gl_Normal.z,4.0) * 1.8 + .25;
	

}