

#pragma once
#define CAMERA_H

#include <stdint.h>
#include <vector>
#include <string>
#include <iomanip>
#include <openssl/evp.h>

/**
 * @brief Struct containing elements of a header
 * @param protocol Unused for now, default set to 0
 * @param flags Stores flags as indicators between server and client
 * @param seq_num Sequence number of header
 * @param size Size of following payload, used to allocate buffer size prior to transmission
 */
// struct CamHeader
// {
//     uint8_t protocol;
//     uint8_t flags; // Start || Listen  ||  Set Encode || Response || End || Unused...
//     uint16_t seq_num;
//     uint32_t size;
// };

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

bool operator==(const Filter &left, const Filter &right)
{
    if (left._red == right._red && left._grn == right._grn && left._blu == right._blu)
    {
        return true;
    }
    return false;
}

std::ostream &operator<<(std::ostream &ostr, const Filter &filter)
{

    ostr << std::hex << std::setw(3) << (int)filter._red << " ";
    ostr << std::hex << std::setw(3) << (int)filter._grn << " ";
    ostr << std::hex << std::setw(3) << (int)filter._blu << " ";
    ostr << std::dec;
    return ostr;
}

std::ostream &operator<<(std::ostream &ostr, const std::vector<Filter> &filters)
{
    ostr << " +--------------+" << std::endl;
    for (auto filter : filters)
    {
        ostr << " | " << filter << " |" << std::endl;
    }
    ostr << " +--------------+" << std::endl;
    return ostr;
}

enum SatColor
{
    RED,
    GREEN,
    BLUE
};

/**
 * @brief Removes leading and trailing characters from a string based on a set of delimiters.
 * @param input The input string to be stripped.
 * @param delims A string containing the characters to be removed from the beginning and end of the input string.
 * @return A new string with the specified characters removed from both ends.
 * @details This function iterates over the input string to identify and remove any characters
 * specified in the `delims` parameter from the start and end of the string. If the input string
 * is empty, it returns an empty string. The function preserves the order of the remaining characters.
 */
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
std::string md5(const std::string &content)
{
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_md5();
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;
    std::string output;

    EVP_DigestInit_ex2(context, md, NULL);
    EVP_DigestUpdate(context, content.c_str(), content.length());
    EVP_DigestFinal_ex(context, md_value, &md_len);
    EVP_MD_CTX_free(context);

    output.resize(md_len * 2);
    for (unsigned int i = 0; i < md_len; ++i)
        std::sprintf(&output[i * 2], "%02x", md_value[i]);
    return output;
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