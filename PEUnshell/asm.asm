






;64位汇编程序模板 (Template)
;声明一个ExitProcess函数
ExitProcess PROTO

asmJunkCode proto

asmInt1Proc proto

.data
	;在这里声明变量

.code 


; 64位下有效的花指令模式
asmJunkCode proc

    ; 模式1：永远为真的跳转
    xor     eax, eax
    test    eax, eax
    jz      label_true
    DB      0E8h, 0, 0, 0, 0   ; 永远不会执行
label_true:
    
    ; 模式2：使用 jmp 绕过
    jmp     label_start
    DB      0E8h, 090h, 090h, 090h, 090h   ; call + nop x4
label_start:
    
    ; 模式3：无条件的短跳转
    jmp     $+3
    DB      0EBh, 001h          ; 嵌套跳转
    DB      0E8h                ; 垃圾
    nop
    
    ret

asmJunkCode endp



asmInt1Proc proc
int 1
ret
asmInt1Proc endp



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


