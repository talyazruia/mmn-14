; file: demo_valid.as
.entry START
.extern EXT1
.extern PRINT

; --- Macros  ---
mcro PUSHTWO
    inc r1        
    dec r2        
mcroend



; --- Data segment ---
STR:    .string    "Hello"             
ARR:    .data    5, -3, 0, 12 
mcro ZERO_R3
    clr r3          
mcroend      
; --- End data ---

; --- Code segment ---
START:    mov    #5, r1               
    lea    MAT, r2              
    PUSHTWO                    
    add    r1, r2               
    jsr    PRINT                 
    prn    r1                   
    bne    LOOP                 
    
MAT:    .mat    [2][3] 1, 2, 3, 4
    mov    MAT[ r1 ][ r2 ], r0  

    jmp    END

ZERO_R3                 
LOOP:    cmp    #0, r3               
    bne    LOOP                  

    mov    EXT1, r4             
    prn    r4
    jmp    END

END:    red    r0                   
    prn    r0
