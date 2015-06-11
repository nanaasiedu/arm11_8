; comment test file
mov r1, #14 ;move 14 into r1
mov r2, #0x1c
mov r3, r2, asr #2 ; move r2 and shifted
;try with ldr (notice no space after ';')
ldr r4, =4000000
ldr r5, =0xabcdef ;
;
andeq r0, r0, r0
