#version 120
#define EPSILON 0.05
uniform sampler2D tex;
uniform float kernel_maxValue;
varying float size;
uniform vec3 light;


float getLight(){
    vec2 direction = gl_TexCoord[0].xy - vec2(.5);
    float ld = length(direction);
    float ll = length(light.xy);
    //safety
    if (ll < EPSILON){
        return 1.;
    }
    
    float reflection = dot(direction/ld, light.xy/ll);
    //softens the tip
    float accent = smoothstep(0.,0.1,ld);
    float light = mix(0.1, 1., accent * clamp(reflection,0.,1.));
    return mix(1., light, smoothstep(EPSILON, 0.2, ll));
}


void main (void) {
    vec4 c = texture2D(tex, gl_TexCoord[0].xy);
    c.r = getLight();
    c.a = sqrt(c.b);
	gl_FragColor = c * kernel_maxValue;
    
}
