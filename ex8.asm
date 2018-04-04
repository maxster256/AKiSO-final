global main
global lnx
global ex
global sinhx
global sinh1x

extern printf
extern scanf

section .data
			variable db "X=",0
			fmt 		 db "%lf",0
			lnxresult  db "A) %lf",10,0
			exresult  db "B) %lf",10,0
			sinhxresult  db "C) %lf",10,0
			sinh1xresult  db "D) %lf",10,0

section .bss
			x resq 1
			result resq 1
			param1 resq 1
			param2 resq 1
			param3 resq 1
			param4 resq 1

section .text

main:
			finit									; initializies FPU by resetting all registers and flags
														; to defaults
			push variable
			call printf
			add esp,4

			push x
			push fmt
			call scanf
			add esp,8

			; prints out result for ln(x)
			fld qword[x]					; load floating point value
			call lnx							; jumps to the procedure
		  fstp qword[result]		; convert to real number and pop
		  push dword[result+4]
		  push dword[result]
		  push lnxresult
		  call printf
		  add esp,12

			; prints out result for e^x
			fld qword[x]
			call ex
      fstp qword[result]
      push dword[result+4]
      push dword[result]
      push exresult
      call printf
      add esp,12

			; prints out the result for sinh(x)
      fld qword[x]
      call sinhx
      fstp qword[result]
      push dword[result+4]
      push dword[result]
      push sinhxresult
      call printf
      add esp,12

			; prints out the result for sinh-1(x)
      fld qword[x]
      call sinh1x
      fstp qword[result]
      push dword[result+4]
      push dword[result]
      push sinh1xresult
      call printf
      add esp,12

			ret

lnx:
			fstp qword[param1]				; convert to real and pop
			fld1											; push 1 (constant)
			fld qword[param1]					; push real number
			fyl2x 										;	load log2(x) to the counter (licznik)
			fldl2e 										; load log2(e) to the denominator (mianownik)

			fdiv											; perform the division
			ret												; return back to main

ex:
			; we'll be using the formula: x^(a+b) = x^(a) * x^(b)
      ; (wyliczenie pierwszego czynnika ktory w wykladniku ma obciecie log2(e)
    	; do czesci ulamkowej przemnozone przez x)
			fstp qword[param2]
			fld qword[param2]

			fldl2e										; push log2(e)
      fmulp           					; 5*log2(e)
			fld qword[param2]
		  fldl2e
			fmulp

			frndint		;	floor[5*log2(e)] <- round to integer
		  fsubp			; 5*log2(e) - floor[5*log2(e)]
			ftst			; compares st (top of the stack) with 0
			fstsw ax	; stores te current value of the status word in ax
			sahf			; copies the AH to flags
			ja go
			fld1			; load constant = 1
			faddp			; adds st(0) to st(1), stores result in st(1) and pops the register stack
go:
      f2xm1			; compute 2^x - 1, x E (-1,1)
      fld1			; load constant = 1
      faddp
      ; wyliczenie drugiego czynnika ktory w wykladniku ma czesc calkowita log2(e)
			; przemnozona przez x
			fld qword[param2]
      fldl2e
			fmulp			; 5*log2(e)
			fld1
      fscale		; 1*2^(log2(e))
      fstp (st1) ; zdejmujemy 1

      ; wymnozenie dwoch czynnikow, wynik w postaci e^(x)
      fmulp

			ret

sinhx:
			; sinhx = (e^x - e^[-x])/2
			fstp qword[param3]
			fld qword[param3]
			call ex

			fld1
			fld qword[param3]
			call ex
			fdivp		; divided st(1) by st(0), stores the result in st(1) and pops back

			fsubp		; subtracts: st(1) = st(0) - st(1)

			fld1
			fld1
			faddp

			fdivp

			ret

sinh1x:
			; sinh-1x = ln(x + sqrt(x^2 + 1))
			fstp qword[param4]
			fld qword[param4]
			fld (st0)
			fmulp   ; st(1) = st(1) * st(0)
			fld1
			faddp
			fsqrt		; computes square root of st(0) and stores the result in st(0)

			fld qword[param4]

			faddp

			call lnx
ret
