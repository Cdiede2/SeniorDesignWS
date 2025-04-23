#include <gtest/gtest.h>
#include <iostream>
#include <format>
#include <openssl/md5.h>
#include <fstream>
#include "shannon-fano.h"
// #include "md5.h"


std::string convertHashToString(const uint8_t* digest);



// Test Gtest framework is correctly linked
// and working
TEST(BASIC, BASIC) {
    EXPECT_TRUE(true);
}

/* Tests of MD5 Hashing Algorithm */
TEST(MD5, MD5_String) {
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
    for( std::string& input : inputs ) {
        std::string result;
        MD5( reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), digest );
        
        result = convertHashToString(digest);
        EXPECT_EQ(result, expected_hashes[i]);
        i++;
    }
}

TEST(MD5, MD5_File) {
    MD5_CTX ctx;
    uint8_t digest[16];
    std::ifstream file("/home/jay/Desktop/SeniorDesign_WS/Code/cursed-2.jpg", std::ios::binary);
    if (!file) {
        std::cerr << "Could not open file for reading." << std::endl;
        return;
    }

    MD5_Init(&ctx);
    char buffer[1024];
    while (file.read(buffer, sizeof(buffer))) {
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
TEST(SFComp, SFComp_INIT) {
    ShannonFano sf;
    std::map<char, double> frequencies;
}   

TEST(SFComp, SFComp_Empty) {
    ShannonFano sf;
    std::map<char, double> frequencies;
    EXPECT_ANY_THROW(sf.buildCodes(frequencies, ""));
    // sf.buildCodes(frequencies, "");
    EXPECT_TRUE(frequencies.empty());
}

TEST(SFComp, SFComp_SingleChar) {
    ShannonFano sf;
    std::map<char, double> frequencies;
    sf.buildCodes(frequencies, "a");
    EXPECT_EQ(frequencies.size(), 1);
    EXPECT_EQ(frequencies['a'], 1.0);
}





/* Utility Functions */
std::string convertHashToString(const uint8_t* digest) {
    std::string result;
    for (int i = 0; i < 16; ++i) {
        result += std::format("{:02x}", digest[i]);
    }
    return result;
}