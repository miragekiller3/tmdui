rem fxc [/T ps(vs)_version] [/E main_func] [/Vn static_var_name] [/Fh h_file] hlsl_file

fxc /T ps_2_0 /E PS_NV12  /Vn cg_PS30_NV12  /Fh d9_PS_NV12.h   d9.hlsl
fxc /T ps_2_0 /E PS_NV12A /Vn cg_PS30_NV12A /Fh d9_PS_NV12A.h  d9.hlsl



pause