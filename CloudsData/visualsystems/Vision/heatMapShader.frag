
//here are some example uniforms
//sampler 2d rect is a texture


uniform sampler2DRect previousFrame;
uniform sampler2DRect currentFrame;
//uniform float hueShift;

float map(float value, float inputMin, float  inputMax, float  outputMin,float  outputMax ){
    
    float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
	return clamp(outVal,outputMin,outputMax);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

void main (void)
{
    //texture look up
    //gives a specific pixel in a texture
    //xyzw - common for poisitions
    //stuv - common for talking about texture coordinates and normals
    //rgba - common for colour
    //st is the same as xy or rg
    //so instead of doing myVec.x and myVec.y
    //you cam do myVec.xy
	float hueShift = .2;
    vec3 colorSample = abs(texture2DRect(currentFrame, gl_TexCoord[0].st).rgb - texture2DRect(previousFrame, gl_TexCoord[0].st).rgb);
	colorSample = 1.0 - pow(vec3(1.0) - colorSample,vec3(1.5));
	
    vec3 hsv = rgb2hsv(colorSample.xyz);
	vec3 newHSV = vec3(mod(hsv.z+hueShift,1.0), 1.0, 1.0);
	vec3 newRGB  = hsv2rgb(newHSV);

	gl_FragColor.rgb = newRGB;
	gl_FragColor.a = map(hsv.z, 0.0, 0.3, 0., 1.0);


}