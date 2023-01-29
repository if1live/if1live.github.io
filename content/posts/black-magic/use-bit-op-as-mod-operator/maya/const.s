mod_16(unsigned int):                             # @mod_16(unsigned int)
        and     edi, 15
        mov     eax, edi
        ret
mod_16_simple(unsigned int):                     # @mod_16_simple(unsigned int)
        and     edi, 15
        mov     eax, edi
        ret
main:                                   # @main
        xor     eax, eax
        ret
