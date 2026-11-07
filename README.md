# Pixel Game

A small C11 pixel-game starting point using [Fenster](https://github.com/zserge/fenster) for the window, framebuffer, input and timing. The example draws at 160 × 120 and scales each pixel 4×. Use the arrow keys to steer the snake towards the green food, P to pause or resume, and Q to quit; eating makes it grow and briefly flashes the background red. The snake wraps around the edges, and direct reversals are ignored. Moving into any currently occupied body cell ends the game after three red flashes. Change `BODY_SEGMENTS` in `src/main.c` to adjust its initial length (the head is separate).

## Requirements

- GCC, Make and VS Code with the recommended C/C++ and CodeLLDB extensions.
- macOS: Xcode Command Line Tools (`xcode-select --install`) for the SDK and Cocoa framework. Apple's `/usr/bin/gcc` is Clang; install GCC separately (for example, `brew install gcc`) if you require GCC. Homebrew names it with a version, such as `gcc-15`.
- Linux: X11 development headers and libraries (for example, `libx11-dev` on Debian/Ubuntu), GCC, Make and GDB for the GDB launch configuration.
- Windows: MinGW-w64 GCC and a Unix-like shell with Make, such as MSYS2. The provided debugger configurations target macOS and Linux; adjust the executable path and debugger for Windows.

## Build and run

```sh
make
make run
make clean
```

The compiler defaults to `gcc`. If GCC has a versioned name, use `make CC=gcc-15` (substitute your installed version), or set `CC` in your environment for VS Code tasks as well. The executable is `build/pixel-game` (`.exe` on Windows).

Open this folder in VS Code. Use **Terminal → Run Build Task** to build, **Terminal → Run Task → run** to run, or F5 and select the LLDB configuration on macOS or GDB on Linux. Start VS Code with `CC` set if your GCC has a versioned name. The C/C++ extension can select the same compiler through its IntelliSense configuration.

Edit `src/main.c` to start making the game. Add other `.c` and `.h` files in `src`; Make rebuilds when they change. Fenster is vendored in `vendor/fenster`, with its MIT license, from upstream commit `e700581dfb7956dd161aee44fc0cff0663e789a1`.
