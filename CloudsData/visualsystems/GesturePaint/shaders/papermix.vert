/* HBlurVertexShader.glsl */

uniform vec2 dimensions;

varying vec2 v_texCoord;
varying vec2 v_blurTexCoords[14];

void main()
{
	gl_Position = ftransform();
	v_texCoord  = gl_MultiTexCoord0.st;
	
    gl_FrontColor = gl_Color;
}
