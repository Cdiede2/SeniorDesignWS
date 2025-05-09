#include "camera.h"


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

    while( (left != input.cend()) && ( delims.find(*left) != std::string::npos ) ) {
        l_places += 1;
        left++;
    }

    while( (right != input.crend()) && ( delims.find(*right) != std::string::npos ) ) {
        r_places += 1;
        right++;
    }

    if( left == input.cend() ) {
        return "";
    }
    else {
        return input.substr( l_places, input.size() - l_places - r_places );
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

int countChar( const std::string& input, char delim ) {

    int sum = 0;
    for( char chr : input ) {
        sum += (chr == delim)?1:0;
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