#include <iostream>
#include <cstring>
#include <openssl/md5.h>
#include <format>

#include <iomanip>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>

// #include "camera.h"

// reinterpret_cast<int*>(0x00000000);

int main(int argc, char** argv) {
    cv::Mat img;
    cv::Mat output_img;
    
    // Validate User Input
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
        return -1;
    }
    
    // Attempt to Load the image
    // Apply Bilateral Filter
    try {
        img = cv::imread(argv[1], cv::IMREAD_COLOR);

        cv::bilateralFilter(img, output_img, std::stoi(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]));

    } catch (const std::exception& e) {
        std::cerr << "Error loading image: " << e.what() << std::endl;
        return -1;
    }
    


    // Draw a circle in the center of the image
    cv::imshow("OpenCV Example", output_img);
    // fcv::imwrite("cursed-2.jpg", output_img);
    
    // Wait for a key press indefinitely
    cv::waitKey(0);

    return 0;
}