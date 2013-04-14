#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect tex;
uniform sampler2DRect range;
uniform vec2 sampleOffset;
uniform float focalDistance;
uniform float focalRange;

float weights[21];

//converts depth value to normalized, linear, between near and far plane
float LinearizeDepth(float zoverw){
    //todo: take near and far as uniforms
    float n = 1.0; // camera z near
    float f = 20000.0; // camera z far
    return (2.0 * n) / (f + n - zoverw * (f - n));
}

//translate a Depth texel into a focal value based on our
float FocalValue(vec2 pos)
{
    float depth = LinearizeDepth( texture2DRect(range, pos).r ) * 20000.;
    return min( abs(depth  - focalDistance) / focalRange, 1.0);
}

void main()
{
    //Gaussian Kernel
	weights[0] = 0.0091679276560113852;
	weights[1] = 0.014053461291849008;
	weights[2] = 0.020595286319257878;
	weights[3] = 0.028855245532226279;
	weights[4] = 0.038650411513543079;
	weights[5] = 0.049494378859311142;
	weights[6] = 0.060594058578763078;
	weights[7] = 0.070921288047096992;
	weights[8] = 0.079358891804948081;
	weights[9] = 0.084895951965930902;
	weights[10] = 0.086826196862124602;
	weights[11] = 0.084895951965930902;
	weights[12] = 0.079358891804948081;
	weights[13] = 0.070921288047096992;
	weights[14] = 0.060594058578763092;
	weights[15] = 0.049494378859311121;
	weights[16] = 0.0386504115135431;
	weights[17] = 0.028855245532226279;
	weights[18] = 0.020595286319257885;
	weights[19] = 0.014053461291849008;
	weights[20] = 0.00916792765601138;

	vec3 sum = vec3(0.0, 0.0, 0.0);
    float rangeTexel = FocalValue( gl_TexCoord[0].st);
    vec2 blurOffset = sampleOffset * rangeTexel;
	vec2 baseOffset = -10.0 * blurOffset;
	vec2 offset = vec2( 0.0, 0.0 );
//    int samples = (fast == 1) ? 11 : 21;
    int samples = 21;
    //iterato through a number of samples
	for( int s = 0; s < samples; ++s ) {
        //sample the surrounding pixels and blend them into the total with the gaussian weight
        //if fast is off, we weight the blur amount by the focal difference from the center point
        //which helps make in focus items 'pop' and avoid color bleeding between focal depths
        vec4 texel = texture2DRect( tex, gl_TexCoord[0].st + baseOffset + offset ) * (1. - abs( FocalValue(gl_TexCoord[0].st + baseOffset + offset)  - rangeTexel) );
        sum += texel.rgb * weights[s];
		offset += blurOffset;
	}
    
	gl_FragColor.rgb = sum * gl_Color.rgb;
    gl_FragColor.a = gl_Color.a;
}