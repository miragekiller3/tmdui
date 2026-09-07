
static const char gl_PS_NV12[] = 
    "#version 140\r\n"
    "\r\n"
    "uniform sampler2D SPIRV_Cross_CombinedTex1g_linearfilter;\r\n"
    "uniform sampler2D SPIRV_Cross_CombinedTex2g_linearfilter;\r\n"
    "\r\n"
    "in vec4 in_var_COLOR0;\r\n"
    "in vec2 varying_TEXCOORD;\r\n"
    "out vec4 out_var_SV_Target;\r\n"
    "\r\n"
    "void main()\r\n"
    "{\r\n"
    "    vec4 _46 = texture(SPIRV_Cross_CombinedTex2g_linearfilter, varying_TEXCOORD);\r\n"
    "    out_var_SV_Target = vec4(mat3(vec3(1.16438305377960205078125), vec3(0.0, -0.3917619884014129638671875, 2.01723194122314453125), vec3(1.59602701663970947265625, -0.8129680156707763671875, 0.0)) * vec3(texture(SPIRV_Cross_CombinedTex1g_linearfilter, varying_TEXCOORD).x - 0.062744997441768646240234375, _46.x - 0.501959979534149169921875, _46.w - 0.501959979534149169921875), 1.0);\r\n"
    "}\r\n"
    "\r\n"
    "";