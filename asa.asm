global restar_y_al_cuadrado
global sumar
global restar_y_al_cuadrado_ymm

%assign OFF 80
%assign OFFYMM 32

restar_y_al_cuadrado:

;--------------------------PARAMETROS----------------------------------
;RDI:		n
;RSI:		cant_vecinos
;RDX:		x 			--->	R8
;RCX:		x_a 		--->	R9
;R8:		r 			--->	R12
;----------------------------------------------------------------------

;-----------------------INTEGRIDAD DE LA PILA--------------------------
PUSH	RBP
MOV  	RBP, RSP
PUSH	R12
PUSH 	R13
;----------------------ORDENAMIENTO   PARAMETROS-----------------------
MOV 	R12, R8			;se guarda el vector auxiliar
MOV 	R8, RDX
MOV 	R9, RCX
	
IMUL	RDI, RSI 		; cantidad de floats

LEA 	R10,[R8  + RDI * 4  - OFF]	; ultima posicion valida para leer/escribir
LEA 	R11,[R9  + RDI * 4  - OFF]
LEA 	R13,[R12 + RDI * 4  - OFF]

.ciclo:

	MOVDQU		XMM1, [R8]
	MOVDQU		XMM2, [R8 + 16]
	MOVDQU		XMM3, [R8 + 32]
	MOVDQU		XMM4, [R8 + 48]
	MOVDQU		XMM5, [R8 + 64]
	;MOVDQU		XMM6, [R8 + 80]
	;MOVDQU		XMM7, [R8 + 96]

	MOVDQU		XMM8, [R9]
	MOVDQU		XMM9, [R9 + 16]
	MOVDQU		XMM10,[R9 + 32]
	MOVDQU		XMM11,[R9 + 48]
	MOVDQU		XMM12,[R9 + 64]
	;MOVDQU		XMM13,[R9 + 80]
	;MOVDQU		XMM14,[R9 + 96]

	SUBPS		XMM1, XMM8
	SUBPS		XMM2, XMM9
	SUBPS		XMM3, XMM10
	SUBPS		XMM4, XMM11
	SUBPS		XMM5, XMM12
	;SUBPS		XMM6, XMM13
	;SUBPS		XMM7, XMM14

	MULPS		XMM1, XMM1
	MULPS		XMM2, XMM2
	MULPS		XMM3, XMM3
	MULPS		XMM4, XMM4
	MULPS		XMM5, XMM5
	;MULPS		XMM6, XMM6
	;MULPS		XMM7, XMM7

	MOVDQU		[R12], XMM1
	MOVDQU		[R12 + 16], XMM2
	MOVDQU		[R12 + 32], XMM3
	MOVDQU		[R12 + 48], XMM4
	MOVDQU		[R12 + 64], XMM5
	;MOVDQU		[R12 + 80], XMM6
	;MOVDQU		[R12 + 96], XMM7

CMP 	R8, R10
JG		.ajustar
JE		.fin_resta_potencia

ADD		R8,  OFF
ADD		R9,  OFF
ADD		R12, OFF
JMP		.ciclo

.ajustar:
MOV 	R8, R10
MOV		R9, R11
MOV 	R12, R13
JMP		.ciclo
	

.fin_resta_potencia:
;-----------------INTEGRIDAD DE LA PILA--------------------------------
POP 	R13
POP 	R12
POP 	RBP
ret

sumar_elementos:
;--------------------------PARAMETROS----------------------------------
;RDI:		n
;RSI:		cant_vecinos
;RDX:		x
;requiere que n % 
;----------------------------------------------------------------------
PUSH	RBP
MOV  	RBP, RSP



POP		RBP
ret
sumar:

;--------------------------PARAMETROS----------------------------------
;RDI:		n
;RSI:		cant_vecinos
;RDX:		res 			
;RCX:		x 		
;R8:		y 			
;R9:		z
;----------------------------------------------------------------------

;-----------------------INTEGRIDAD DE LA PILA--------------------------
PUSH	RBP
MOV  	RBP, RSP
PUSH	R12
PUSH 	R13
;----------------------ORDENAMIENTO   PARAMETROS-----------------------
	
IMUL	RDI, RSI 		; cantidad de floats

LEA 	R10,  [RCX  + RDI * 4  - OFF]	; ultima posicion valida para leer/escribir
LEA 	R11,  [R8   + RDI * 4  - OFF]
LEA 	R12,  [R9   + RDI * 4  - OFF]
LEA 	R13,  [RDX  + RDI * 4  - OFF]

.ciclo_suma:

;Matriz x
	MOVDQU		XMM1, [RCX]
	MOVDQU		XMM2, [RCX + 16]
	MOVDQU		XMM3, [RCX + 32]
	MOVDQU		XMM4, [RCX + 48]
	MOVDQU		XMM5, [RCX + 64]
