
static const char gl_PS_Tex[] = 
    "#version 140\r\n"
    "\r\n"
    "uniform sampler2D SPIRV_Cross_CombinedTex1g_linearfilter;\r\n"
    "\r\n"
    "in vec4 varying_COLOR0;\r\n"
    "in vec2 varying_TEXCOORD;\r\n"
    "out vec4 out_var_SV_Target;\r\n"
    "\r\n"
    "void main()\r\n"
    "{\r\n"
    "    out_var_SV_Target = texture(SPIRV_Cross_CombinedTex1g_linearfilter, varying_TEXCOORD) * varying_COLOR0;\r\n"
    "}\r\n"
    "\r\n"
    "";