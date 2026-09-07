
static const char gl_PS[] = 
    "#version 140\r\n"
    "\r\n"
    "in vec4 varying_COLOR0;\r\n"
    "in vec2 in_var_TEXCOORD;\r\n"
    "out vec4 out_var_SV_Target;\r\n"
    "\r\n"
    "void main()\r\n"
    "{\r\n"
    "    out_var_SV_Target = varying_COLOR0;\r\n"
    "}\r\n"
    "\r\n"
    "";