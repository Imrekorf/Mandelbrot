# mandelbrot

## Requirements
- GLFW  >= 3.3
- CMake >= 3.27
- System supporting bash

## Build instructions
```bash
git clone --recurse-submodules https://github.com/Imrekorf/Mandelbrot
cd mandelbrot
cmake --preset Release
cmake --build Release
```
After building applications will be available in ```Release/bin```

If you wish to build a debug version use the following commands after cloning the repo:
```bash
cd mandelbrot
cmake --preset Debug
cmake --build Debug
```

After building applications will be available in ```Debug/bin```