gcc2_compiled.:
___gnu_compiled_c:
	.text
	.align	2
	.globl	_crt0
_crt0:
# Initialize sp to point to the end of the stack space.        
    16777212 -> add.1; _stack_storage -> add.2
    add.3 -> sp
	sp -> stw.1; return-address -> stw.2
	sp -> add.1; -4 -> add.2; add.3 -> sp
	_main -> call.1
	sp -> add.1; 4 -> add.2; add.3 -> sp
	sp -> ldw.1; ldw.2 -> return-address
	.stabs	"-1",0xd6,0,0,LFR0
LFR0:
	return-address -> jump.1


	.comm	_stack_storage, 16777216
