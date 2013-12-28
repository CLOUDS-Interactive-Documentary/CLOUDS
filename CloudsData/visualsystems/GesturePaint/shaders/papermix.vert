/* HBlurVertexShader.glsl */

uniform vec2 dimensions;
uniform sampler2DRect s_texture;
uniform sampler2DRect b_texture;
varying vec2 v_texCoord;
varying float v_blurTexCoords[14];

void main()
{
	gl_Position = ftransform();
	v_texCoord  = gl_MultiTexCoord0.st;
    
	gl_FrontColor = gl_Color;

	v_blurTexCoords[ 0] = -0.028*dimensions.x;
    v_blurTexCoords[ 1] = -0.024*dimensions.x;
    v_blurTexCoords[ 2] = -0.020*dimensions.x;
    v_blurTexCoords[ 3] = -0.016*dimensions.x;
    v_blurTexCoords[ 4] = -0.012*dimensions.x;
    v_blurTexCoords[ 5] =  0.008*dimensions.x;
    v_blurTexCoords[ 6] =  0.004*dimensions.x;
    v_blurTexCoords[ 7] =  0.004*dimensions.x;
    v_blurTexCoords[ 8] =  0.008*dimensions.x;
    v_blurTexCoords[ 9] =  0.012*dimensions.x;
    v_blurTexCoords[10] =  0.016*dimensions.x;
    v_blurTexCoords[11] =  0.020*dimensions.x;
    v_blurTexCoords[12] =  0.024*dimensions.x;
    v_blurTexCoords[13] =  0.028*dimensions.x;

}
