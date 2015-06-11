ldr r0, =0x20200000 ; base GPIO Address
mov r2, #1          ; r2 = 1
lsl r2, #18         ; r2 = 0x20000
str r2, [r0, #4]    ; set Pin 16 as output

mov r1, #1
lsl r1, #16

mov r2, #1          ; isON = true

loop:

cmp r2, #0          ; if !isON
bne else
str r1, [r0, #28]   ; set Pin 16
b endif
else:
str r1, [r0, #40]   ; clear Pin 16
endif:
eor r2, r2, #1      ; toggle isON

mov r3, #0x2e0000   ; set delay = 0x2e0000
delay:              ; begin delay
sub r3, r3, #1
cmp r3, #0
bne delay           ; end delay

b loop              ; and repeat
