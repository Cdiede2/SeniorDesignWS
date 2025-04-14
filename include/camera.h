#pragma once
#define CAMERA_H

#include <stdint.h>


class Pixel
{
    public:
        // Constructors
        Pixel(){}
        Pixel(uint8_t r, uint8_t g, uint8_t b)
        {
            _red = r;
            _green = g;
            _blue = b;
        }

        // Overloaded Operators
        Pixel operator*( Pixel& right ) {
            Pixel result;
            result._red = _red      * right._red    / 255;
            result._green = _green  * right._green  / 255;
            result._blue = _blue    * right._blue   / 255;
            return result;
        }

        uint16_t dot( Pixel& right ) {
            Pixel result = *this * right;
            return result._red + result._green + result._blue;
        }

        // Getters
        uint8_t& red() {
            return _red;
        }
        uint8_t& green() {
            return _green;
        }
        uint8_t& blue() {
            return _blue;
        }

    private:
        uint8_t _red;
        uint8_t _green;
        uint8_t _blue;
};