;Matriz y
	MOVDQU		XMM6, [R8]
	MOVDQU		XMM7, [R8 + 16]
	MOVDQU		XMM8, [R8 + 32]
	MOVDQU		XMM9, [R8 + 48]
	MOVDQU		XMM10,[R8 + 64]
;Matriz z	
	MOVDQU		XMM11, [R9]
	MOVDQU		XMM12, [R9 + 16]
	MOVDQU		XMM13, [R9 + 32]
	MOVDQU		XMM14, [R9 + 48]
	MOVDQU		XMM15, [R9 + 64]


	ADDPS		XMM1, XMM6
	ADDPS		XMM1, XMM11

	ADDPS		XMM2, XMM7
	ADDPS		XMM2, XMM12
	
	ADDPS		XMM3, XMM8
	ADDPS		XMM3, XMM13

	ADDPS		XMM4, XMM9
	ADDPS		XMM4, XMM14

	ADDPS		XMM5, XMM10
	ADDPS		XMM5, XMM15

	MOVDQU		[RDX], XMM1
	MOVDQU		[RDX + 16], XMM2
	MOVDQU		[RDX + 32], XMM3
	MOVDQU		[RDX + 48], XMM4
	MOVDQU		[RDX + 64], XMM5


CMP 	RCX, R10
JG		.ajustar_suma
JE		.fin_suma

ADD		RCX,  OFF
ADD		RDX,  OFF
ADD		R8, OFF
ADD		R9, OFF
JMP		.ciclo_suma

.ajustar_suma:
MOV 	RCX, R10
MOV		RDX, R13
MOV 	R8, R11
MOV 	R9, R12
JMP		.ciclo_suma
	

.fin_suma:
;-----------------INTEGRIDAD DE LA PILA--------------------------------
POP 	R13
POP 	R12
POP 	RBP
ret



;restar_y_al_cuadrado_ymm:
;;--------------------------PARAMETROS----------------------------------
;;RDI:		n
;;RSI:		cant_vecinos
;;RDX:		x 			--->	R8
;;RCX:		x_a 		--->	R9
;;R8:		r 			--->	R12
;;----------------------------------------------------------------------
;
;;-----------------------INTEGRIDAD DE LA PILA--------------------------
;PUSH	RBP
;MOV  	RBP, RSP
;PUSH	R12
;PUSH 	R13
;;----------------------ORDENAMIENTO   PARAMETROS-----------------------
;MOV 	R12, R8			;se guarda el vector auxiliar
;MOV 	R8, RDX
;MOV 	R9, RCX
;	
;IMUL	RDI, RSI 		; cantidad de floats
;
;LEA 	R10,[R8 + RDI * 4  - OFFYMM]	; ultima posicion valida para leer/escribir
;LEA 	R11,[R9 + RDI * 4  - OFFYMM]
;LEA 	R13,[R12 + RDI * 4 - OFFYMM]
;
;.ciclo2:
;
;	VMOVDQU		YMM1, [R8]
;	;VMOVDQU		YMM2, [R8 + 32]
;	;VMOVDQU		YMM3, [R8 + 64]
;	;VMOVDQU		YMM4, [R8 + 96]
;
;	VMOVDQU		YMM5,[R9]
;	;VMOVDQU		YMM6,[R9 + 32]
;	;VMOVDQU		YMM7,[R9 + 64]
;	;VMOVDQU		YMM8,[R9 + 96]
;
;	VSUBPS		YMM1, YMM1, YMM5
;	;VSUBPS		YMM2, YMM2, YMM6
;	;VSUBPS		YMM3, YMM3, YMM7
;	;VSUBPS		YMM4, YMM4, YMM8
;
;
;	VMULPS		YMM1, YMM1, YMM1
;	;VMULPS		YMM2, YMM2, YMM2
;	;VMULPS		YMM3, YMM3, YMM3
;	;VMULPS		YMM4, YMM4, YMM4
;
;
;	VMOVDQU		[R12], YMM1
;	;VMOVDQU		[R12 + 32], YMM2
;	;VMOVDQU		[R12 + 64], YMM3
;	;VMOVDQU		[R12 + 96], YMM4
;
;
;CMP 	R8, R10
;JG		.ajustar2
;JE		.fin_resta_potencia2
;
;ADD		R8,  OFFYMM
;ADD		R9,  OFFYMM
;ADD		R12, OFFYMM
;JMP		.ciclo2
;
;.ajustar2:
;MOV 	R8, R10
;MOV		R9, R11
;MOV 	R12, R13
;JMP		.ciclo2
;	
;
;.fin_resta_potencia2:
;;-----------------INTEGRIDAD DE LA PILA--------------------------------
;POP 	R13
;POP 	R12
;POP 	RBP
;ret