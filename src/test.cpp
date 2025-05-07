#include <gtest/gtest.h>
#include <iostream>
#include <format>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>

#include <openssl/md5.h>
#include <fstream>
#include <nlohmann/json.hpp>


#include "shannon-fano.h"

#include "camera.h"
// #include "md5.h"

std::string convertHashToString(const uint8_t *digest);

// Test Gtest framework is correctly linked
// and working
TEST(BASIC, BASIC)
{
    EXPECT_TRUE(true);
}

/* String Manipulation Functions: Strip()*/
TEST(STRING_MANIP_STRIP, Empty_String)
{
    std::string input = "";
    std::string output;

    EXPECT_NO_THROW(output = strip(input, ""));
    EXPECT_NO_THROW(output = strip(input, " "));
    EXPECT_EQ("", output);
}

TEST(STRING_MANIP_STRIP, Empty_Delims)
{
    std::string input = "hello";
    std::string output;

    EXPECT_NO_THROW(output = strip(input, ""));
    EXPECT_EQ("hello", output);

    input = "__hello__";
    EXPECT_NO_THROW(output = strip(input, ""));
    EXPECT_EQ("__hello__", output);
}

TEST(STRING_MANIP_STRIP, Left_Delims)
{
    std::string input = "__hello";
    std::string output;

    EXPECT_NO_THROW(output = strip(input, "_"));
    EXPECT_EQ("hello", output);

    input = "_hello";
    EXPECT_NO_THROW(output = strip(input, "_"));
    EXPECT_EQ("hello", output);

    input = "hello";
    EXPECT_NO_THROW(output = strip(input, "_"));
    EXPECT_EQ("hello", output);

    input = "__hello__world";
    EXPECT_NO_THROW(output = strip(input, "_"));
    EXPECT_EQ("hello__world", output);
}

TEST(STRING_MANIP_STRIP, Right_Delims)
{
    std::string input = "hello__";
    std::string output;

    EXPECT_NO_THROW(output = strip(input, "_"));
    EXPECT_EQ("hello", output);

    input = "hello_";
    EXPECT_NO_THROW(output = strip(input, "_"));
    EXPECT_EQ("hello", output);

    input = "hello";
    EXPECT_NO_THROW(output = strip(input, "_"));
    EXPECT_EQ("hello", output);

    input = "hello__world__";
    EXPECT_NO_THROW(output = strip(input, "_"));
    EXPECT_EQ("hello__world", output);
}

TEST(STRING_MANIP_STRIP, Left_Right_Delims)
{
    std::string input = "__hello__";
    std::string output;

    EXPECT_NO_THROW(output = strip(input, "_"));
    EXPECT_EQ("hello", output);

    input = "_hello_";
    EXPECT_NO_THROW(output = strip(input, "_"));
    EXPECT_EQ("hello", output);

    input = "hello";
    EXPECT_NO_THROW(output = strip(input, "_"));
    EXPECT_EQ("hello", output);

    input = "__hello__world__";
    EXPECT_NO_THROW(output = strip(input, "_"));
    EXPECT_EQ("hello__world", output);
}

TEST(STRING_MANIP_STRIP, Multiple_Delims)
{
    std::string input = "_/hello/_";
    std::string output;

    EXPECT_NO_THROW(output = strip(input, "_/"));
    EXPECT_EQ("hello", output);

    input = "__hello_world!__";
    EXPECT_NO_THROW(output = strip(input, "_/!"));
    EXPECT_EQ("hello_world", output);
}

/* String Manipulation Functions: Split() */
TEST(STRING_MANIP_SPLIT, Empty_String)
{
    std::string input = "";
    std::vector<std::string> result;
    EXPECT_NO_THROW(result = split(input, '/'));
    EXPECT_EQ("", result.at(0));
}

TEST(STRING_MANIP_SPLIT, Non_Empty_String_No_Split)
{
    std::string input = "Hello World!";
    std::vector<std::string> result;
    EXPECT_NO_THROW(result = split(input, '/'));
    EXPECT_EQ(1, result.size());
    EXPECT_EQ("Hello World!", result.at(0));
}

TEST(STRING_MANIP_SPLIT, Non_Empty_String_Multiple_Splits)
{
    std::string input = "The quick brown fox jumped over the lazy dog";
    std::vector<std::string> result;
    EXPECT_NO_THROW(result = split(input, ' '));

    EXPECT_EQ(9, result.size());

    EXPECT_EQ("The", result.at(0));
    EXPECT_EQ("quick", result.at(1));
    EXPECT_EQ("brown", result.at(2));
    EXPECT_EQ("fox", result.at(3));
    EXPECT_EQ("jumped", result.at(4));
    EXPECT_EQ("over", result.at(5));
    EXPECT_EQ("the", result.at(6));
    EXPECT_EQ("lazy", result.at(7));
    EXPECT_EQ("dog", result.at(8));
}

TEST(STRING_MANIP_SPLIT, Non_Empty_String_Forward_Backward_Delim)
{
    std::string input = "   The quick brown fox jumped over the lazy dog  ";
    std::vector<std::string> result;
    EXPECT_NO_THROW(result = split(input, ' '));

    EXPECT_EQ(9, result.size());

    EXPECT_EQ("The", result.at(0));
    EXPECT_EQ("quick", result.at(1));
    EXPECT_EQ("brown", result.at(2));
    EXPECT_EQ("fox", result.at(3));
    EXPECT_EQ("jumped", result.at(4));
    EXPECT_EQ("over", result.at(5));
    EXPECT_EQ("the", result.at(6));
    EXPECT_EQ("lazy", result.at(7));
    EXPECT_EQ("dog", result.at(8));
}

