#ifndef EDGE_PATH_PLANNING_HPP
#define EDGE_PATH_PLANNING_HPP

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

std::vector<std::vector<cv::Point>> interpolate(const std::vector<std::vector<cv::Point>> &path);
std::vector<std::vector<cv::Point>> edge_path_coordinates(cv::Mat &image);
std::vector<cv::Point> generate_edge_streak(cv::Mat &image);
std::vector<cv::Point> get_neighbours(const cv::Point pixel, const cv::Mat &image);

#endif