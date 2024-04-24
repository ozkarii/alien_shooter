
//ARM INSTRUCTIONS
//Instructions are conditional
//32-bit instruction


//How to load data from memory:
//LDR R0, =variable 	(Load address to register. You can use any global variable defined in main.c, or variable found here)
//LDR R1, [R0] 			(Load data from address, in this case 32-bit data)
//LDRB R1, [R0] 		(Load data from address, in this case 8-bit data)
//******
//LDR R0, =0x123456		(Load address to register, Use only if you know the memory address)
//LDR R1, [R0]			(load data from address, in this case 32-bit data)
//LDRB R1, [R0]			(load data from address, in this case 8-bit data)

//How to store data to memory:
//STR R1,[R0]			(R1 contains the data and R0 contains the address where  to store, 32-bit)
//STRB R1,[R0]			(R1 contains the data and R0 contains the address where  to store, 8-bit)


.data
	dir: .byte 1		//8-bit variable for direction
	pos: .byte 2        // Add position here
	led_addr: .word 0x41200000
.text

.global blinker


blinker:
	push {r0}				//Push used register to stack
	push {r1}
	push {r2}
	push {r3}				// temp register for varaible address
	
	ldr r0, =led_addr
	ldr r0, [r0]			// r0 contains the value of led register adress
	ldr r1, =pos
	ldrb r1, [r1]			// r1 contains position value
	ldr r2, =dir
	ldrb r2, [r2]			// r2 contains direction value
	
	cmp r1, #2
	blt right_edge
	cmp r1, #4
	bgt left_edge

	cmp r2, #1
	blt move_left
	cmp r2, #0
	bgt move_right

	b exit

exit:
	pop {r3}
	pop {r2}
	pop {r1}
	pop {r0}				//Pop register from stack, Remember the pop order if used more than one register (first pushed, last popped)
	bx lr					//Return to place where this function has been called.

left_edge:
	mov r2, #1
	ldr r3, =dir			// store dir variable address to r3
	strb r2, [r3]
	b move_right

right_edge:
	mov r2, #0
	ldr r3, =dir			// store position address to r3
	strb r2, [r3]
	b move_left

move_left:
	lsl r1, r1, #1			// shift led register value right by 1
	strb r1, [r0]
	ldr r3, =pos
	strb r1, [r3]
	b exit

move_right:
	lsr r1, r1, #1			// shift led register value left by 1
	strb r1, [r0]
	ldr r3, =pos
	strb r1, [r3]
	b exit
