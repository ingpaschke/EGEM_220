# EnhancedGEM Library v2.20

A comprehensive GEM (Graphics Environment Manager) UI library for Atari ST/TT/Falcon computers, originally written in C and 68k assembly by Christian Grunenberg (1994). Adapted for modern cross-compilation with `m68k-atari-mint-gcc` and the current GemLib.

**Original author:** Christian Grunenberg, 1994
**License:** Freeware (including all source code and example programs)

## Overview

EnhancedGEM extends the native GEM AES/VDI APIs into a higher-level, event-driven GUI toolkit. It provides window management, dialog handling, popup menus, font selection, drag & drop, inter-application communication, and much more.

## Features

- **Window management** — open, close, resize, scroll, iconify, with automatic redraw clipping
- **Dialog system** — flying, windowed, and modal dialogs with edit fields, sliders, and popups
- **Popup menus** — standard, cycle, checkbox/radio styles with keyboard navigation
- **Font selector** — fully parametric dialog with preview, effects, and color selection
- **Event handling** — extended event model with 4 mouse areas, 4 timers, dual mouse buttons
- **Object enhancements** — checkboxes, radio buttons, hotkeys, headers, cycle buttons, and more
- **Drag & Drop** — MiNT D&D protocol and AV protocol support
- **Inter-app communication** — XAcc 2.0, AV, and PAULA protocols
- **File utilities** — wildcard matching, path parsing, file selector wrapper, settings persistence
- **Clipboard** — scrap path management and change notifications
- **Cookie jar** — read, write, create, and remove TOS cookies
- **Conditional compilation** — over 15 feature flags allow aggressive size reduction for minimal builds

## Directory Structure

```
source/         Library source code
  aes_compat.c  AES compatibility shim for modern GemLib
  e_gem.h       Main library header
  proto.h       Internal prototypes
  makefile.gcc  GCC cross-compilation Makefile
example/        Example programs
  dialdemo/     Dialog demonstrations
  windemo/      Window demonstrations (sample, hello, mouse, lines, balls)
  winview/      File viewer / clipboard viewer
  font/         Font palette and selector examples
  play_it/      Puzzle game example
  preview/      Resource file previewer
doku/           Documentation (German)
```

## Building

### Cross-compilation with m68k-atari-mint-gcc (recommended)

Requires `m68k-atari-mint-gcc` with MiNTLib and GemLib installed.

```sh
cd source
make -f makefile.gcc            # build the library
make -f makefile.examples       # build example programs
```

Produces `libe_gem.a` and example `.tos` executables. Builds with 32-bit `int` (no `-mshort`) using the modern GemLib (`gem.h`). See [README-GEMLIB.md](README-GEMLIB.md) for details on GemLib compatibility.

### Pure C (native Atari, 16-bit int)

Open `source/e_gem.prj` in the Pure C IDE. For a minimal build, use `source/egemlght.prj`.

### Linking

```sh
m68k-atari-mint-gcc -o myapp.tos myapp.c -I source -L source -le_gem -lgem
```

## Light Version

Define `SMALL_LIGHT` to disable all optional features, or selectively disable individual features:

| Flag | Disables |
|------|----------|
| `SMALL_NO_XACC_AV` | XAcc/AV protocol |
| `SMALL_NO_EDIT` | Text editing fields |
| `SMALL_NO_DD` | Drag & Drop |
| `SMALL_NO_GRAF` | Graphics/slider functions |
| `SMALL_NO_POPUP` | Popup menus |
| `SMALL_NO_FONT` | Font handling |
| `SMALL_NO_SCROLL` | Window scrolling |
| `SMALL_NO_CLIPBRD` | Clipboard support |
| `SMALL_NO_MENU` | Menu system |
| `SMALL_NO_ICONIFY` | Window iconification |
| `SMALL_NO_XTEXT` | Extended text rendering |
| `SMALL_ALERT` | Minimal alert icons (8 instead of 17) |

## Quick Start

```c
#include <e_gem.h>

int Init(XEVENT *ev, int avail) {
    return (MU_MESAG | MU_KEYBD) & avail;
}

int Event(XEVENT *ev) {
    if (ev->ev_mwich & MU_MESAG) {
        switch (ev->ev_mmgpbuf[0]) {
            case AP_TERM: exit_gem(TRUE, 0); break;
        }
    }
    return ev->ev_mwich;
}

int main(void) {
    if (init_gem(NULL, "  MyApp", "MyApp", NULL, 0, 0, 0)) {
        Event_Handler(Init, Event);
        Event_Multi(NULL);
        exit_gem(TRUE, 0);
    }
    return -1;
}
```

## Documentation

- [README-GEMLIB.md](README-GEMLIB.md) — Modern GemLib compatibility layer
- [README-32BIT.md](README-32BIT.md) — 32-bit `int` support details
- `doku/manual.txt` — Complete reference manual (German)
- `doku/usage.txt` — Usage guide (German)
- `doku/history.txt` — Version history
