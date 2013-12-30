uniform sampler2DRect tex;  
  
void main(void) 
{    
   gl_FragColor = texture2DRect(tex, gl_TexCoord[0].xy);  
}
