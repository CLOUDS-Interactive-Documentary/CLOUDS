/* BlurFragmentShader.glsl */
//precision mediump float;

uniform sampler2DRect s_texture;
uniform sampler2DRect s_paper;

varying vec2 v_texCoord;
varying vec2 v_blurTexCoords[14];

const float epsilon = 1e-6;

void main()
{
    gl_FragColor = vec4(0.0);
    gl_FragColor.rgb += texture2DRect(s_texture, v_blurTexCoords[ 0]).rgb*0.0044299121055113265;
    gl_FragColor.rgb += texture2DRect(s_texture, v_blurTexCoords[ 1]).rgb*0.00895781211794;
    gl_FragColor.rgb += texture2DRect(s_texture, v_blurTexCoords[ 2]).rgb*0.0215963866053;
    gl_FragColor.rgb += texture2DRect(s_texture, v_blurTexCoords[ 3]).rgb*0.0443683338718;
    gl_FragColor.rgb += texture2DRect(s_texture, v_blurTexCoords[ 4]).rgb*0.0776744219933;
    gl_FragColor.rgb += texture2DRect(s_texture, v_blurTexCoords[ 5]).rgb*0.115876621105;
    gl_FragColor.rgb += texture2DRect(s_texture, v_blurTexCoords[ 6]).rgb*0.147308056121;
    gl_FragColor.rgb += texture2DRect(s_texture, v_texCoord         ).rgb*0.159576912161;
    gl_FragColor.rgb += texture2DRect(s_texture, v_blurTexCoords[ 7]).rgb*0.147308056121;
    gl_FragColor.rgb += texture2DRect(s_texture, v_blurTexCoords[ 8]).rgb*0.115876621105;
    gl_FragColor.rgb += texture2DRect(s_texture, v_blurTexCoords[ 9]).rgb*0.0776744219933;
    gl_FragColor.rgb += texture2DRect(s_texture, v_blurTexCoords[10]).rgb*0.0443683338718;
    gl_FragColor.rgb += texture2DRect(s_texture, v_blurTexCoords[11]).rgb*0.0215963866053;
    gl_FragColor.rgb += texture2DRect(s_texture, v_blurTexCoords[12]).rgb*0.00895781211794;
    gl_FragColor.rgb += texture2DRect(s_texture, v_blurTexCoords[13]).rgb*0.0044299121055113265;
	
//	vec4 paperTexture = texture2DRect(s_paper, v_texCoord);
//	gl_FragColor.rgb -= epsilon*10.;
	gl_FragColor.rgb -= .0001;
//	gl_FragColor.rgb = mix(paperTexture.rgb,gl_FragColor.rgb, gl_FragColor.a);
	gl_FragColor.a = 1.;
	
}