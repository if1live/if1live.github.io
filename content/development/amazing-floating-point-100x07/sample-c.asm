# https://godbolt.org/g/4m8F4S
# x86-64 clang 5.0.0

.LCPI0_0:
  .long 1060320051 # float 0.699999988
main: # @main
  push rbp
  mov rbp, rsp
  sub rsp, 32
  movabs rdi, .L.str
  movss xmm0, dword ptr [rip + .LCPI0_0] # xmm0 = mem[0],zero,zero,zero
  mov dword ptr [rbp - 4], 0
  mov dword ptr [rbp - 8], 100
  movss dword ptr [rbp - 12], xmm0
  cvtsi2ss xmm0, dword ptr [rbp - 8]
  mulss xmm0, dword ptr [rbp - 12]
  cvttss2si eax, xmm0
  mov dword ptr [rbp - 16], eax
  mov esi, dword ptr [rbp - 16]
  mov al, 0
  call printf
  xor esi, esi
  mov dword ptr [rbp - 20], eax # 4-byte Spill
  mov eax, esi
  add rsp, 32
  pop rbp
  ret
.L.str:
  .asciz "%d"
