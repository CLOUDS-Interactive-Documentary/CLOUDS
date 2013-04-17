#version 110
#extension GL_ARB_texture_rectangle : enable

uniform vec2 dim;
uniform vec2 textureScale;

uniform vec2 shift;
uniform vec2 scale;

uniform sampler2DRect depthTex;
uniform vec2 principalPoint;
uniform vec2 fov;
uniform float farClip;
uniform float edgeClip;
uniform float xsimplify;
uniform float ysimplify;
uniform int useTexture;
uniform mat3 colorRotate;
uniform vec3 colorTranslate;
uniform vec2 colorFOV;
uniform vec2 colorPP;
uniform vec3 dK;
uniform vec2 dP;

varying float VZPositionValid0;

const float epsilon = 1e-6;

void main(void)
{
    //align to texture
    vec2 halfvec = vec2(.5,.5);
    float depth = texture2DRect(depthTex, floor(gl_Vertex.xy) + halfvec).r * 65535.;
    float right = texture2DRect(depthTex, floor(gl_Vertex.xy  + vec2(xsimplify,0.0)) + halfvec ).r * 65535.;
    float down  = texture2DRect(depthTex, floor(gl_Vertex.xy  + vec2(0.0,ysimplify)) + halfvec ).r * 65535.;
    float left  = texture2DRect(depthTex, floor(gl_Vertex.xy + vec2(-xsimplify,0.0)) + halfvec ).r * 65535.;
    float up    = texture2DRect(depthTex, floor(gl_Vertex.xy + vec2(0.0,-ysimplify)) + halfvec ).r * 65535.;
    float bl    = texture2DRect(depthTex, vec2(floor(gl_Vertex.x - xsimplify),floor( gl_Vertex.y + ysimplify)) + halfvec ).r * 65535.;
    float ur    = texture2DRect(depthTex, vec2(floor(gl_Vertex.x  + xsimplify),floor(gl_Vertex.y - ysimplify)) + halfvec ).r * 65535.;

    //TODO: make uniform var
    float nearClip = 20.0;
    //cull invalid verts
    VZPositionValid0 = (depth < farClip &&
                        right < farClip &&
                        down < farClip &&
                        left < farClip &&
                        up < farClip &&
                        bl < farClip &&
                        ur < farClip &&
                        
                        depth > nearClip &&
                        right > nearClip &&
                        down > nearClip &&
                        left > nearClip &&
                        up > nearClip &&
                        bl > nearClip &&
                        ur > nearClip &&
                        
                        abs(down - depth) < edgeClip &&
                        abs(right - depth) < edgeClip &&
                        abs(up - depth) < edgeClip &&
                        abs(left - depth) < edgeClip &&
                        abs(ur - depth) < edgeClip &&
                        abs(bl - depth) < edgeClip
						) ? 1.0 : 0.0;


	vec4 pos = vec4((gl_Vertex.x - principalPoint.x) * depth / fov.x,
                    (gl_Vertex.y - principalPoint.y) * depth / fov.y, depth, 1.0);

    //projective texture on the geometry
    if(useTexture == 1){
        vec4 texCd;
		//http://opencv.willowgarage.com/documentation/camera_calibration_and_3d_reconstruction.html
		vec3 projection = colorRotate * pos.xyz + colorTranslate + vec3(shift*dim / textureScale,0);
		//vec3 projection = pos.xyz + colorTranslate + vec3(shift*dim,0);
		if(projection.z != 0.0) {

			vec2 xyp = projection.xy / projection.z;
			float r2 = pow(xyp.x, 2.0) + pow(xyp.y, 2.0);
			float r4 = r2*r2;
			float r6 = r4*r2;
			vec2 xypp = xyp;
			xypp.x = xyp.x * (1.0 + dK.x*r2 + dK.y*r4 + dK.z*r6) + 2.0*dP.x * xyp.x * xyp.y + dP.y*(r2 + 2.0 * pow(xyp.x,2.0) );
			xypp.y = xyp.y * (1.0 + dK.x*r2 + dK.y*r4 + dK.z*r6) + dP.x * (r2 + 2.0*pow(xyp.y, 2.0) ) + 2.0*dP.y*xyp.x*xyp.y;
			vec2 uv = (colorFOV * xypp + colorPP) * textureScale;
			texCd.xy = ((uv-dim/2.0) * scale) + dim/2.0;
		}

        gl_TexCoord[0] = texCd;
    }
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
    gl_FrontColor = gl_Color;
}
