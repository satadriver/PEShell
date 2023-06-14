# PEShell

本程序是一个windows 32位加壳的例子。

原理是在peunshell生成的文件中插入一个段"lpdata"，加壳程序的实现在peshell工程中，peunshell运行时释放出这个段中隐藏的程序，并内存加载执行（或者直接释放出exe、dll文件）。

测试中发现只能运行一些小程序，比如几百kb、几MB的程序，稍微大点的程序都运行不起来。
