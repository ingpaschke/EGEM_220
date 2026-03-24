# EnhancedGEM Library v2.20

A comprehensive GEM (Graphics Environment Manager) UI library for Atari ST/TT/Falcon computers, written in C and 68k assembly.

**Author:** Christian Grunenberg, 1994
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
SOURCE/         Library source code (C + 68k assembly)
EXAMPLE/        Example programs
  DIALDEMO/     Dialog demonstrations
  WINDEMO/      Window demonstrations
  WINVIEW/      File viewer / clipboard viewer
  FONT/         Font palette and selector examples
  PLAY_IT/      Puzzle game example
  PREVIEW/      Resource file previewer
DIALOG.DEM/     Precompiled dialog demo applications
DOKU/           Documentation (German)
```

## Building

### Pure C (native Atari, 16-bit int)

Open `SOURCE/E_GEM.PRJ` in the Pure C IDE. For a minimal build, use `SOURCE/EGEMLGHT.PRJ`.

### GCC with -mshort (16-bit int)

```
cd SOURCE
make -f MAKEFILE.GCC
```

Produces `e_gem.olb`.

### GCC without -mshort (32-bit int)

```
cd SOURCE
make -f MAKEFILE.G32
```

Produces `e_gem32.olb`. See [README-32BIT.md](README-32BIT.md) for details.

### Lattice C (16-bit int)

```
cd SOURCE
make -f MAKEFILE.LCC
```

Produces `e_gem.lib`.

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
#include "e_gem.h"

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

Full documentation is in `DOKU/` (German):
- `MANUAL.TXT` — Complete reference manual
- `USAGE.TXT` — Usage guide
- `HISTORY.TXT` — Version history
- `E_GEM.TXT` — Release notes
