ldr r0, =0x20200000
mov r2, #1
lsl r2, #18
str r2, [r0, #4]

mov r1, #1
lsl r1, #16

mov r2, #1

loop:

cmp r2, #0
bne else
str r1, [r0, #28]
b endif
else:
str r1, [r0, #40]
endif:
eor r2, r2, #1

mov r3, #0x2e0000
delay:
sub r3, r3, #1
cmp r3, #0
bne delay

b loop
