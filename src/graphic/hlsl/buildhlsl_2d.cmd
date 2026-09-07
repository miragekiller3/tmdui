rem fxc [/T ps(vs)_version] [/E main_func] [/Vn static_var_name] [/Fh h_file] hlsl_file

rem fxc /T vs_3_0 /E VS /Fh VS_2d.h line_all_iup.hlsl.inl
fxc /T ps_3_0 /E PS /Fh PS_2d.h line_all_iup.hlsl.inl

rem fxc /T ps_2_0 /E Video_Y411 /Fh Video_Y411.h Video.hlsl
rem fxc /T ps_2_0 /E Video_NV21 /Fh Video_NV21.h Video.hlsl

rem fxc /T ps_2_0 /E PS_NV12  /Vn g_PS20_NV12  /Fh d9_PS_NV12.h   d9.hlsl
rem fxc /T ps_2_0 /E PS_NV12A /Vn g_PS20_NV12A /Fh d9_PS_NV12A.h  d9.hlsl

rem fxc /T vs_4_0 /E VS       /Vn g_VS         /Fh d11_VS.h       d11.hlsl
rem fxc /T ps_4_0 /E PS       /Vn g_PS         /Fh d11_PS.h       d11.hlsl
rem fxc /T ps_4_0 /E PS_Tex   /Vn g_PS_Tex     /Fh d11_PS_Tex.h   d11.hlsl
rem fxc /T ps_4_0 /E PS_NV12  /Vn g_PS_NV12    /Fh d11_PS_NV12.h  d11.hlsl
rem fxc /T ps_4_0 /E PS_NV12A /Vn g_PS_NV12A   /Fh d11_PS_NV12A.h d11.hlsl

pause