.method private hidebysig static 
	int32 b () cil managed 
{
	// Method begins at RVA 0x2074
	// Code size 26 (0x1a)
	.maxstack 2
	.locals init (
		[0] int32 a,
		[1] float32 b,
		[2] float32 c,
		[3] int32 x,
		[4] int32
	)

	IL_0000: nop
	IL_0001: ldc.i4.s 100
	IL_0003: stloc.0
	IL_0004: ldc.r4 0.7
	IL_0009: stloc.1
	IL_000a: ldloc.0
	IL_000b: conv.r4
	IL_000c: ldloc.1
	IL_000d: mul
	IL_000e: stloc.2
	IL_000f: ldloc.2
	IL_0010: conv.i4
	IL_0011: stloc.3
	IL_0012: ldloc.3
	IL_0013: stloc.s 4
	IL_0015: br.s IL_0017

	IL_0017: ldloc.s 4
	IL_0019: ret
} // end of method Program::b