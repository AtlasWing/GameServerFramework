@echo off
rem �����ͻ�����Դ���¹�����ʱ�������
rem --------------ͳһ���ã������и���-----------
rem --�ͻ�����Դ·��--
set client_res_path=\\10.1.101.232\������Դ\ClientRes
rem --����WinRARѹ�����߰�װ·��
set winrar_bin_path="C:\Program Files\WinRAR\winrar.exe"
rem --��ת������IP��ַ--
set server_ip=10.1.101.6
rem --��ת�������û���--
set server_username=banben11
rem --��ת�������û�����--
set server_password=banben2011
rem --------------���ý���-----------------------

set hour=%time:~,2%
if "%time:~,1%"==" " set hour=0%time:~1,1%
set filename=ClientRes%date:~0,4%%date:~5,2%%date:~8,2%%hour%%time:~3,2%%time:~6,2%

rem --------------������Դ-----------
echo "��ʼ��%client_res_path%���ؿͻ�����Դ�ļ�"
md %filename%
xcopy /S /E %client_res_path% %filename%

rem --------------ѹ����Դ-----------
echo "������ɣ���ʼѹ���ļ�"
%winrar_bin_path% a %filename%.zip %filename%
echo "ѹ����ɣ�����Դ�ϴ������ط�����"

rem --------------�ϴ���Դ-----------
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
echo "��Դ�ϴ���ɣ��ϴ�λ��: %server_ip%:/home/%server_username%/client/%filename%.zip"

echo "���¼������ʹ��./tools/zonemanage.py����Դ�ϴ�������"

pause