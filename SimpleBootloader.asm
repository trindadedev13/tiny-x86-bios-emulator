[BITS 16]
[ORG  0x7C00]

Main:
        mov       ah, 0x0E ; func teletype output
        mov       si, msg  ; si = msg
        mov       al, [si] ; al = *si


        PrintLoop:
                  int        0x10     ; draw

                  inc        si       ; si++
                  mov        al, [si] ; al = *si

                  cmp        al, 0x00 ; al == 0x00(null)
                  je         PrintLoop.Done

                  jmp        PrintLoop

        PrintLoop.Done:
                  jmp        $

msg     db 'Hello, World!', 0x00

times   510-($-$$) db 0
dw      0xAA55