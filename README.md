# bob-ross-project-FA25-Embedded-Algorithms
## File Structure
```
.
├── .devcontainer/
│   ├── devcontainer.json
│   └── Dockerfile
├── main/
│   ├── CMakeLists.txt
│   ├── Kconfig.projbuild
│   ├── edge_path_planning.cpp
│   ├── edge_path_planning.hpp
│   ├── favicon.ico
│   ├── idf_component.yml
│   ├── index.html
│   └── main.cpp
├── .clangd
├── .gitignore
├── CMakeLists.txt
├── dependencies.lock
├── LICENSE
├── partitions.csv
├── README.md
└── sdkconfig.ci
```
[![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v5.5.2-E7352C.svg?logo=espressif)](https://github.com/espressif/esp-idf/releases/tag/v5.5.2)
[![CMake](https://img.shields.io/badge/CMake-v3.30.2-darkmagenta.svg?logo=cmake)](https://github.com/Kitware/CMake/releases/tag/v3.30.2)
[![OpenCV](https://img.shields.io/badge/OpenCV-4.10.0~3-indigo.svg?logo=opencv)](https://components.espressif.com/components/espressif/opencv/versions/4.10.0~3)
[![License](https://img.shields.io/github/license/wpi-rpc/bob-ross-project-FA25-Embedded-Systems?label=License&color=crimson&logo=opensourceinitiative&logoColor=white)](LICENSE)

## References
* Espressif Systems Resources
    * [`zlib` component](https://components.espressif.com/components/espressif/zlib/versions/1.3.1/readme)
    * [`mdns` component](https://components.espressif.com/components/espressif/mdns/versions/1.9.1/readme)
    * [Configuration file](https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/configuration-file.html)
    * [Panic handler](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/fatal-errors.html#panic-handler)
    * [Web server AT examples](https://docs.espressif.com/projects/esp-at/en/latest/esp32/AT_Command_Examples/Web_server_AT_Examples.html)
    * [TCP-IP AT examples](https://docs.espressif.com/projects/esp-at/en/latest/esp32/AT_Command_Examples/TCP-IP_AT_Examples.html)
    * [Logging library](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/log.html)
* OpenCV Resources
    * [`cv::Mat` class reference](https://docs.opencv.org/4.x/d3/d63/classcv_1_1Mat.html)
    * [`cv::String` class reference](https://docs.opencv.org/3.4/d1/d8f/classcv_1_1String.html)
    * [Canny edge detector](https://docs.opencv.org/3.4/da/d5c/tutorial_canny_detector.html)
    * [Feature detection](https://docs.opencv.org/4.x/dd/d1a/group__imgproc__feature.html)
    * [Image file reading and writing](https://docs.opencv.org/3.4/d4/da8/group__imgcodecs.html)
    * [Operations with images](https://docs.opencv.org/3.4/d5/d98/tutorial_mat_operations.html)
    * [`cv::Point_<_Tp>` class template reference](https://docs.opencv.org/3.4/db/d4e/classcv_1_1Point__.html)
* Microsoft C++ Resources
    * [Raw pointers](https://learn.microsoft.com/en-us/cpp/cpp/raw-pointers)
* Mozilla Developer Notes
    * [`HTMLCanvasElement`: `toDataURL()` method](https://developer.mozilla.org/en-US/docs/Web/API/HTMLCanvasElement/toDataURL)
    * [`CanvasRenderingContext2D`: `getImageData()` method](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/getImageData)