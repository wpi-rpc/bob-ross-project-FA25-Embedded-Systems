#ifndef EDGE_PATH_PLANNING_HPP
#define EDGE_PATH_PLANNING_HPP

#include <vector>
#include <opencv2/core.hpp>

typedef std::vector<cv::Point> Streak;
typedef std::vector<Streak> Path;

Path edge_path_coordinates(cv::Mat &image, const cv::Point offset=cv::Point(0,0)); // `image` is mutable; `offset` is added to account for cropping, if any at all
Streak generate_edge_streak(cv::Mat &image, const cv::Point offset=cv::Point(0,0));
Path interpolate(const Path &path);
Streak get_neighbours(const cv::Point pixel, const cv::Mat &image);

#endif