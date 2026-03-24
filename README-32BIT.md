# EnhancedGEM — 32-bit int Support

This document describes the changes made to support compilers where `sizeof(int) == 4`.

## Background

The original EnhancedGEM code assumed `sizeof(int) == 2` throughout, as was standard for Atari ST compilers (Pure C, GCC with `-mshort`, Lattice C with `-w`). This made the code incompatible with modern GCC builds without `-mshort`, where `int` is 32 bits.

## Approach

A new type `INT16` is introduced in `E_GEM.H`:

```c
#include <limits.h>
#if SHRT_MAX == INT_MAX
typedef int INT16;       /* -mshort: int is already 16-bit */
#else
typedef short INT16;     /* 32-bit int: use short for 16-bit data */
#endif
```

- With `-mshort`: `INT16` = `int` (zero overhead, identical to original code)
- Without `-mshort`: `INT16` = `short` (correct 16-bit type for AES/VDI)

Detection is automatic via `<limits.h>` — no manual flags required.

## What Uses INT16

All data that must be 16 bits for the GEM AES/VDI interface:

| Category | Examples |
|----------|----------|
| AES message buffers | `INT16 msg[8]`, `XEVENT.ev_mmgpbuf`, `MESSAG.msg` |
| VDI coordinate arrays | `INT16 pxy[8]`, `INT16 old[4]` |
| VDI workstation data | `INT16 work_out[57]`, `INT16 work_in[11]` |
| VDI attribute arrays | `INT16 attrib[10]`, `INT16 extent[8]` |
| Bitmap pixel data | All icon/image arrays in `IMAGE.C`, `ALERTS.C` |
| Image data pointers | `RS_IMDOPE.image`, `BUTTON.but_on`, `IMAGE.hi_on` |
| Mouse cursor data | `INT16 mform[]` in `GRAF.C` |
| Resource data | `WORD` (redefined as `INT16` in `RSC.H`) |

## Patterns Fixed

### 1. `*(long *)&int_pair` aliasing

The original code copied two adjacent 16-bit `int` fields as a single 32-bit `long`:

```c
/* Original — assumes sizeof(int) == 2 */
*(long *) &event->ev_mm1x = *(long *) &event->ev_mmox;
```

Replaced with the `COPY_2INTS` macro:

```c
COPY_2INTS(event->ev_mm1x, event->ev_mm1y, event->ev_mmox, event->ev_mmoy);
```

The macro uses `*(long *)` when int is 16-bit (fast path) and explicit two-field assignment when int is 32-bit.

### 2. Mixed-width `*(long *)` copies

Copying from INT16 message arrays to `int` struct fields:

```c
/* Original */
*(long *) &dd.dd_mx = *(long *) &msg[4];

/* Fixed */
dd.dd_mx = msg[4]; dd.dd_my = msg[5];
```

### 3. Assembly replacement

The 68k assembly `RC_OB.S` uses hardcoded 16-bit word offsets and `move.w` instructions, making it incompatible with 32-bit `int` parameters. C implementations of `rc_copy`, `rc_equal`, `rc_intersect`, and `rc_inside` are provided in `RC_OB_C.C` and compiled automatically when `SHRT_MAX != INT_MAX`.

### 4. Pointer packing in AES messages

Patterns like `*(char **) &msg[3]` pack a 32-bit pointer across two 16-bit message slots. These continue to work unchanged because `msg` is `INT16` (always 16-bit), so two adjacent elements still form a pointer-sized value.

## Building

```
cd SOURCE
make -f MAKEFILE.G32
```

This produces `e_gem32.olb` — a library built without `-mshort`.

## Backward Compatibility

The original 16-bit int builds are completely unchanged:

- `INT16` resolves to `int` (zero overhead)
- `COPY_2INTS` uses the original `*(long *)` trick
- Assembly `RC_OB.S` is still used by Pure C builds
- All existing Makefiles and `.PRJ` files work as before

## Files Modified

| File | Changes |
|------|---------|
| `E_GEM.H` | INT16 typedef, COPY_2INTS macro, struct fields, function prototypes |
| `RSC.H` | `WORD` redefined as `INT16` |
| `PROTO.H` | MESSAG struct, function prototypes |
| `RC_OB_C.C` | C implementations of rc_copy/rc_equal/rc_intersect, widened guards |
| `IMAGE.C` | Bitmap arrays `int[]` → `INT16[]` |
| `ALERTS.C` | Alert icon data → `INT16[]`, unified `I_CAST` |
| `GRAF.C` | Mouse form data, COPY_2INTS for event fields |
| `EVENT.C` | Message buffers, event field copies, function params |
| `WINDOW.C` | Message buffers, VDI arrays |
| `DIAL.C` | work_out/ext_work_out, message buffers, VDI arrays |
| `POPUP.C` | COPY_2INTS for event fields |
| `TOOL.C` | Message buffers, VDI arrays, clipping arrays |
| `XACC_AV.C` | Message buffers, function params |
| `DRAGDROP.C` | Message buffers, mixed-width copy fix |
| `FSELECT.C` | Message buffers, VDI arrays, clipping arrays |
| `FILETOOL.C` | Message buffer |
| `OBJC.C` | VDI query arrays |

## New File

| File | Purpose |
|------|---------|
| `MAKEFILE.G32` | GCC build without `-mshort`, produces `e_gem32.olb` |
