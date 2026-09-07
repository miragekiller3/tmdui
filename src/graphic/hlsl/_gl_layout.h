
static void InitVAO_VS_INPUT1(CGLDevice* dev)
{
    GX_GL_VAO_IMPL(2, GL_FLOAT, 32,  0, false, 0)
    GX_GL_VAO_IMPL(4, GL_FLOAT, 32,  8, false, 1)
    GX_GL_VAO_IMPL(2, GL_FLOAT, 32, 24, false, 2)
}


static void InitLOC_VS_INPUT1(CGLDevice* dev, GLuint prog)
{
    GX_GL_LOC_IMPL(0, POSITION0)
    GX_GL_LOC_IMPL(1, COLOR0)
    GX_GL_LOC_IMPL(2, TEXCOORD0)
}

static const char* ubo_name_CBuffer = "type_CBuffer";

