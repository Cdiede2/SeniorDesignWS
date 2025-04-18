#include <iostream>
#include <cstring>
#include <openssl/md5.h>
#include <format>

#include <iomanip>
#include <vector>
#include <map>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>

// #include "camera.h"
#include "shannon-fano.h"


// reinterpret_cast<int*>(0x00000000);

int main(int argc, char** argv) {
    ShannonFano sf;
    std::map<char, double> frequencies;

    if( argc < 2 ) {
        std::cerr << "Usage: " << argv[0] << " <input string>" << std::endl;
        return 1;
    }
    std::string input = argv[1];

    // sf.buildCodes(frequencies, "the quick brown fox jumps over the lazy dog");
    sf.buildCodes(frequencies, input );
    
    // for( auto &pair : frequencies ) {
    //     std::cout << pair.first << ": " << pair.second << std::endl;
    // }
    
    return 0;
}