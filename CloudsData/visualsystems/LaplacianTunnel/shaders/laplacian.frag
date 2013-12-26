
uniform vec3 fogColor;
uniform float minFogDist;
uniform float maxFogDist;
uniform float cameray;
varying float zdist;

void main() {

	float depth = (zdist-cameray);
	float fogStr = clamp((depth-minFogDist) / (maxFogDist-minFogDist),0.0,1.0);
	gl_FragColor = mix(gl_Color, vec4(fogColor,1.0), fogStr);

}