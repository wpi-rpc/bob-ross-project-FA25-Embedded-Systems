#ifndef COLOR_PATH_PLANNING_HPP
#define COLOR_PATH_PLANNING_HPP

#include <vector>
#include <string>
#include <map>
#include <opencv2/core.hpp>

typedef std::vector<cv::Point> Streak;
typedef std::vector<Streak> Path;

struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t id;
};

Color red;
red.red = 255;
red.green = 0;
red.blue = 0;
red.id = 7;

Color blue;
blue.red = 0;
blue.green = 0;
blue.blue = 255;
blue.id = 6;

Color green;
green.red = 0;
green.green = 255;
green.green = 0;
green.id = 5;

Color cyan;
cyan.red = 0;
cyan.green = 255;
cyan.blue = 255;
cyan.id = 4;

Color magenta;
magenta.red = 255;
magenta.green = 0;
magenta.blue = 255;
magenta.id = 3;

Color yellow;
yellow.red = 255;
yellow.green = 255;
yellow.blue = 0;
yellow.id = 2;

Color white;
white.red = 255;
white.green = 255;
white.blue = 255;
white.id = 1;

Color black;
black.red = 0;
black.green = 0;
black.blue = 0;
black.id = 0;

Color colors[8] = {red, blue, green, cyan, magenta, yellow, white, black};

struct ColorPathSegment {
    cv::Point start;
    cv::Point end;
    int color;
};

typedef std::vector<ColorPathSegment> ColorPath;

ColorPath color_path_coordinates(const cv::Mat &gray_image, const cv::Mat &bgr_image);
std::string get_average_color(const cv::Mat &bgr_image, const cv::Point start, const cv::Point end, bool horizontal, int step = 1);
ColorPath merge_consecutive_colors(const ColorPath &path);

#endif