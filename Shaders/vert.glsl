layout (location = 0) in vec2 vPos;

#if !defined(GL_ES)
out gl_PerVertex
{
    vec4 gl_Position;
};
#endif

out vec2 vTexCoord;
vec2 texCoord[4] = vec2[4](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 1.0), vec2(0.0, 1.0));

void main(void)
{
    vTexCoord = texCoord[gl_VertexID];
    gl_Position = vec4(vPos.xy,.0f,1.f);

}