@echo off
rem 外网客户端资源更新工具临时解决方案
rem --------------统一配置，可自行更改-----------
rem --客户端资源路径--
set client_res_path=\\10.1.101.232\工具资源\ClientRes
rem --本地WinRAR压缩工具安装路径
set winrar_bin_path="C:\Program Files\WinRAR\winrar.exe"
rem --中转服务器IP地址--
set server_ip=10.1.101.6
rem --中转服务器用户名--
set server_username=banben11
rem --中转服务器用户密码--
set server_password=banben2011
rem --------------配置结束-----------------------

set hour=%time:~,2%
if "%time:~,1%"==" " set hour=0%time:~1,1%
set filename=ClientRes%date:~0,4%%date:~5,2%%date:~8,2%%hour%%time:~3,2%%time:~6,2%

rem --------------下载资源-----------
echo "开始从%client_res_path%下载客户端资源文件"
md %filename%
xcopy /S /E %client_res_path% %filename%

rem --------------压缩资源-----------
echo "下载完成，开始压缩文件"
%winrar_bin_path% a %filename%.zip %filename%
echo "压缩完成，将资源上传到本地服务器"

rem --------------上传资源-----------
del tmp.txt
echo open %server_ip%>> tmp.txt
echo %server_username%>> tmp.txt
echo %server_password%>> tmp.txt
echo cd>> tmp.txt
echo ls>> tmp.txt
echo cd client>> tmp.txt
echo put %filename%.zip>> tmp.txt
echo quit>> tmp.txt
ftp -s:tmp.txt
del tmp.txt
echo "资源上传完成，上传位置: %server_ip%:/home/%server_username%/client/%filename%.zip"

echo "请登录服务器使用./tools/zonemanage.py将资源上传到外网"

pause