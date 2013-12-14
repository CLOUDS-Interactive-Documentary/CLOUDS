#version 120
#extension GL_ARB_texture_rectangle : enable

#define TWO_PI 6.28318530717958647693
#define PI 3.14159265358979323846
#define HALF_PI  1.57079632679489661923

uniform vec3 lightPos;
uniform float time;
uniform float frequency;
uniform vec3 colour;
uniform sampler2D tex;
uniform float texAmt;

//varying vec3 nEye;
varying vec3 vEye;
varying float fogAmount;

void main()
{
    vec3 l = normalize(lightPos - vEye);
	vec3 lightN = normalize(cross(dFdx(vEye), dFdy(vEye)));
    //vec3 lightN = normalize(nEye);
    
	float diffuse = max(dot(lightN, l), 0.0);
    
    //diffuse *= 0.4;
	
    /*
    vec3 h = normalize(lightN + l);
    
	float nDotH = max(dot(lightN, h), 0.0);
	
	float specular = 1e-2 * pow(nDotH, 64);
     */
    
    
    //vec4 col = colour + vec4(0.0, 0.2 * (1.0 - gl_TexCoord[0].t), 0.0, 0.0);
    
    //gl_FragColor = col;
    //gl_FragColor = vec4(gl_TexCoord[0].t, 0.0, 0.0, 0.3);
    //gl_FragColor = vec4(gl_TexCoord[0].s / 512.0, 0.0, 0.0,1.0);//
    /*vec3 col = vec3(0.6, 0.6, 0.6);//texture2DRect(tex, gl_TexCoord[0].st).rgb;
    col.r += clamp(sin(frequency * -0.25 * time + gl_TexCoord[0].s / 512.0), 0.0, 0.3);
    col.g += clamp(sin(frequency * 0.25 * time + HALF_PI + gl_TexCoord[0].s / 512.0), 0.0, 0.3);
    col.b += clamp(sin(frequency * 0.5 * time + PI + gl_TexCoord[0].s / 512.0), 0.0, 0.3);*/
    
    gl_FragColor = (1.0 - fogAmount) * vec4(colour.rgb * texture2D(tex, gl_TexCoord[0].st).rgb, 1.0);//vec4(texAmt * texture2D(tex, gl_TexCoord[0].st).rgb + 0.2 * colour * (diffuse/* + specular*/), 1.0);

}