#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "edge_path_planning.hpp"

int main() {
    char* IMAGE = "jordan.png"; // ISO C++ apparently "forbids converting a string constant to ‘char*’," but we do not
    // 240, 180
    // IMAGE_SIZE = (1000, 1000)
    // # Process the image
    cv::Mat edges, gray_img = cv::imread(IMAGE, cv::IMREAD_GRAYSCALE); // # choose the image you want here
    // gray_img = cv.resize(gray_img, IMAGE_SIZE)
    // assert gray_img is not None, "file could not be read, check with os.path.exists()"
    cv::Canny(gray_img,edges,100,250); // # edge detection function (parameters can be adjusted)
    // color_img = cv.imread(IMAGE)
    // color_img = cv.resize(color_img, IMAGE_SIZE)

    std::vector<std::vector<cv::Point>> edge_path = edge_path_coordinates(edges);
    cv::String coord_buffer = "[\n";
    for (int i=0; i<edge_path.size(); i++) {
        coord_buffer+="\t[";
        for (int j=0; j<edge_path[i].size(); j++) {
            coord_buffer+="("+std::to_string(edge_path[i][j].x)+","+std::to_string(edge_path[i][j].y)+"), ";
        } coord_buffer+="],\n";
    } coord_buffer+="]\n";
    // std::cout << coord_buffer;
    return 0;
}