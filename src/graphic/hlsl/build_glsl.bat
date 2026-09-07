.\bin\ShaderConductorCmd.exe --input d11.hlsl --output _temp.glsl --stage vs --entry VS --target glsl
.\bin\file2cs.exe -string_glsl140 _temp.glsl gl_VS _gl_VS.h

.\bin\ShaderConductorCmd.exe --input d11.hlsl --output _temp.glsl --stage ps --entry PS --target glsl
.\bin\file2cs.exe -string_glsl140 _temp.glsl gl_PS _gl_PS.h

.\bin\ShaderConductorCmd.exe --input d11.hlsl --output _temp.glsl --stage ps --entry PS_Tex --target glsl
.\bin\file2cs.exe -string_glsl140 _temp.glsl gl_PS_Tex _gl_PS_Tex.h

.\bin\ShaderConductorCmd.exe --input d11.hlsl --output _temp.glsl --stage ps --entry PS_NV12 --target glsl
.\bin\file2cs.exe -string_glsl140 _temp.glsl gl_PS_NV12 _gl_PS_NV12.h

.\bin\ShaderConductorCmd.exe --input d11.hlsl --output _temp.glsl --stage ps --entry PS_NV12A --target glsl
.\bin\file2cs.exe -string_glsl140 _temp.glsl gl_PS_NV12A _gl_PS_NV12A.h

del _temp.glsl /q
pause

rem dxc /T vs_6_0 /E VS -spirv d11.hlsl   /Fo temp.spv -DENABLE_SPIRV_CODEGEN=ON 
rem spirv-cross temp.spv --output vertex.glsl

rem fxc /T vs_5_0 /E main vertex.hlsl /Fo vertex.dxbc
rem spirv-cross vertex.dxbc --output vertex.glsl