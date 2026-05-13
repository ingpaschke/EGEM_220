# EnhancedGEM — Modern GemLib Compatibility

This document describes how E_GEM works with the modern GemLib (`gem.h` / `libgem.a`) from the [freemint/gemlib](https://github.com/freemint/gemlib) project, as shipped with `m68k-atari-mint-gcc`.

## Background

E_GEM was written in 1994 against the old MiNT GEM library (`aesbind.h` / `vdibind.h`), which had a very different API from modern GemLib:

| Feature | Old MiNTLib | Modern GemLib |
|---------|-------------|---------------|
| Headers | `<aesbind.h>`, `<vdibind.h>` | `<gem.h>` (includes `<mt_gem.h>`) |
| `wind_set` | Variable arg count (2-6 args) | Typed macros: `wind_set_int`, `wind_set_str`, `wind_set_grect` |
| `evnt_timer` | Two args: `(lo_word, hi_word)` | One arg: `(milliseconds)` |
| `scrp_clear` | Takes `int` arg (clear all vs scrap only) | No-arg macro |
| Constants | `SELECTABLE`, `LASTOB`, `BLACK`, etc. | `OF_SELECTABLE`, `OF_LASTOB`, `G_BLACK` (old names via `__GEMLIB_OLDNAMES`) |
| `vq_gdos` | Returns `short` | `vq_vgdos()` returns `long` |
| `objc_sysvar` | Raw AES trap | `mt_objc_sysvar()` wrapper |

## Compatibility Layer

`dial.c` originally implemented `objc_sysvar`, `appl_search`, `appl_getinfo`, and `wind_xget` using raw AES parameter blocks (`INTIN[]`, `INTOUT[]`, `ADDRIN[]`) and the `__aes__()` trap function. These have been rewritten to use `mt_*()` calls directly, so no AES trap shim is needed under modern GemLib.

### `vq_gdos()` Wrapper

E_GEM expects `vq_gdos()` returning `short` (0 = no GDOS, positive = GDOS type). Modern GemLib provides `vq_vgdos()` returning `long` (-2 = no GDOS). The wrapper in `rc_ob_c.c` bridges this:

```c
short vq_gdos(void)
{
    long v = vq_vgdos();
    if (v == -2L || v == 0L)
        return 0;
    return (short)v;
}
```

### `wind_set` Variants

The original `wind_set()` accepted 2-6 arguments. Modern GemLib's `wind_set` macro requires exactly 6. E_GEM undefs the GemLib macro and all call sites use the appropriate typed wrapper:

| E_GEM call | Use case | Example |
|------------|----------|---------|
| `wind_set_int(h, f, v)` | Single value (WF_TOP, WF_BOTTOM, WF_BEVENT) | `wind_set_int(handle, WF_TOP)` |
| `wind_set_str(h, f, s)` | String pointer (WF_NAME, WF_INFO) | `wind_set_str(handle, WF_NAME, title)` |
| `wind_set6(h, f, a, b, c, d)` | Coordinate quad (WF_CURRXYWH, WF_ICONIFY) | `wind_set6(handle, WF_CURRXYWH, x, y, w, h)` |

### `evnt_timer` Conversion

Old code passes `evnt_timer(lo, hi)` with the timer value split across two 16-bit words. E_GEM redefines the macro to reconstruct the full value for modern GemLib:

```c
#undef evnt_timer
#define evnt_timer(a,b) mt_evnt_timer(((unsigned long)(b) << 16) + (unsigned short)(a), aes_global)
```

### Old-Style Constant Names

E_GEM uses 1990s-era constant names (`SELECTABLE`, `LASTOB`, `NORMAL`, `BLACK`, etc.). Modern GemLib provides these under the `__GEMLIB_OLDNAMES` guard. E_GEM defines this before including `gem.h`:

```c
#define __GEMLIB_OLDNAMES
#include <gem.h>
```

### Macro Conflicts

Several GemLib macros conflict with E_GEM's own declarations. These are undef'd after including `gem.h`:

```c
#undef scrp_clear      /* E_GEM's version takes an int arg */
#undef appl_getinfo    /* E_GEM wraps with mt_appl_getinfo directly */
#undef appl_xgetinfo   /* E_GEM provides its own with capability detection */
#undef objc_sysvar     /* E_GEM wraps with mt_objc_sysvar */
#undef appl_search     /* E_GEM wraps with mt_appl_search */
#undef wind_set        /* replaced by wind_set_int/wind_set_str/wind_set6 */
#undef wind_get        /* replaced by direct mt_wind_get call */
#undef AES_VERSION     /* E_GEM uses gl_ap_version directly */
#undef evnt_timer      /* E_GEM converts 2-arg to 1-arg form */
#undef vq_gdos         /* E_GEM provides short wrapper */
```

### Constants Guarded Against Double Definition

GemLib may already define `WF_BEVENT`, `WF_ICONIFY`, `FL3DMASK`, `SC_CHANGED`, etc. E_GEM wraps its own definitions in `#ifndef` guards to avoid conflicts, and uses separate `#ifndef` guards for sub-groups (e.g., `DD_OK` is guarded independently from `WF_ICONIFY`).

## Build Configuration

The Makefile passes these flags for modern GemLib compatibility:

```makefile
CFLAGS = -Wall -O2 -fomit-frame-pointer -fno-strict-aliasing \
         -Wno-incompatible-pointer-types -Wno-int-conversion \
         -Wno-pointer-to-int-cast -Wno-implicit-function-declaration \
         -D__CREATE_E_GEM_LIB__ -D__TCC_COMPAT__
```

| Flag | Reason |
|------|--------|
| `-fno-strict-aliasing` | Legacy `*(long *)&short_pair` type-punning patterns |
| `-Wno-incompatible-pointer-types` | `int *` vs `short *` in VDI calls (same size with `-mshort`, harmless with 32-bit `int`) |
| `-Wno-int-conversion` | `long` ↔ pointer conversions in TOS system calls (`Malloc`, `Mxalloc`, etc.) |
| `-Wno-pointer-to-int-cast` | Resource init code casts pointers to array indices |
| `-Wno-implicit-function-declaration` | `vst_arbpt` (SpeedoGDOS function in `gemx.h`, declared but not prototyped in all paths) |
| `-D__CREATE_E_GEM_LIB__` | Enables library-internal code paths |
| `-D__TCC_COMPAT__` | Enables TurboC/PureC compatible resource initialization |

## Files

| File | Role |
|------|------|
| `rc_ob_c.c` | `vq_gdos()` wrapper, C rectangle operations |
| `e_gem.h` | All `#undef` / `#define` compatibility macros |

## Remaining Warnings

The build produces ~170 warnings, mostly:
- Macro redefinitions (`MU_MX`, `BUSYBEE`, `wind_set_int`, `wind_set_str`) — harmless, same expansion
- `void main()` in example programs — cosmetic (should be `int main`)
- Pointer-to-int-cast in resource initialization — safe for 32-bit m68k
- Incompatible pointer types in VDI calls — `int *` vs `short *` passed to GemLib `short *` parameters; values fit in 16 bits on Atari

None of these affect correctness.
