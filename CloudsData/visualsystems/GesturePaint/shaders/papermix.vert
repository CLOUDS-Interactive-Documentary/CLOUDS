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
	
//	v_blurTexCoords[ 0] = v_texCoord + vec2(-0.028, 0.0)*dimensions.x;
//    v_blurTexCoords[ 1] = v_texCoord + vec2(-0.024, 0.0)*dimensions.x;
//    v_blurTexCoords[ 2] = v_texCoord + vec2(-0.020, 0.0)*dimensions.x;
//    v_blurTexCoords[ 3] = v_texCoord + vec2(-0.016, 0.0)*dimensions.x;
//    v_blurTexCoords[ 4] = v_texCoord + vec2(-0.012, 0.0)*dimensions.x;
//    v_blurTexCoords[ 5] = v_texCoord + vec2(-0.008, 0.0)*dimensions.x;
//    v_blurTexCoords[ 6] = v_texCoord + vec2(-0.004, 0.0)*dimensions.x;
//    v_blurTexCoords[ 7] = v_texCoord + vec2( 0.004, 0.0)*dimensions.x;
//    v_blurTexCoords[ 8] = v_texCoord + vec2( 0.008, 0.0)*dimensions.x;
//    v_blurTexCoords[ 9] = v_texCoord + vec2( 0.012, 0.0)*dimensions.x;
//    v_blurTexCoords[10] = v_texCoord + vec2( 0.016, 0.0)*dimensions.x;
//    v_blurTexCoords[11] = v_texCoord + vec2( 0.020, 0.0)*dimensions.x;
//    v_blurTexCoords[12] = v_texCoord + vec2( 0.024, 0.0)*dimensions.x;
//    v_blurTexCoords[13] = v_texCoord + vec2( 0.028, 0.0)*dimensions.x;

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
