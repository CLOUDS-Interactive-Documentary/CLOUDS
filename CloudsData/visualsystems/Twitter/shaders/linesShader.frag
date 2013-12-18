#version 110
#extension GL_ARB_texture_rectangle : enable

void main(){
    
    if(positionValid < epsilon){
    	discard;
        return;
    }

	float attenuate = 1.0 + max(isEye() * eyeMultiplier, isSkin() * skinMultiplier);
	if(attenuate < epsilon){
		discard;
		return;
	}
	
    vec4 col = texture2DRect(texture, gl_TexCoord[0].st);
    gl_FragColor = gl_Color * col * attenuate * max( calculateLight(), isSkin() );
	//gl_FragColor = vec4(normal,1.0);
}

