rem fxc [/T ps(vs)_version] [/E main_func] [/Vn static_var_name] [/Fh h_file] hlsl_file

rem fxc /T ps_3_0 /E PS_NV12  /Vn cg_PS30_NV12  /Fh d9_PS_NV12.h   d9.hlsl
rem fxc /T ps_3_0 /E PS_NV12A /Vn cg_PS30_NV12A /Fh d9_PS_NV12A.h  d9.hlsl

.\bin\fxc /T vs_2_0 /E VS       /Vn cg3_VS        /Fh _d9_VS.h        d9.hlsl
.\bin\fxc /T ps_2_0 /E PS       /Vn cg3_PS        /Fh _d9_PS.h        d9.hlsl
.\bin\fxc /T ps_2_0 /E PS_Tex   /Vn cg3_PS_Tex    /Fh _d9_PS_Tex.h    d9.hlsl
.\bin\fxc /T ps_2_0 /E PS_NV12  /Vn cg3_PS_NV12   /Fh _d9_PS_NV12.h   d9.hlsl
.\bin\fxc /T ps_2_0 /E PS_NV12A /Vn cg3_PS_NV12A  /Fh _d9_PS_NV12A.h  d9.hlsl


.\bin\fxc /T vs_4_0 /E VS       /Vn cg4_VS        /Fh _d11_VS.h       d11.hlsl
.\bin\fxc /T ps_4_0 /E PS       /Vn cg4_PS        /Fh _d11_PS.h       d11.hlsl
.\bin\fxc /T ps_4_0 /E PS_Tex   /Vn cg4_PS_Tex    /Fh _d11_PS_Tex.h   d11.hlsl
.\bin\fxc /T ps_4_0 /E PS_NV12  /Vn cg4_PS_NV12   /Fh _d11_PS_NV12.h  d11.hlsl
.\bin\fxc /T ps_4_0 /E PS_NV12A /Vn cg4_PS_NV12A  /Fh _d11_PS_NV12A.h d11.hlsl

pause