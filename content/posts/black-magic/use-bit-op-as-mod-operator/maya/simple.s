mod_power_of_two(unsigned int, unsigned int):                 # @mod_power_of_two(unsigned int, unsigned int)
        lea     eax, [rsi - 1]
        and     eax, edi
        ret
mod(unsigned int, unsigned int):                               # @mod(unsigned int, unsigned int)
        xor     edx, edx
        mov     eax, edi
        div     esi
        mov     eax, edx
        ret
main:                                   # @main
        xor     eax, eax
        ret
