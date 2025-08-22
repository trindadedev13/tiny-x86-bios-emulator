[BITS 16]
[ORG  0x7C00]

Main:
        mov       ah, 0x0E
        mov       si, msg

        PrintLoop:
                  mov        al, [si]
                  cmp        al, 0x00
                  je         PrintLoop.Done
                  int        0x10
                  inc        si
                  jmp        PrintLoop

        PrintLoop.Done:
                  jmp        $

msg     db 'Hello, World!', 0x00

times   510-($-$$) db 0
dw      0xAA55