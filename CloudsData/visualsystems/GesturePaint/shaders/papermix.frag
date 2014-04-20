
/* VBlurVertexShader.glsl */

uniform vec2 dimensions;

uniform sampler2DRect source_texture;
uniform sampler2DRect water_texture;
uniform sampler2DRect flow_texture;

uniform float flowSwap;
uniform float flowWidth;

varying vec2 v_texCoord;
varying float v_blurTexCoords[14];

const float epsilon = 1e-6;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
	vec4 flowSample = texture2DRect(water_texture, v_texCoord*.25);
	vec2 flowSwapCoord = vec2(flowWidth - v_texCoord.x,v_texCoord.y);
	
	vec2 flowDeviationA = texture2DRect(flow_texture, flowSwapCoord).rg * 2.0 - 1.0;
	vec2 flowDeviationB = texture2DRect(flow_texture, v_texCoord).rg * 2.0 - 1.0;
	vec2 flowDeviation = mix(flowDeviationA,flowDeviationB,flowSwap);
	vec2 flowDirection = -normalize( flowSample.rg + flowDeviation);
	float flowAmount   = length( flowSample.rg );

	if(flowAmount > epsilon){
		gl_FragColor = vec4(0.0);
		gl_FragColor += texture2DRect(source_texture, v_texCoord+
										  flowDirection*v_blurTexCoords[ 0]*flowAmount)*0.0044299121055113265;
		gl_FragColor += texture2DRect(source_texture, v_texCoord+
										  flowDirection*v_blurTexCoords[ 1]*flowAmount)*0.00895781211794;
		gl_FragColor += texture2DRect(source_texture, v_texCoord+
										  flowDirection*v_blurTexCoords[ 2]*flowAmount)*0.0215963866053;
		gl_FragColor += texture2DRect(source_texture, v_texCoord+
										  flowDirection*v_blurTexCoords[ 3]*flowAmount)*0.0443683338718;
		gl_FragColor += texture2DRect(source_texture, v_texCoord+
										  flowDirection*v_blurTexCoords[ 4]*flowAmount)*0.0776744219933;
		gl_FragColor += texture2DRect(source_texture, v_texCoord+
										  flowDirection*v_blurTexCoords[ 5]*flowAmount)*0.115876621105;
		gl_FragColor += texture2DRect(source_texture, v_texCoord+
										  flowDirection*v_blurTexCoords[ 6]*flowAmount)*0.147308056121;
		gl_FragColor += texture2DRect(source_texture, v_texCoord)*0.159576912161;
		gl_FragColor += texture2DRect(source_texture, v_texCoord+
										  flowDirection*v_blurTexCoords[ 7]*flowAmount)*0.147308056121;
		gl_FragColor += texture2DRect(source_texture, v_texCoord+
										  flowDirection*v_blurTexCoords[ 8]*flowAmount)*0.115876621105;
		gl_FragColor += texture2DRect(source_texture, v_texCoord+
										  flowDirection*v_blurTexCoords[ 9]*flowAmount)*0.0776744219933;
		gl_FragColor += texture2DRect(source_texture, v_texCoord+
										  flowDirection*v_blurTexCoords[10]*flowAmount)*0.0443683338718;
		gl_FragColor += texture2DRect(source_texture, v_texCoord+
										  flowDirection*v_blurTexCoords[11]*flowAmount)*0.0215963866053;
		gl_FragColor += texture2DRect(source_texture, v_texCoord+
										  flowDirection*v_blurTexCoords[12]*flowAmount)*0.00895781211794;
		gl_FragColor += texture2DRect(source_texture, v_texCoord+
										  flowDirection*v_blurTexCoords[13]*flowAmount)*0.0044299121055113265;
        vec4 color = vec4(rgb2hsv(gl_FragColor.rgb),gl_FragColor.a);
        color.y += 0.05;
        gl_FragColor = vec4(hsv2rgb(color.rgb),color.a);
	}
	else{
		gl_FragColor = texture2DRect(source_texture, v_texCoord);
        vec4 color = vec4(rgb2hsv(gl_FragColor.rgb),gl_FragColor.a);
        color.y += 0.05;
        gl_FragColor = vec4(hsv2rgb(color.rgb),color.a);
	}
}
