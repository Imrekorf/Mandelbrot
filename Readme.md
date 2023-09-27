# mandelbrot

## Requirements
- GLFW  >= 3.3
- CMake >= 3.27
- System supporting bash

## Build instructions
```bash
git clone https://github.com/Imrekorf/Mandelbrot
cd mandelbrot
cmake --preset=<Release/Debug>
cd <Release/Debug> && make -j$(nproc)
```

After building applications will be available in ```<Release/Debug>/bin```