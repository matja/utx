; This sets IRQ00-0F vectors in the 8259s to be IntF0 thru FF.
;_______________________________________________________________________________
;
; When the PICUs are initialized, all the hardware interrupts are MASKED.
; Each driver that uses a hardware interrupt(s) is responsible
; for unmasking that particular IRQ.
;
;F0	IRQ 0	8254 Timer
;F1	IRQ 1 	Keyboard (8042)
;F2	IRQ 2	Cascade from PIC2 (handled internally)
;F3	IRQ 3 	COMM 2 Serial port
;F4	IRQ 4	COMM 1 Serial port
;F5	IRQ 5	Line Printer 2
;F6	IRQ 6	Floppy disk controller
;F7	IRQ 7	Line Printer 1
;
;F8	IRQ 8	CMOS Clock
;F9	IRQ 9	?
;FA	IRQ 10	?
;FB	IRQ 11	?
;FC	IRQ 12	?
;FD	IRQ 13	Math coprocessor
;FE	IRQ 14	Hard disk controller
;FF	IRQ 15	?
;_______________________________________________________________________________
;
;INPUT
; none
;
;OUTPUT
; none
;_______________________________________________________________________________


INIT_PIC:

	push eax

	mov al,00010001b
	out PIC1+0,al          	;ICW1 - MASTER
	push eax
	pop eax
	out PIC2+0,al          	;ICW1 - SLAVE
	push eax
	pop eax
	mov al,0xf0		;IRQ0 interrupt
	out PIC1+1,al          	;ICW2 - MASTER
	push eax
	pop eax
	mov al,0xf8		;IRQ8 interrupt
	out PIC2+1,al          	;ICW2 - SLAVE
	push eax
	pop eax
	mov al,00000100b
	out PIC1+1,al          	;ICW3 - MASTER
	push eax
	pop eax
	mov al,00000010b
	out PIC2+1,al          	;ICW3 - SLAVE
	push eax
	pop eax
	mov al,00000001b
	out PIC1+1,al          	;ICW4 - MASTER
	push eax
	pop eax
	out PIC2+1,al          	;ICW4 - SLAVE
	push eax
	pop eax
	mov al,11111011b  	;Mask all but cascade
	out PIC1+1,al          	;MASK - MASTER (0= Ints ON)
	push eax
     pop  eax
     mov  al, 11111111b       ; Mask all
     out  PIC2+1, al          ; MASK - SLAVE


     mov  al, 0x20            ; clear PIC
     out  PIC2, al
     out  PIC1, al

     pop  eax
     ret

