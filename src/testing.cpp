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
#include "client.h"
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

TEST(STRING_MANIP_STRIP, Only_Delims)
{
    std::string test_one{"_"};
    std::string test_two{"__"};

    std::string output;
    EXPECT_NO_THROW(output = strip(test_one, "_"));
    EXPECT_EQ(output, "");

    EXPECT_NO_THROW(output = strip(test_two, "_"));
    EXPECT_EQ(output, "");
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

TEST(STRING_MANIP_SPLIT, Non_Empty_String_All_Delim)
{
    std::string str{"..."};
    std::vector<std::string> output;
    output = split(str, '.');

    EXPECT_EQ(output.size(), 1);
    EXPECT_EQ(output.at(0), "");
}

TEST(STRING_MANIP_SPLIT, Numeric) {
    std::string str{".192.168.137.110."};
    std::vector<std::string> output;
    output = split(str, '.');

    EXPECT_EQ( output.size(), 4 );

    // EXPECT_EQ(output.at(0),"");
    EXPECT_EQ(output.at(0),"192");
    EXPECT_EQ(output.at(1),"168");
    EXPECT_EQ(output.at(2),"137");
    EXPECT_EQ(output.at(3),"110");
    // EXPECT_EQ(output.at(5),"");
}

/* String Manipulation Functions: countChar() */
TEST(CountChar, CountChar_Empty)
{
    std::string empty_string;
    EXPECT_EQ(countChar(empty_string, '.'), 0);
}

TEST(CountChar, CountChar_Non_Empty_No_Delim)
{
    std::string helloWorld{"Hello World!"};
    std::string randomChar{"/1/234/5652/1was/vvv"};

    EXPECT_EQ(countChar(helloWorld, '.'), 0);
    EXPECT_EQ(countChar(randomChar, ' '), 0);
}

TEST(CountChar, CountChar_Non_Empty_Delim)
{
    std::string str{"the quick fox jumped over the lazy brown dog."};

    EXPECT_EQ(countChar(str, 't'), 2);
    EXPECT_EQ(countChar(str, 'h'), 2);
    EXPECT_EQ(countChar(str, 'e'), 4);
    EXPECT_EQ(countChar(str, ' '), 8);
    EXPECT_EQ(countChar(str, 'q'), 1);
    EXPECT_EQ(countChar(str, 'u'), 2);
    EXPECT_EQ(countChar(str, 'g'), 1);
    EXPECT_EQ(countChar(str, '.'), 1);

    std::string ip_addr{"127.0.0.1."};
    EXPECT_EQ(countChar(ip_addr, '.'), 4);
}


/* Tests Valid IPv4 Function */
TEST(ValidIPv4, ValidIPv4) {
    EXPECT_TRUE( validIPv4("1.1.1.1") );
    EXPECT_TRUE( validIPv4("10.22.22.22") );
    EXPECT_TRUE( validIPv4("192.168.137.1") );
    EXPECT_TRUE( validIPv4("192.168.137.110") );
    EXPECT_EQ( validIPv4("192.168.137.1"),  3232270593 );
}

TEST(ValidIPv4, Malformed_Input) {
    EXPECT_FALSE( validIPv4("") );
    EXPECT_FALSE( validIPv4(".") );
    EXPECT_FALSE( validIPv4("...") );
    EXPECT_FALSE( validIPv4(" . . . ") );
    EXPECT_FALSE( validIPv4(". . . ") );
    EXPECT_FALSE( validIPv4(" . . .") );
    EXPECT_FALSE( validIPv4(" . . . .") );
    EXPECT_FALSE( validIPv4("192 168 137 100") );
    EXPECT_FALSE( validIPv4(" 192 168 137 100") );
    EXPECT_FALSE( validIPv4("192..168137.1") );   
}

TEST(ValidIPv4, Invalid_Octets) {
    EXPECT_FALSE( validIPv4("1.1.1") );
    EXPECT_FALSE( validIPv4("1.1.1.1.1") );
    EXPECT_FALSE( validIPv4(".192.168.1.1") );
    EXPECT_FALSE( validIPv4("192.168.1.1.") );
    EXPECT_FALSE( validIPv4("123.456.789.101") );
}

TEST(ValidIPv4, Listening_Address) {
    EXPECT_TRUE( validIPv4Listening("0.0.0.0") );

    EXPECT_TRUE( validIPv4("127.0.0.0") );
    EXPECT_TRUE( validIPv4Listening("127.0.0.0") );

    EXPECT_TRUE( validIPv4("192.168.137.0") );
    EXPECT_TRUE( validIPv4Listening("192.168.137.0") );
}

/* Tests of MD5 Hashing Algorithm */
TEST(MD5, MD5_Default_Image) {
    cv::Mat img;
    EXPECT_NO_THROW( img = cv::imread("../assets/default.png") );
    EXPECT_EQ( getImageHash(img), "5b67ac2197fcf1467d046759a642ac77" );
    EXPECT_TRUE(true);
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

/* Test JSON Serialization */
TEST(JSON, Generic_JSON_Serialization)
{
    nlohmann::json j;
    j["name"] = "John Doe";
    j["age"] = 30;
    j["is_student"] = false;

    std::string jsonString = j.dump();
    // std::cout << "Serialized JSON: " << jsonString << std::endl;

    nlohmann::json parsedJson = nlohmann::json::parse(jsonString);
    EXPECT_EQ(parsedJson["name"], "John Doe");
    EXPECT_EQ(parsedJson["age"], 30);
    EXPECT_EQ(parsedJson["is_student"], false);
}

TEST(JSON, SF_JSON_Serialization)
{
    ShannonFano sf;
    std::map<char, double> frequencies;
    std::string input = "hello world";

    EXPECT_NO_THROW(sf.buildCodes(frequencies, input));

    std::map<char, std::string> codes = sf.getCodes();

    nlohmann::json j = codes;

    std::string jsonString = nlohmann::json(codes).dump();
    // std::cout << "Serialized JSON: " << j.dump(2) << std::endl;
}



/* Test Client Code */
TEST(Client, Init)
{
    EXPECT_NO_THROW(Client test);
    Client test_two;

    EXPECT_EQ(test_two.getCurrentState(), state::IDLE_STAGE);
    EXPECT_EQ(test_two.getServerPort(), 39554);
    EXPECT_EQ(test_two.getServerAddress(), std::string("255.255.255.255"));
    EXPECT_ANY_THROW(test_two.connectToServer());
    EXPECT_ANY_THROW(test_two.sendRequestSrv());
}

TEST(Client_Client, Input_Validation_Server_Address_Simple)
{
    Client test;

    EXPECT_TRUE( validIPv4("127.0.0.1") );
    EXPECT_NO_THROW(test.setServerAddress("127.0.0.1"));

    EXPECT_TRUE( validIPv4("192.168.1.1") );
    EXPECT_NO_THROW(test.setServerAddress("192.168.1.1"));

    EXPECT_FALSE( validIPv4("0.0.0.0") );
    EXPECT_ANY_THROW(test.setServerAddress("0.0.0.0"));
}

TEST(Client_setServerAddress, Input_Validation_Server_Address_Empty_Input)
{
    Client test;

    EXPECT_ANY_THROW(test.setServerAddress(""));
    EXPECT_ANY_THROW(test.setServerAddress("."));
    // EXPECT_ANY_THROW(test.setServerAddress("..."));
    // EXPECT_ANY_THROW(test.setServerAddress("...."));
}

TEST(Client_setServerAddress, Input_Validation_Server_Address_Complex_Errors)
{
    Client test;

    EXPECT_ANY_THROW(test.setServerAddress("127.0.0.1."));
    EXPECT_ANY_THROW(test.setServerAddress(".127.0.0.1"));
    EXPECT_ANY_THROW(test.setServerAddress(".127.0...0.1"));
    EXPECT_ANY_THROW(test.setServerAddress("127.0.1"));
    EXPECT_ANY_THROW(test.setServerAddress("127.0.1."));
    EXPECT_ANY_THROW(test.setServerAddress("127..0.1"));
    EXPECT_ANY_THROW(test.setServerAddress(".127.0.1"));
}

TEST(Client_setServerPort, Valid_Addresses)
{
    Client clientObject;

    EXPECT_EQ(clientObject.getServerPort(), 39554);

    EXPECT_NO_THROW(clientObject.setServerPort(1024));
    EXPECT_EQ(clientObject.getServerPort(), 1024);

    EXPECT_NO_THROW(clientObject.setServerPort(65535));
    EXPECT_EQ(clientObject.getServerPort(), 65535);
}

TEST(Client_setServerPort, Invalid_Addresses)
{
    Client clientObject;
    EXPECT_ANY_THROW(clientObject.setServerPort(-1));
    EXPECT_ANY_THROW(clientObject.setServerPort(65536));
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