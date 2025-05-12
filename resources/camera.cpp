#include "camera.h"

bool checkHashJSON( nlohmann::json input, std::string hash ) {
    std::string resultMD5;
    std::string expected;

    // If 'hash' field inside input, remove
    if( input.contains("hash") ) {
        expected = input.at("hash"); 
        input.erase("hash");

        // Compute MD5 Hash of input
        resultMD5 = md5( input.dump() );
        return (resultMD5 == expected);

    // If 'hash' field is missing, use hash provided in parameter list
    } else {
        resultMD5 = md5( input.dump() );
        return (resultMD5 == hash);
    }
    return false;
}

std::string getImageHash( cv::Mat& img ) {
    std::vector<uchar> buffer;
    cv::imencode(".png", img, buffer);
    return md5({reinterpret_cast<const char*>(buffer.data())});
}

uint32_t validIPv4(const std::string &ipAddress)
{
    std::vector<std::string> octets;

    // Try-Catch to handle C++ exception from std::stoi
    try
    {
        // Assuming the decimal representation of each octet is 3 characters
        // plus the addition of three '.'
        // i.e.) (3*4) + 3 = 15 characters
        if (countChar(ipAddress, '.') > LONGEST_POSSIBLE_IPV4)
        {
            return false;
        }

        // Check that each character in string is either numeric
        // or the delimiting character ('.')
        for (char chr : ipAddress)
        {
            if (!isdigit(chr) && (chr != '.'))
            {
                false;
            }
        }

        // Check if First or Last Character is a '.'
        if (ipAddress.at(0) == '.' || ipAddress.at(ipAddress.size() - 1) == '.')
        {
            return false;
        }

        // Split IP Address into octets
        // If more than 4, return false
        octets = split(ipAddress, '.');
        if (octets.size() != 4)
        {
            return false;
        }

        // Check that each 'octet' is actually an octet
        //       0 <= octet < 256
        for (std::string octet : octets)
        {
            if (std::stoi(octet) > 255)
            {
                return false;
            }
        }
    }
    catch (std::exception &exc)
    {
        return false;
    }

    uint32_t ipv4 = 0;
    ipv4 |= (uint8_t)std::stoi(octets.at(3));
    ipv4 |= (uint8_t)std::stoi(octets.at(2)) << 8;
    ipv4 |= (uint8_t)std::stoi(octets.at(1)) << 16;
    ipv4 |= (uint8_t)std::stoi(octets.at(0)) << 24;
    return ipv4;
}

bool validIPv4Listening(const std::string &ipAddress)
{
    uint32_t decimalIP = validIPv4(ipAddress);

    // Cannot be valid if IP Address is not first a correctly formatted IPv4
    if ( decimalIP = 0x00000000 )
    {
        // Listening Address 0.0.0.0 is valid, but otherwise incompatible by validIPv4() due to how value is returned
        if(ipAddress == "0.0.0.0") {
            return true;
        } else {
            return false;
        }
    } 

    bool allZero = true;
    for (int i = 0; i < 32; i++)
    {
        int bit = (0b1 << i);

        // If all bits have been 0 so far and current is also 0
        if ( !( decimalIP & bit) )
        {
            // Encountered a Zero after the first One, not a listening address
            if( ! allZero ) {
                return false;
            }
            continue;
        }

        // If all bits have been 0 so far and current is 1
        else
        {
            allZero = false;
        } 
    }
    return true;
}

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

std::string strip(const std::string &input, const std::string delims)
{
    std::string::const_iterator left = input.cbegin();
    size_t l_places = 0;

    std::string::const_reverse_iterator right = input.crbegin();
    size_t r_places = 0;

    while ((left != input.cend()) && (delims.find(*left) != std::string::npos))
    {
        l_places += 1;
        left++;
    }

    while ((right != input.crend()) && (delims.find(*right) != std::string::npos))
    {
        r_places += 1;
        right++;
    }

    if (left == input.cend())
    {
        return "";
    }
    else
    {
        return input.substr(l_places, input.size() - l_places - r_places);
    }

    /*
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
    */
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

int countChar(const std::string &input, char delim)
{

    int sum = 0;
    for (char chr : input)
    {
        sum += (chr == delim) ? 1 : 0;
    }
    return sum;
}

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