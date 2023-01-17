#ifdef LIGHTING_SHADER
uniform mediump mat4 u_ModelViewMatrix; 
uniform mediump mat4 u_ProjectionMatrix;
uniform lowp mat3 u_NormalMatrix;

attribute lowp vec3 a_normal;

varying lowp vec3 normal;
varying lowp vec3 position;
#else
uniform mat4 u_MVPMatrix;
#endif

attribute mediump vec3 a_position;
attribute lowp vec2 a_texCoord;

varying lowp vec2 texCoord;

void main( void ) 
{
    texCoord = a_texCoord;
#ifdef LIGHTING_SHADER
    position = vec3(u_ModelViewMatrix * vec4(a_position, 1.0));
    normal = normalize(u_NormalMatrix * a_normal);
    gl_Position = u_ProjectionMatrix * vec4(position, 1.0);
#else
    gl_Position = u_MVPMatrix * vec4(a_position, 1.0);
#endif
}
