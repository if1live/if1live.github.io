mod_256(unsigned int):                            # @mod_256(unsigned int)
        movzx   eax, dil
        ret
mod_256_simple(unsigned int):                    # @mod_256_simple(unsigned int)
        movzx   eax, dil
        ret
main:                                   # @main
        xor     eax, eax
        ret
