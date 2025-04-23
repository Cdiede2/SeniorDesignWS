#include "shannon-fano.h"

ShannonFano::ShannonFano() : frequencies({}), codes({}), reverseCodes({})
{
    return;
}

ShannonFano::ShannonFano(const std::map<char, double> &frequencies) : frequencies(frequencies), codes({}), reverseCodes({})
{
    return;
}




void ShannonFano::buildCodes( std::map<char, double> &freqs, const std::string &input ) {
    size_t size = input.size();


    if ( ! input.size() ) {
        std::cerr << "Input string is empty." << std::endl;
        throw std::invalid_argument("Input string is empty.");
    }

    // Count Frequencies
    for( char c : input ) {
        freqs[c] += 1;
    }

    // Normalize Frequencies
    for( auto &pair : freqs ) {
        pair.second /= size;
    }

    // Sort By Frequency, Greatest to Least
    std::vector<std::pair<char, double>> freqVec(freqs.begin(), freqs.end());
    std::sort(freqVec.begin(), freqVec.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });
    
    // Build Codes
    buildCodesRecursive(freqVec, "", 0, freqVec.size() - 1);

    
    for(  auto &pair : freqVec ) {
        std::cout << std::format("{}\t|\t{:.2f}\t|\t{:.2f}\t|\t{}", pair.first, pair.second, -std::ceil( std::log2(pair.second)), this->codes[pair.first]) << std::endl;
        std::string code;
        for( size_t i = 0; i < std::ceil( std::log2(pair.second) ); i++ ) {
            code += "0";
        }
    }


    return;
}

void ShannonFano::buildCodesRecursive( std::vector<std::pair<char, double>> &freqs, std::string code, size_t start, size_t end ) {
    if( start == end ) {
        return;
    }

    // Find the split point
    double total = 0;
    for( size_t i = start; i <= end; i++ ) {
        total += freqs[i].second;
    }

    double half = total / 2;
    double sum = 0;
    size_t split = start;
    while( sum < half && split <= end ) {
        sum += freqs[split].second;
        split++;
    }

    // Assign codes to the left and right halves
    for( size_t i = start; i < split; i++ ) {
        this->codes[freqs[i].first] = code + "0";
        this->reverseCodes[code + "0"] = freqs[i].first;
    }
    for( size_t i = split; i <= end; i++ ) {
        this->codes[freqs[i].first] = code + "1";
        this->reverseCodes[code + "1"] = freqs[i].first;
    }

    // Recursively build codes for the left and right halves
    buildCodesRecursive(freqs, code + "0", start, split - 1);
    buildCodesRecursive(freqs, code + "1", split, end);
}
