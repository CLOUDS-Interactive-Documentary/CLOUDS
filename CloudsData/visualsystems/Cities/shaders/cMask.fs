uniform sampler2DRect tex0;

uniform vec2 resolution;

uniform float size;
uniform float smoothness;
uniform float curve;

void main( void ){
    vec2 st = gl_FragCoord.xy/resolution.xy;

    float dist = distance(st*size,vec2(size*0.5));
    dist = pow(dist,curve);

    float value = 0.0;
    value = 6.0*pow(dist,5.0)-15.0*pow(dist,4.0)+10.0*pow(dist,3.0);

    vec4 color = texture2DRect(tex0,st*resolution); 
    gl_FragColor = mix(color,vec4(0.0),value);
}