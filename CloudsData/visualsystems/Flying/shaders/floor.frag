#version 120
#extension GL_ARB_texture_rectangle : enable

#define TWO_PI 6.28318530717958647693
#define PI 3.14159265358979323846
#define HALF_PI  1.57079632679489661923

uniform vec3 lEye;
uniform vec3 hsv0;
uniform vec3 hsv1;
uniform sampler2D tex;
uniform vec2 resolution;
uniform float growDist;

varying float fogAmount;
varying vec3 vEye;
varying float depth;
varying float distFromFloorLookAt;

vec3 hsvToRgb(vec3 hsv) { return mix(vec3(1.),clamp((abs(fract(hsv.x+vec3(3.,2.,1.)/3.)*6.-3.)-1.),0.,1.),hsv.y)*hsv.z; }

void main()
{
    vec3 bright = hsvToRgb(mix(hsv0, hsv1, depth));
    
    vec3 wireframe = texture2D(tex, resolution * gl_TexCoord[0].st).rgb;
    
    vec3 col = step(distFromFloorLookAt, growDist) * bright + step(growDist, distFromFloorLookAt) * wireframe;
    
    // fade sides
    col *= clamp(gl_TexCoord[0].s / 0.1, 0.0, 1.0) * clamp((1.0 - gl_TexCoord[0].s) / 0.1, 0.0, 1.0);
    
    vec3 l = normalize(lEye - vEye);
	
    vec3 lightN = normalize(cross(dFdx(vEye), dFdy(vEye)));
    
	float diffuse = max(dot(lightN, l), 0.0);
    
    vec3 h = normalize(lightN + l);
    
	float nDotH = max(dot(lightN, h), 0.0);
	
	float specular = pow(nDotH, 64);
    
    //gl_FragColor = vec4(gl_TexCoord[0].t / maxDepth, gl_TexCoord[0].t / maxDepth, gl_TexCoord[0].t / maxDepth, 1.0);//gl_Color;//vec4(gl_TexCoord[0].s, gl_TexCoord[0].s, gl_TexCoord[0].s, 1.0) * diffuse;//
    gl_FragColor = (1.0 - fogAmount) * vec4(col * (diffuse + specular), 1.0);//gl_Color;//vec4(gl_TexCoord[0].s, gl_TexCoord[0].s, gl_TexCoord[0].s, 1.0) * diffuse;//
}