






;64λ������ģ�� (Template)
;����һ��ExitProcess����
ExitProcess PROTO

asmInt1Proc proto

.data
	;��������������

.code 


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
	;����д�Լ��Ĵ���
	
	mov rcx,0
	call ExitProcess
myAsmTest ENDP
END


