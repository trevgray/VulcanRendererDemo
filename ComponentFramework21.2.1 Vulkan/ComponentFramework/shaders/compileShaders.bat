@echo off
rem drag and drop a file will set %1 to the name of the file
set filename=%1

rem the next line will remove the quotation marks from the file name
set filename=%filename:"=%

rem append the '.spv' suffix on the compiled glsl output file
set output=%filename%.spv
echo Compiling: %filename%

rem if the old output file exists delete it. 
if exist %output% ( del %output% )
rem get the path to your install version of Vulkan
E:/Vulkan/Bin/glslc.exe -c "%filename%" -o "%output%"
echo Created spv file: %output%
pause
