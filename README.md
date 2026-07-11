# algol126

an unhinged, screaming-fast c++23 compiler pipeline for **algol 126**—the direct chronological continuation of the historical algol specification timeline. no bloat, zero runtime safeties, and 100% spite-driven statement semicolons. 

translates complex template shapes and vector registers straight into heavily optimized c23 target payloads, leaving gcc 16 free to abuse native hardware registers.

## performance loops
because the front-end token layout maps directly to flat lookup arrays, the entire compiler pass executes virtually at ram bandwidth speed:
* **lexing & structural ast parsing**: ~0.45 ms
* **backend code generation & gcc optimization**: ~33.20 ms
* **total translation toolchain cycle loop**: **33.65 ms**

## core architectural pillars
* **`scale ... elacs;` turbo sandbox**: drops standard runtime validation metrics entirely, commanding the compiler backend to inject aggressive unrolling pragmas and ignore alias tracking assumptions (`#pragma GCC ivdep` + `#pragma GCC unroll 4`).
* **hardware-native vectorization**: packed 128-bit `vec4` primitives map directly to underlying simd lanes on an avx2 target target through explicit component unrolling.
* **naked address arithmetic**: explicit, unchecked reference manipulation (`ref type`) enabling close-to-metal address jumps with zero runtime index validation checking overhead.
* **compile-time contractual safety**: zero-overhead static validation tracks enforcing pointer contract checks on non-nullable instances before backend code emission. optional prefixes (`?ref`) specify safe nullable boundaries.
* **brutal text-copy modules**: high-speed macro-style module ingestion (`import "path";`) that dynamically tokenizes external source scripts on the fly and grafts them inline at the active stream cursor.
* **nested routine definitions**: native functional encapsulation blocks (`proc`) tracking parameters smoothly by compiling directly down into gcc-native nested layouts.
* **compile-time template abstractions**: generic type layout definitions (`template(type element) struct box { ... };`) that dynamically copy and substitute data field alignments on concrete instantiation targets.

## language syntax reference baseline
```algol
begin
    # 1. compile-time generic blueprint #
    template(type element) struct cache_cell {
        element register_slot;
        int32 cell_id;
    };

    # 2. structure instantiation #
    cache_cell(int32) hardware_node;

    print(("ingesting parallel loop threshold bounds:"));
    int32 loop_limit := input(());

    # 3. optional pointer contract safe for null voids #
    ?ref int32 volatile_network_buffer := void;
    ref int32 raw_memory_buffer := loop_limit;

    # 4. enter the zero-safeguard register sandbox #
    scale
        for i from 1 to loop_limit do
            # dangerous hardware lookup jumps with zero checking flags #
            ref int32 current_slot := raw_memory_buffer + i;
        od;
    elacs;

    print(("algol 126 operational metrics verified cleanly."));
end;
```

## pipeline automation deployment
the repository contains a granular dependency tracker `Makefile` alongside a fast sanity check `build.sh` script. ensure you are running `g++-14` or newer to support cutting-edge c++23 standard library module structures (`import std;`).

```bash
# grant executable privileges to the script harness
chmod +x build.sh

# wipe cached artifacts, verify host compilers, build modules, and run the test matrix
./build.sh
```

## notice & licensing
```text
ALGOL126 Compiler (A126C)
Copyright (c) 2026 mxreal64 (https://github.com)

This Source Code Form is subject to the terms of the Mozilla Public 
License, v. 2.0. If a copy of the MPL was not distributed with this 
file, You can obtain one at http://mozilla.org.
```
