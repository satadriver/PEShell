






;64位汇编程序模板 (Template)
;声明一个ExitProcess函数
ExitProcess PROTO

.data
	;在这里声明变量

.code 

asmSingleTrap PROC

	pushfq
	pop rax
	or rax,100h
	push   rax
	popfq

	ret
asmSingleTrap ENDP

myAsmTest PROC
	;这里写自己的代码
	
	mov rcx,0
	call ExitProcess
myAsmTest ENDP
END


