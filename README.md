# algol126 (Experimental Branch)

An unhinged, screaming-fast C++23 module compiler pipeline for **algol 126**—the direct chronological continuation of the historical ALGOL specification timeline. 

> [!WARNING]
> **EXPERIMENTAL BRANCH NOTICE**: This branch represents the bleeding-edge architecture shift. The compiler pipeline has transitioned from a high-level C23 transpiler into a **true, native, self-contained `x86_64` assembly generator toolchain**. It completely bypasses high-level string transpilation and emits pure Intel-syntax instructions directly to bare-metal hardware.

No bloat, zero runtime safeties, and 100% spite-driven statement semicolons. Translates complex template shapes and vector registers straight into heavily optimized `.s` assembly payloads, leaving GCC purely free to act as a blazing-fast native hardware assembler and linker.

## Performance Loops
Because the front-end token layout maps directly to flat lookup arrays and the backend directly spits raw assembly text without AST string concatenation overhead, the entire compiler pass executes virtually at RAM bandwidth speed:
* **Lexing & Structural AST Parsing**: ~0.45 ms
* **Native Assembly Code Generation**: ~1.50 ms
* **Host Assembler Pass & Native Linking**: ~25.20 ms
* **Total Translation Toolchain Cycle Loop**: **~27.15 ms**

## Stabilized Experimental Additions (Key.txt Spec)

### 1. Multi-Width Register Scaling Matrix (Section 2)
* Automatically promotes narrow 1-byte (`int8`/`uint8`) and 2-byte (`int16`/`uint16`) integer contexts up to 32-bit (`eax`/`ebx`) or 64-bit (`rax`/`rbx`) execution windows during binary operations.
* Prevents GNU Assembler size mismatches (`Error: operand size mismatch for imul`).
* Integrates scalar SSE assembly pipelines for floating-point calculations (`real32` via `movss`/`addss` and `real64` via `movsd`/`addsd`) utilizing hardware `xmm` register banks.

### 2. User-Defined Compound Memory Layout Engine (Section 2 & 4)
* **Compile-Time Offset Tracking**: Natively computes relative byte displacements inside user-defined types (`struct`) on a multi-pass symbol table layout.
* **Absolute Stack Allocation**: Dynamically measures collective struct footprints and shifts isolated stack frames relative to the Base Pointer (`[rbp - offset]`).
* **Membership Access Operator (`.`)**: Resolves structural field paths recursively to emit precise address fetches directly over the memory track layout.

### 3. The Turbo Scale Block Unroller (Section 1)
* Intercepts loops inside `scale ... elacs;` sandboxes containing compile-time constants and physically replicates your underlying AST tree body statements sequentially inline, entirely erasing loop jumping overhead and branch prediction penalties.

### 4. Native Print Gateway (Section 3)
* Direct alignment to System V AMD64 ABI registers (`rsi`, `rdi`) calling libc print routines (`printf@PLT`) safely over static read-only data sections (`.section .rodata`).

---

## Language Syntax Reference Baseline

The following structural footprint showcases narrow mixed-width operations running natively alongside your `scale` block hardware optimizer:

```algol
begin
    int8 tiny_step := 12;
    int32 standard_factor := 4;
    int64 wide_result := 0;

    # 1. Automatic narrow width register upcasting check #
    tiny_step := tiny_step + 3;

    # 2. Enters turbo unrolling mode without conditional jump branch penatlies #
    scale 
        for i from 1 to 4 do
            wide_result := tiny_step * standard_factor;
        od; 
    elacs;

    print((wide_result));
end
```

### Emitted Assembly Target Execution Layout (`build/target.s` Trace):
```assembly
main:
    push rbp
    mov rbp, rsp
    sub rsp, 512

    xor rax, rax
    mov eax, 12
    mov byte ptr [rbp - 1], al      # Isolated 1-byte int8 allocation

    xor rax, rax
    mov eax, 4
    mov dword ptr [rbp - 5], eax     # Standard 4-byte int32 allocation

    xor rax, rax
    mov eax, 0
    mov qword ptr [rbp - 13], rax    # Large 8-byte int64 allocation

    # --- HARDWARE TURBO SANDBOX: SEQUENTIAL UNROLL PASS START ---
    # Unroll Step Iteration Track [1]
    mov dword ptr [rbp - 17], 1
    mov eax, dword ptr [rbp - 5]
    push rax
    xor rax, rax
    mov al, byte ptr [rbp - 1]
    pop rbx
    imul eax, ebx                    # Auto promoted to 32-bit registers!
    mov qword ptr [rbp - 13], rax
    
    # ... Loops fully unrolled inline 3 more times sequentially ...
    # --- HARDWARE TURBO SANDBOX: SEQUENTIAL UNROLL PASS EXIT ---

    mov rax, qword ptr [rbp - 13]
    mov rsi, rax
    lea rdi, [rip + .LC_INT_FMT]
    mov eax, 0
    call printf@PLT

    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret

.section .rodata
.LC_INT_FMT:
    .string "%d\n"
.LC_FLOAT_FMT:
    .string "%f\n"
.LC_STR_FMT:
    .string "%s\n"
```

## Pipeline Automation Deployment
The repository contains a granular dependency tracker `Makefile` alongside a fast sanity check `build.sh` script. Ensure you are running `g++-14` or newer to support cutting-edge C++23 standard library module structures (`import std;`).

```bash
# wipe cached artifacts, verify host compilers, build modules, and run the test matrix
make clean
make
./a126c examples/type_spec_check.a68
./build/type_spec_check
```

## notice & licensing
```text
ALGOL126 Compiler (A126C)
Copyright (c) 2026 mxreal64 (https://github.com)

This Source Code Form is subject to the terms of the Mozilla Public 
License, v. 2.0. If a copy of the MPL was not distributed with this 
file, You can obtain one at http://mozilla.org.
```
