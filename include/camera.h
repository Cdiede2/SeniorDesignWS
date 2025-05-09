#pragma once
#define CAMERA_H

#include <stdint.h>
#include <vector>
#include <string>
#include <iomanip>
#include <openssl/evp.h>



struct ClientException
{
    std::string what;
    uint8_t val;
};

struct ServerException
{
    std::string what;
    uint8_t val;
};

struct Filter
{
    uint8_t _red;
    uint8_t _grn;
    uint8_t _blu;
};

enum numbers
{
    RETURN_OK,
    RETURN_NETWORK_ERR,
    RETURN_USR_ERR
};

enum SatColor
{
    RED,
    GREEN,
    BLUE
};

/**
 * @brief Compares two Filter objects for equality.
 * 
 * @param left The first Filter object to compare.
 * @param right The second Filter object to compare.
 * @return true if the red, green, and blue components of both Filter objects are equal; otherwise, false.
 * @details This operator checks if the _red, _grn, and _blu fields of the two Filter objects are identical.
 * It returns true if all three components match, otherwise it returns false.
 */
bool operator==(const Filter &left, const Filter &right);


std::ostream &operator<<(std::ostream &ostr, const Filter &filter);

std::ostream &operator<<(std::ostream &ostr, const std::vector<Filter> &filters);

/**
 * @brief Removes leading and trailing characters from a string based on a set of delimiters.
 * @param input The input string to be stripped.
 * @param delims A string containing the characters to be removed from the beginning and end of the input string.
 * @return A new string with the specified characters removed from both ends.
 * @details This function iterates over the input string to identify and remove any characters
 * specified in the `delims` parameter from the start and end of the string. If the input string
 * is empty, it returns an empty string. The function preserves the order of the remaining characters.
 */
std::string strip(const std::string &input, const std::string delims);

/**
 * @brief Splits a string into a vector of substrings based on a delimiter character.
 * @param input The input string to be split.
 * @param delim The delimiter character used to split the string.
 * @return A vector of substrings obtained by splitting the input string.
 * @details This function first removes leading and trailing occurrences of the delimiter
 * from the input string using the `strip` function. It then iteratively finds the delimiter
 * within the string and extracts substrings between occurrences of the delimiter.
 * The resulting substrings are stored in a vector, which is returned as the output.
 */
std::vector<std::string> split(const std::string &input, char delim);

/**
 * @brief Counts the occurrences of a specific character in a string.
 * 
 * @param input The input string in which to count occurrences of the character.
 * @param delim The character to count within the input string.
 * @return The number of times the specified character appears in the input string.
 * 
 * @details This function iterates through each character in the input string and increments
 * a counter whenever the specified character (`delim`) is encountered. The final count is
 * returned as an integer. If the input string is empty, the function returns 0.
 */
int countChar( const std::string& input, char delim );

/**
 * @brief Parses a header string into a CamHeader structure.
 * @param header The header string to be parsed.
 * @return A CamHeader structure containing the parsed values.
 * @details This function splits the input header string using the '/' delimiter and
 * assigns the parsed values to the corresponding fields in the CamHeader structure.
 * It throws an exception if the header format is invalid or if the number of fields is incorrect.
 */
// CamHeader parseHeader(const std::string &header)
// {
//     CamHeader camHeader;
//     std::vector<std::string> headerVector = split(header, '/');

//     if (headerVector.size() != 3)
//     {
//         throw std::invalid_argument("Invalid header format");
//     }

//     camHeader.protocol = std::stoi(headerVector[0]);
//     camHeader.flags = std::stoi(headerVector[1], nullptr, 16);
//     camHeader.seq_num = std::stoi(headerVector[2]);

//     if (camHeader.flags > 0xFF)
//     {
//         throw std::invalid_argument("Invalid flags value");
//     }
//     return camHeader;
// }

/**
 * @brief Generates and returns the MD5 Checksum
 * @param string: The string to return the hash of
 * @return The MD5 checksum of the parameter string.
 * @ref Thank you Michael!
 * https://stackoverflow.com/questions/7860362/how-can-i-use-openssl-md5-in-c-to-hash-a-string
 */
std::string md5(const std::string &content);

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