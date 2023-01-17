#ifdef LIGHTING_SHADER
uniform mediump vec3 u_lightPosition;
uniform lowp vec3 u_ambientColor;
uniform lowp vec3 u_diffuseColor;
uniform lowp vec3 u_specularColor;
uniform mediump float u_shininess;
uniform lowp float u_dissolve;

varying lowp vec3 normal;
varying lowp vec3 position;
#endif

uniform sampler2D u_diffuse;

varying lowp vec2 texCoord;

void main( void ) {

   lowp vec4 diffuse_color = texture2D( u_diffuse, texCoord );
   
#ifdef LIGHTING_SHADER
   lowp float alpha = diffuse_color.a;
   
   mediump vec3 L = normalize(u_lightPosition - position);
   
   mediump vec3 E = normalize(-position);
   
   mediump vec3 R = normalize(-reflect(L, normal));
   
   mediump vec4 ambient = vec4(u_ambientColor, 1.0);
   
   mediump vec4 diffuse = vec4(u_diffuseColor*diffuse_color.rgb, 1.0) * max(dot(normal, L), 0.0);
   
   mediump vec4 specular = vec4(u_specularColor, 1.0) * pow(max(dot(R, E), 0.0), u_shininess*0.3);
   
   diffuse_color = vec4(0.1) + ambient + diffuse + specular;
   
   diffuse_color.a = alpha;
#endif
   
    #ifdef SOLID_OBJECT
	
		gl_FragColor = diffuse_color;
	#endif

	#ifdef ALPHA_TESTED_OBJECT

		if( diffuse_color.a < 0.1 ) discard;
	  
		else gl_FragColor = diffuse_color;
	#endif

	#ifdef TRANSPARENT_OBJECT
		#ifdef LIGHTING_SHADER
            gl_FragColor = diffuse_color;		
            gl_FragColor.a = u_dissolve; 
        #else
            gl_FragColor = diffuse_color;		
            gl_FragColor.a = 0.65;
        #endif
	#endif

}
  