/* Camera Tests */
TEST(CAMERA_TEST, Camera_Init) {
    EXPECT_TRUE(true);
}


/* Tests of MD5 Hashing Algorithm */
TEST(MD5, MD5_String)
{
    MD5_CTX ctx;
    uint8_t digest[16];

    std::vector<std::string> inputs = {
        "",
        "a",
        "abc",
        "message digest",
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
        "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
    };

    std::vector<std::string> expected_hashes = {
        "d41d8cd98f00b204e9800998ecf8427e",
        "0cc175b9c0f1b6a831c399e269772661",
        "900150983cd24fb0d6963f7d28e17f72",
        "f96b697d7cb7938d525a2f31aaf161d0",
        "c3fcd3d76192e4007dfb496cca67e13b",
        "d174ab98d277d9f5a5611c2c9f419d9f",
        "57edf4a22be3c955ac49da2e2107b67a",
    };

    size_t i = 0;
    for (std::string &input : inputs)
    {
        std::string result;
        MD5(reinterpret_cast<const unsigned char *>(input.c_str()), input.size(), digest);

        result = convertHashToString(digest);
        EXPECT_EQ(result, expected_hashes[i]);
        i++;
    }
}

TEST(MD5, MD5_File)
{
    MD5_CTX ctx;
    uint8_t digest[16];
    std::ifstream file("/home/jay/Desktop/SeniorDesign_WS/Code/cursed-2.jpg", std::ios::binary);
    if (!file)
    {
        std::cerr << "Could not open file for reading." << std::endl;
        return;
    }

    MD5_Init(&ctx);
    char buffer[1024];
    while (file.read(buffer, sizeof(buffer)))
    {
        // std::cout << "Reading " << file.gcount() << " bytes from file." << std::endl;
        MD5_Update(&ctx, buffer, file.gcount());
    }
    file.close();

    MD5_Final(digest, &ctx);

    std::string result = convertHashToString(digest);
    // std::cout << "MD5 hash of file: " << result << std::endl;
    // EXPECT_EQ(result, "57edf4a22be3c955ac49da2e2107b67a");
    EXPECT_NE(result, "d41d8cd98f00b204e9800998ecf8427e");
}

/* Tests of Shannon-Fano Compression */
TEST(SFComp, SFComp_INIT)
{
    ShannonFano sf;
    std::map<char, double> frequencies;
}

TEST(SFComp, SFComp_Empty)
{
    ShannonFano sf;
    std::map<char, double> frequencies;
    EXPECT_ANY_THROW(sf.buildCodes(frequencies, ""));
    // sf.buildCodes(frequencies, "");
    EXPECT_TRUE(frequencies.empty());
}

TEST(SFComp, SFComp_SingleChar)
{
    ShannonFano sf;
    std::map<char, double> frequencies;
    sf.buildCodes(frequencies, "a");
    EXPECT_EQ(frequencies.size(), 1);
    EXPECT_EQ(frequencies['a'], 1.0);
}

/* Test Parse Header */
// TEST(PARSE_HEADER, Normal_Header) {
//     CamHeader header = parseHeader("0/80/64");
//     EXPECT_EQ(header.protocol, 0);
//     EXPECT_EQ(header.flags, 0x80);
//     EXPECT_EQ(header.seq_num, 64);
// }

// TEST(PARSE_HEADER, Invalid_Header) {
//     EXPECT_ANY_THROW(parseHeader("0/80/64/extra"));
//     EXPECT_ANY_THROW(parseHeader("0/80"));
//     EXPECT_ANY_THROW(parseHeader("0"));
//     EXPECT_ANY_THROW(parseHeader("0:80:64:extra"));
// }

/* Test Image Open */
TEST(IMAGE, IMAGE_Open)
{
    cv::Mat image = cv::imread("/home/jay/Desktop/SeniorDesign_WS/Code/test_img.png");
    EXPECT_FALSE(image.empty());
    EXPECT_EQ(image.type(), CV_8UC3);
    // EXPECT_EQ(image.cols, 640);
    // EXPECT_EQ(image.rows, 480);
}

/* Test JSON Serialization */
TEST(JSON, Generic_JSON_Serialization)
{
    nlohmann::json j;
    j["name"] = "John Doe";
    j["age"] = 30;
    j["is_student"] = false;

    std::string jsonString = j.dump();
    std::cout << "Serialized JSON: " << jsonString << std::endl;

    nlohmann::json parsedJson = nlohmann::json::parse(jsonString);
    EXPECT_EQ(parsedJson["name"], "John Doe");
    EXPECT_EQ(parsedJson["age"], 30);
    EXPECT_EQ(parsedJson["is_student"], false);
}

TEST(JSON, SF_JSON_Serialization) {
    ShannonFano sf;
    std::map<char, double> frequencies;
    std::string input = "hello world";
 
    EXPECT_NO_THROW( sf.buildCodes(frequencies, input) );
 
    std::map<char, std::string> codes = sf.getCodes();
    
    nlohmann::json j = codes;

    std::string jsonString = nlohmann::json(codes).dump();
    std::cout << "Serialized JSON: " << j.dump(2) << std::endl;
}



/* Utility Functions */
std::string convertHashToString(const uint8_t *digest)
{
    std::string result;
    for (int i = 0; i < 16; ++i)
    {
        result += std::format("{:02x}", digest[i]);
    }
    return result;
}