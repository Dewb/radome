varying vec3 ReflectDir;
varying vec4 position;

void main()
{
    gl_Position = ftransform();
    vec3 normal = gl_Normal;
    
    gl_TexCoord[0] = gl_MultiTexCoord0; 
    
    ReflectDir = normal;
    position = gl_Vertex;
}