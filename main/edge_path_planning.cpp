#include "edge_path_planning.hpp"

std::vector<std::vector<cv::Point>> interpolate(const std::vector<std::vector<cv::Point>> &path) {
    std::vector<std::vector<cv::Point>> new_path = {};
    
    for (const std::vector<cv::Point> &streak : path) {
        std::vector<cv::Point> new_streak = {};
        cv::Point previous_point = streak[0];
        new_streak.push_back(previous_point);
        for (const cv::Point &point : streak) {
            int dx=point.x-previous_point.x, dy=point.y-previous_point.y;
            if ((dx*dx+dy*dy)>10) { // Possibly unnecessary; recall the triangle inequality
                new_streak.push_back(point);
                previous_point = point;
            }
        } new_path.push_back(new_streak);
    }
    
    return new_path;
}

std::vector<std::vector<cv::Point>> edge_path_coordinates(cv::Mat &image) { // `image` is mutable
    std::vector<std::vector<cv::Point>> path = {};
    std::vector<cv::Point> streak = generate_edge_streak(image);
    // Keep going as long as drawing streak is present
    while (streak.size()) {
        // Ignore streaks that are too short
        if (streak.size()>2) {
            path.push_back(streak);
        } streak = generate_edge_streak(image);
    }

    return interpolate(path);
}

std::vector<cv::Point> generate_edge_streak(cv::Mat &image) {
    std::vector<cv::Point> streak = {};
    std::vector<cv::Point> queue = {};
    // Find first white pixel in image
    bool stop = false;
    for (int i=0; i<image.rows; i++) {
        /* cv::MatIterator_<uchar> iter = std::find(image.row(i).begin<uchar>(), image.row(i).end<uchar>(), 255); // Should ideally work but sadly painfully slow (and leading to UB)
        if (iter!=image.row(i).end<uchar>()) {
            queue.push_back(cv::Point(std::distance(image.row(i).begin<uchar>(), iter), i));
            break;
        } */
        uchar* row = image.ptr<uchar>(i); // Row pointers strangely do the job better
        for (int j=0; j<image.cols; j++) {
            if (row[j] == 255) {
                queue.push_back(cv::Point(j,i));
                stop = true;
                break;
            }
        } if (stop) break;
    }
    while (queue.size()) {
        cv::Point current = queue.front();
        queue.erase(queue.begin());
        if (image.at<uchar>(current) == 255) {
            // Mark pixel as visited by coloring it gray
            image.at<uchar>(current) = 128;
            streak.push_back(current);
            std::vector<cv::Point> neighbours = get_neighbours(current, image);
            // Sort neighbours by number of white pixels surrounding them (smaller number of white pixels means it's more likely to be an edge)
            if (neighbours.size()>1) {
                // Reverse because we want pixels with fewer white neighbours to be explored first
                std::sort(neighbours.begin(), neighbours.end(), [&](const cv::Point &p1, const cv::Point &p2){return get_neighbours(p1, image).size()<get_neighbours(p2, image).size();}); // `&image` used in place of `&` for the sake of good housekeeping
            } if (neighbours.size() && (std::find(queue.begin(), queue.end(), neighbours[0]) == queue.end())) { // Surprisingly not an out-of-range exception because of C++ "short-circuit" evaluation of logical AND
                queue.insert(queue.begin(), neighbours[0]);
            }
        }
    }
    return streak;
}

// # Returns all white pixel neighbours of a given pixel;
std::vector<cv::Point> get_neighbours(const cv::Point pixel, const cv::Mat &image) {
    std::vector<cv::Point> neighbours = {};
    int x=pixel.x, y=pixel.y;
    // # Neighbours below pixel;
    if (y) {
        if (image.at<uchar>(cv::Point(x,y-1)) == 255) {
            neighbours.push_back(cv::Point(x,y-1));
        } if (x > 0 && image.at<uchar>(cv::Point(x-1,y-1)) == 255) {
            neighbours.push_back(cv::Point(x-1,y-1));
        } if (x < image.cols-1 && image.at<uchar>(cv::Point(x+1,y-1)) == 255) {
            neighbours.push_back(cv::Point(x+1,y-1));
        }
    }
    // # neighbours above pixel;
    if (y < image.rows-1) {
        if (image.at<uchar>(cv::Point(x,y+1)) == 255) {
            neighbours.push_back(cv::Point(x,y+1));
        } if (x > 0 && image.at<uchar>(cv::Point(x-1,y+1)) == 255) {
            neighbours.push_back(cv::Point(x-1,y+1));
        } if (x < image.cols-1 && image.at<uchar>(cv::Point(x+1,y+1)) == 255) {
            neighbours.push_back(cv::Point(x+1,y+1));
        }
    }
    // # neighbours left and right of pixel;
    if (x > 0 && image.at<uchar>(cv::Point(x-1,y)) == 255) {
        neighbours.push_back(cv::Point(x-1,y));
    } if (x < image.cols-1 && image.at<uchar>(cv::Point(x+1,y)) == 255) {
        neighbours.push_back(cv::Point(x+1,y));
    }

    return neighbours;
}