
/* VBlurVertexShader.glsl */

uniform vec2 dimensions;
varying vec2 v_texCoord;
varying vec2 v_blurTexCoords[14];
uniform float blurRadius;

void main()
{
	gl_Position = ftransform();
	v_texCoord  = gl_MultiTexCoord0.st;
	
    v_blurTexCoords[ 0] = v_texCoord + vec2(0.0, -0.028)*dimensions.y*blurRadius;
    v_blurTexCoords[ 1] = v_texCoord + vec2(0.0, -0.024)*dimensions.y*blurRadius;
    v_blurTexCoords[ 2] = v_texCoord + vec2(0.0, -0.020)*dimensions.y*blurRadius;
    v_blurTexCoords[ 3] = v_texCoord + vec2(0.0, -0.016)*dimensions.y*blurRadius;
    v_blurTexCoords[ 4] = v_texCoord + vec2(0.0, -0.012)*dimensions.y*blurRadius;
    v_blurTexCoords[ 5] = v_texCoord + vec2(0.0, -0.008)*dimensions.y*blurRadius;
    v_blurTexCoords[ 6] = v_texCoord + vec2(0.0, -0.004)*dimensions.y*blurRadius;
    v_blurTexCoords[ 7] = v_texCoord + vec2(0.0,  0.004)*dimensions.y*blurRadius;
    v_blurTexCoords[ 8] = v_texCoord + vec2(0.0,  0.008)*dimensions.y*blurRadius;
    v_blurTexCoords[ 9] = v_texCoord + vec2(0.0,  0.012)*dimensions.y*blurRadius;
    v_blurTexCoords[10] = v_texCoord + vec2(0.0,  0.016)*dimensions.y*blurRadius;
    v_blurTexCoords[11] = v_texCoord + vec2(0.0,  0.020)*dimensions.y*blurRadius;
    v_blurTexCoords[12] = v_texCoord + vec2(0.0,  0.024)*dimensions.y*blurRadius;
    v_blurTexCoords[13] = v_texCoord + vec2(0.0,  0.028)*dimensions.y*blurRadius;
}
