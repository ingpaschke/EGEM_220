# EnhancedGEM — 32-bit int Support

This document describes how the library supports compilers where `sizeof(int) == 4`.

## Background

The original EnhancedGEM code (1994) assumed `sizeof(int) == 2` throughout, as was standard for Atari ST compilers (Pure C, GCC with `-mshort`, Lattice C with `-w`). Modern `m68k-atari-mint-gcc` without `-mshort` uses 32-bit `int`, which required systematic adaptation.

## How It Works

The library builds with 32-bit `int` by default. All data that must be 16 bits for the GEM AES/VDI interface uses `short` explicitly.

Key adaptations:

- **AES message buffers** — `short msg[8]` throughout (AES messages are 8 x 16-bit words)
- **VDI coordinate arrays** — `short pxy[]`, `short attrib[]`, `short work_out[]`, etc.
- **Bitmap pixel data** — `short` arrays for all icons, images, and mouse cursors
- **Pointer packing** — `*(char **) &msg[3]` packs a 32-bit pointer across two 16-bit message slots; this works because `short` is always 16 bits regardless of `int` size
- **Assembly replacement** — C implementations of `rc_copy`, `rc_equal`, `rc_intersect`, `rc_inside` replace the 68k assembly `rc_ob.s` (which uses hardcoded 16-bit offsets)
- **`*(long *)&pair` aliasing** — the original code copied two adjacent `int` fields as a single `long`; replaced with explicit field-by-field copies where the fields are now `int` (32-bit)

## Build Flags

The Makefile uses `-fno-strict-aliasing` because legacy code patterns (type-punning through `long *` casts on `short` field pairs in GRECT/OBJECT structs) are technically undefined behavior under C99 strict aliasing rules but are standard practice in GEM programming. This flag ensures GCC does not miscompile these patterns.

## Backward Compatibility

The original 16-bit int toolchains (Pure C, GCC with `-mshort`, Lattice C) are unaffected:

- Pure C uses its own `.prj` files and 68k assembly `rc_ob.s`
- GCC with `-mshort` can still build using the same source (add `-mshort` to CFLAGS)
- All `short` declarations are equivalent to `int` when both are 16-bit
