				global main
				extern printf
				extern scanf

				section .data
				message1 db "Insert n(hex) greater than or equal 0: ",0
				message2 db "%x!! (hex) = %08x%08x%08x%08x (hex)",10,0
				error1 	 db "Invalid argument: n should be < 100.",10,0
				error2   db "Invalid argument: n should be >= 0.",10,0
				format   db "%x",0
				outnum   dd 0,0,0,1					; place 4 32-bit numbers in memory

				section .bss								; statically-allocated variables that are not explicitly initialized to any value (used by linkers)
				n resd 1										; allocates 4 bytes for n variable, to which the number will be loaded from stdio
				section .text

main:
				push 	message1
				call 	printf
				add 	esp, 4								; removes top 4 bytes from the stack (pop, but isn't stored)

				push 	n											; prepares for reading number from stdio
				push 	format
				call 	scanf
				add 	esp, 8								; after scanf cleanup
cnd1:
				cmp 	dword[n], 53					; the limit of our calculations
				jl 		cnd2									; if dword[n] less than 42, jump to cnd2
				push 	error1								; else show the error
				call 	printf
				add 	esp, 4
				jmp 	main									; ask for another number
cnd2:
        cmp 	dword[n], 1
        jg 		start									; if given n is > 0 start calculating
				jle 	printout							; else if n = 1 OR n = 0 print result of 0! = 1
        push 	error2								; else show negative number error
        call 	printf
        add 	esp, 4
				jmp 	main									; ask for another number
start:
				mov 	ecx, dword[n]					; move n to ecx registry
				mov 	edx, 0								; move 0 to edx registry
																		; edx stores what's left after each number addition (carry)
																		; for use in next iteration
nextnum:
				mov 	esi, 16								; esi registry value are not changed by C library functions (like printf)
nextpart:
				sub 	esi, 4								; subtract 4 from value stored in esi
				mov 	ebx, edx							; move what's in edx to ebx
				mov 	eax, [outnum+esi]			; move contents of what's at memory adress outnum+esi into eax

				mul 	ecx										; perform unsigned multiply on eax with ecx -> (eax*ecx)
				add 	eax, ebx							; eax = eax + ebx (adding what was left in the previous iteration)
				mov 	[outnum+esi], eax			; store calculation at [outnum+esi]

				cmp 	esi, 0
				jg 		nextpart							; if esi > 0 repeat

				sub 	ecx, 2								; ecx = n - 2
				cmp 	ecx, 1
				jge 	nextnum								; if ecx => 1 multiply result by (n-2)

printout:
				push 	dword[outnum+12]			; pushes last number of the digit onto the stack
        push 	dword[outnum+8]
        push 	dword[outnum+4]
        push 	dword[outnum]
				push 	dword[n]
				push 	message2							; prepares the string format
				call 	printf								; calls printf
				add 	esp, 24								; cleans up after printf (or else memory protection violation)
				ret
