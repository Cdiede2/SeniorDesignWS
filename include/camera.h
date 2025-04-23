#pragma once
#define CAMERA_H

#include <stdint.h>
#include <vector>
#include <string>

std::string strip(const std::string &input, const std::string delims)
{
    std::string::const_iterator left = input.cbegin();
    std::string::const_iterator right = input.end();
    std::string result;

    if (!input.size())
    {
        return "";
    }
    while (delims.find(*left) != std::string::npos)
    {
        left++;
    }
    while (delims.find(*(right - 1)) != std::string::npos)
    {
        right--;
    }

    while (left != right)
    {
        result.push_back(*left++);
    }
    return result;
}

std::vector<std::string> split(const std::string &input, char delim)
{
    std::string workingStr = strip(input, {delim});
    std::vector<std::string> strVec;
    if (!input.size())
        return {""};

    size_t pos = 0;
    size_t next = workingStr.find(delim, pos);
    strVec.push_back(workingStr.substr(pos, next - pos));

    pos = next;
    while (pos != std::string::npos)
    {
        next = workingStr.find(delim, pos + 1);
        strVec.push_back(workingStr.substr(pos + 1, next - pos - 1));
        pos = next;
    }
    return strVec;
}

// class Pixel
// {
//     public:
//         // Constructors
//         Pixel(){}
//         Pixel(uint8_t r, uint8_t g, uint8_t b)
//         {
//             _red = r;
//             _green = g;
//             _blue = b;
//         }

//         // Overloaded Operators
//         Pixel operator*( Pixel& right ) {
//             Pixel result;
//             result._red = _red      * right._red    / 255;
//             result._green = _green  * right._green  / 255;
//             result._blue = _blue    * right._blue   / 255;
//             return result;
//         }

//         uint16_t dot( Pixel& right ) {
//             Pixel result = *this * right;
//             return result._red + result._green + result._blue;
//         }

//         // Getters
//         uint8_t& red() {
//             return _red;
//         }
//         uint8_t& green() {
//             return _green;
//         }
//         uint8_t& blue() {
//             return _blue;
//         }

//     private:
//         uint8_t _red;
//         uint8_t _green;
//         uint8_t _blue;
// };