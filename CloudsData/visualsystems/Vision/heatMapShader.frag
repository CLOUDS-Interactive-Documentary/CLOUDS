
//here are some example uniforms
//sampler 2d rect is a texture
//
//uniform sampler2DRect thresholdedImage;
uniform sampler2DRect previousFrame;
uniform sampler2DRect currentFrame;
//uniform float inRangeMax;
//uniform float outRangeMin;
//uniform float outRangeMax;

float rgbToGray(vec4 rgbVal){
 float f =  0.21*rgbVal.r + 0.71*rgbVal.g + 0.07*rgbVal.b;
 return f;
}

float map(float value, float inputMin, float  inputMax, float  outputMin,float  outputMax ){
    
    float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
	return outVal;
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
//    vec4 colorSample = texture2DRect(thresholdedImage, gl_TexCoord[0].st);
    vec4 colorSample = texture2DRect(currentFrame, gl_TexCoord[0].st) - texture2DRect(previousFrame, gl_TexCoord[0].st);
    vec3 hsv;
    float f;
//	f = (colorSample.x +colorSample.y + colorSample.z)/3.0;
    hsv = rgb2hsv(colorSample.xyz);

    if (hsv.z > 0.1) {

        vec3 newHSV;
        newHSV.x = hsv.z;//map(hsv.z, 0.1, float  inputMax, float  outputMin,float  outputMax )
        newHSV.y = 0.8; //vec3(0.4,1.0,1.);
        newHSV.z = 0.8;
        
        vec3 newRGB;
        newRGB  = hsv2rgb(newHSV);
        gl_FragColor = vec4(newRGB,0.8);//sample * tint * brightness;
    }
    else{
        gl_FragColor = colorSample;    
    }

    
    //same a
    //vec4 sample = texture2DRect(image, gl_TexCoord[0].xy);
	//use the color tint as well as whatever ofSetColor() is set to

}