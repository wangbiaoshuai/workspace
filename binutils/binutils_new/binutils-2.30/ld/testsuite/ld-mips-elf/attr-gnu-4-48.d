#source: attr-gnu-4-4.s -W
#source: attr-gnu-4-8.s -W
#ld: -r
#error: \A[^\n]*: [^\n]* linking -mfp32 module with previous -mfp64 modules\n
#error:   [^\n]*: Warning: .* uses -mips32r2 -mfp64 \(12 callee-saved\) \(set by .*\), .* uses unknown floating point ABI 8\n
#error:   [^\n]*: failed to merge target specific data of file [^\n]*\.o\Z
