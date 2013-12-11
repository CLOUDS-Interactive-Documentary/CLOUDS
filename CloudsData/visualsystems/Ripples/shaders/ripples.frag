uniform sampler2DRect backbuffer;   // previous buffer
uniform sampler2DRect tex0;         // actual buffer

uniform float damping;

vec2 offset[4];

void main()
{
    vec2 st = gl_TexCoord[0].st;
    
    offset[0] = vec2(-1.0,  0.0);
    offset[1] = vec2( 1.0,  0.0);
    offset[2] = vec2( 0.0,  1.0);
    offset[3] = vec2( 0.0, -1.0);
    
    //  Grab the information arround the active pixel
    //
    //      [3]
    //
    //  [0]  st  [1]
    //
    //      [2]
    
    vec3 sum = vec3(0.0, 0.0, 0.0);
    
    for (int i = 0; i < 4 ; i++){
        sum += texture2DRect(tex0, st + offset[i]).rgb;
    }
    
    //  make an average and substract the center value
    //
    sum = (sum / 2.0) - texture2DRect(backbuffer, st).rgb;
    sum *= damping;
    
    gl_FragColor = vec4(sum, 1.0);
    //gl_FragColor = texture2DRect(tex0, st);

    // gl_FragColor = vec4(st, 0.0, 1.0);
}
