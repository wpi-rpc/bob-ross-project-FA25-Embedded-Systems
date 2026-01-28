# bob-ross-project-FA25-Embedded-Algorithms
## `../algorithms`

### Overview
See project repo [Algorithms](https://github.com/wpi-rpc/bob-ross-project-FA25-Algorithms/) for further elucidation.

This is a temporary subdirectory of the primary repository. The following milestones have been achieved:
- `edge_path_planning.py` refactored to `edge_path_planning.cpp`+`edge_path_planning.hpp`
- `turtle_draw.py` refactored to `turtle_draw.cpp`

Assuming one has already installed and built OpenCV on their local machine (specifically the `~` directory of the user's Linux VM), they may execute the following using the C++ wrapper for GCC in the shell:

```
g++ -std=c++17 turtle_draw.cpp edge_path_planning.cpp \
    -I$HOME/opencv_build/include/opencv4 \
    -L$HOME/opencv_build/lib64 -lopencv_core -lopencv_imgproc -lopencv_imgcodecs \
    -o turtle_draw
```

### Workflows
The above shall be condensed into a CMake-compatible file in a future commit.

> [!IMPORTANT]
> Reconfigure the ESP-IDF project (`idf.py reconfigure`) AFTER cleaning and purging the build directory (`idf.py fullclean`) upon incorporation of additional components (or, in this particular instance, specifically the Espressif Systems implementation of the OpenCV component). Only afterwards should rebuilding (`idf.py build`) be considered.