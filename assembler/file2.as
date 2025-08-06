cmp mat    [r3    ]   [ r4  ],  #-5
clr W
jmp W1[r4][r4]
stop
rts
.mat  [     +4    ]     [      +55] 2,3,3 
.string "ab"
.data 1,+2,-3
mat: mov r1, r2
W: mov r4, r3
W1: mov r5, r6
