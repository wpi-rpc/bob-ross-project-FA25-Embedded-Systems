#include "color_path_planning.hpp"
#include <cmath>
#include <map>
#include <algorithm>

int get_average_color(const cv::Mat &bgr_image, const cv::Point start, const cv::Point end, bool horizontal, int step) {
    int count = 0;
    double total_blue = 0, total_green = 0, total_red = 0;
    
    // Tally up the colors from start to end
    if (horizontal) {
        // Iterate along x-axis (horizontal)
        int start_x = std::min(start.x, end.x);
        int end_x = std::max(start.x, end.x);
        
        for (int i = start_x; i <= end_x; i += step) {
            cv::Vec3b pixel = bgr_image.at<cv::Vec3b>(start.y, i);
            total_blue += pixel[0];
            total_green += pixel[1];
            total_red += pixel[2];
            count++;
        }
    } else {
        // Iterate along y-axis (vertical)
        int start_y = std::min(start.y, end.y);
        int end_y = std::max(start.y, end.y);
        
        for (int i = start_y; i <= end_y; i += step) {
            cv::Vec3b pixel = bgr_image.at<cv::Vec3b>(i, start.x);
            total_blue += pixel[0];
            total_green += pixel[1];
            total_red += pixel[2];
            count++;
        }
    }
    
    // Average out the colors
    if (count == 0) return black.id;
    
    total_blue /= count;
    total_green /= count;
    total_red /= count;
    
    // Find closest color to the one we have
    std::string closest_color = 0;
    double smallest_diff = 1024;
    
    for (const auto &color : colors) {
        // Get total difference between colors
        double total_diff = std::abs(color.blue - total_blue) + 
                           std::abs(color.green - total_green) + 
                           std::abs(color.red - total_red);
        
        if (total_diff <= smallest_diff) {
            smallest_diff = total_diff;
            closest_color = color.id
        }
    }
    
    return closest_color;
}

ColorPath color_path_coordinates(const cv::Mat &gray_image, const cv::Mat &bgr_image) {
    ColorPath path = {};
    bool reverse = false;
    
    // Horizontal pass
    for (int y = 0; y < gray_image.rows; y++) {
        cv::Point prev_edge = cv::Point(-1, -1);
        std::vector<int> value_range;
        
        // If going reverse, iterate backwards
        if (!reverse) {
            for (int x = 0; x < gray_image.cols; x++) {
                value_range.push_back(x);
            }
        } else {
            for (int x = gray_image.cols - 1; x >= 0; x--) {
                value_range.push_back(x);
            }
        }
        
        for (int x : value_range) {
            uchar current_pixel = gray_image.at<uchar>(y, x);
            
            if (current_pixel == 128) {
                if (prev_edge.x != -1) {
                    // Calculate color between previous edge and current edge
                    int color;
                    if (!reverse) {
                        color = get_average_color(bgr_image, prev_edge, cv::Point(x, y), true);
                    } else {
                        color = get_average_color(bgr_image, prev_edge, cv::Point(x, y), true, -1);
                    }
                    
                    path.push_back({prev_edge, cv::Point(x, y), color});
                }
                prev_edge = cv::Point(x, y);
            }
        }
        
        reverse = !reverse;
    }
    
    // Vertical pass
    reverse = false;
    for (int x = 0; x < gray_image.cols; x++) {
        cv::Point prev_edge = cv::Point(-1, -1);
        std::vector<int> value_range;
        
        if (!reverse) {
            for (int y = 0; y < gray_image.rows; y++) {
                value_range.push_back(y);
            }
        } else {
            for (int y = gray_image.rows - 1; y >= 0; y--) {
                value_range.push_back(y);
            }
        }
        
        for (int y : value_range) {
            uchar current_pixel = gray_image.at<uchar>(y, x);
            
            if (current_pixel == 128) {
                if (prev_edge.x != -1) {
                    // Calculate color between previous edge and current edge
                    int color;
                    if (!reverse) {
                        color = get_average_color(bgr_image, prev_edge, cv::Point(x, y), false);
                    } else {
                        color = get_average_color(bgr_image, prev_edge, cv::Point(x, y), false, -1);
                    }
                    
                    path.push_back({prev_edge, cv::Point(x, y), color});
                }
                prev_edge = cv::Point(x, y);
            }
        }
        
        reverse = !reverse;
    }
    
    return merge_consecutive_colors(path);
}

ColorPath merge_consecutive_colors(const ColorPath &path) {
    ColorPath new_path = {};
    
    for (size_t i = 1; i < path.size(); i++) {
        // Check if previous segment's end matches current segment's start and colors are the same
        if (path[i-1].end == path[i].start && path[i-1].color == path[i].color) {
            // Merge by extending the previous segment's end to current segment's end
            ColorPathSegment merged = {path[i-1].start, path[i].end, path[i].color};
            if (!new_path.empty() && new_path.back().end == merged.start && new_path.back().color == merged.color) {
                new_path.back().end = merged.end;
            } else {
                new_path.push_back(merged);
            }
        } else {
            new_path.push_back(path[i]);
        }
    }
    
    // Add first element if path is not empty
    if (!path.empty() && new_path.empty()) {
        new_path.push_back(path[0]);
    } else if (!path.empty() && new_path.empty()) {
        new_path.push_back(path[0]);
    }
    
    return new_path;
}


