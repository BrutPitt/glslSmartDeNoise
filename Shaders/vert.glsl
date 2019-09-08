layout (location = 0) in vec4 vPos;

#ifdef GL_ES
#else
out gl_PerVertex
{
	vec4 gl_Position;
};
#endif

void main(void)
{
    gl_Position = vPos;
}