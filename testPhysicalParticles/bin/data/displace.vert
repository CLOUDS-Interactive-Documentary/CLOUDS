#version 120

uniform sampler2DRect positions;

varying vec2 mapCoordinate;

void main() {
    gl_Position = vec4(texture2DRect(positions, gl_Vertex.xy).xyz, 1.0);

    //gl_Position.xy = gl_TexCoord[0].xy;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Position;

    gl_FrontColor =  gl_Color;
	
    mapCoordinate = gl_Vertex.xy;
}