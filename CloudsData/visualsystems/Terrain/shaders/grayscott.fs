uniform sampler2DRect backbuffer;
uniform sampler2DRect tex0;

uniform float diffU; // = 0.25;
uniform float diffV; // = 0.04;
uniform float k; // = 0.047;
uniform float f; // = 0.2;

uniform float time;
uniform float fade; //0.00 - 0.2

float kernel[9];
vec2 offset[9];

uniform vec2 mouse;

void main(void){
    vec2 st   = gl_TexCoord[0].st;
    
    kernel[0] = 0.707106781;
    kernel[1] = 1.0;
    kernel[2] = 0.707106781;
    kernel[3] = 1.0;
    kernel[4] = -6.82842712;
    kernel[5] = 1.0;
    kernel[6] = 0.707106781;
    kernel[7] = 1.0;
    kernel[8] = 0.707106781;

    offset[0] = vec2( -1.0, -1.0);
    offset[1] = vec2(  0.0, -1.0);
    offset[2] = vec2(  1.0, -1.0);
    offset[3] = vec2( -1.0, 0.0);
    offset[4] = vec2(  0.0, 0.0);
    offset[5] = vec2(  1.0, 0.0);
    offset[6] = vec2( -1.0, 1.0);
    offset[7] = vec2(  0.0, 1.0);
    offset[8] = vec2(  1.0, 1.0);
    
    vec2 texColor		= texture2DRect( backbuffer, st ).rb;
    float srcTexColor   = texture2DRect( tex0, st ).r*fade;
   // float srcTexColor   = vec2(texture2DRect( tex0, st )*mouse).r;

    vec2 lap            = vec2( 0.0, 0.0 );
    
    for( int i=0; i < 9; i++ ){
        vec2 tmp    = texture2DRect( backbuffer, st + offset[i] ).rb;
        lap         += tmp * kernel[i];
    }
    
    float F     = f + srcTexColor * 0.025 - 0.0005;
    float K     = k + srcTexColor * 0.025 - 0.0005;
    
    float u		= texColor.r;
    float v		= texColor.g + srcTexColor * 0.5;
    
    float uvv   = u * v * v;
    
    float du    = diffU * lap.r - uvv + F * (1.0 - u);
    float dv    = diffV * lap.g + uvv - (F + K) * v;
    
    u += du * 0.6;
    v += dv * 0.6;

    gl_FragColor.rgba = vec4(clamp( u, 0.0, 1.0 ), 1.0 - u/v ,clamp( v, 0.0, 1.0 ), 1.0);
}