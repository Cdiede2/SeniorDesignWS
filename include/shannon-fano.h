#pragma once
#define SHANNON_FANO_H

#include <cmath>
#include <map>
#include <vector>
#include <algorithm>


#include <iostream>
#include <format>


class ShannonFano
{
public:
    ShannonFano();
    ShannonFano(const std::map<char, double> &frequencies);

    /**
     * @brief Encodes the input data using the Shannon-Fano algorithm.
     * @details
     */
    void encode();

    /**
     * @brief Decodes the input data using the Shannon-Fano algorithm.
     * @param encoded The encoded string to decode.
     * @details
     */
    void decode(const std::string &encoded);

    /**
     * @brief Prints the codes for each character.
     * @details
     */
    std::map<char, std::string> getCodes() const;

    /**
     * @brief Prints the decoded string.
     * @param encoded The encoded string to decode.
     * @details
     */
    void printDecoded(const std::string &encoded) const;

    void buildCodes(std::map<char, double> &freqs, const std::string &input);

private:
    std::map<char, double> frequencies;
    std::map<char, std::string> codes;
    std::map<std::string, char> reverseCodes;

    void sortFrequencies();
    void normalizeFrequencies();
    void buildCodesRecursive(std::vector<std::pair<char, double>> &freqs, std::string code, size_t start, size_t end);
};

template <class T>
std::vector<T> quicksort(std::vector<T> &input)
{
    if (input.size() <= 1)
        return input;

    T pivot = input[input.size() / 2];
    std::vector<T> less, equal, greater;

    for (const T &item : input)
    {
        if (item < pivot)
            less.push_back(item);
        else if (item == pivot)
            equal.push_back(item);
        else
            greater.push_back(item);
    }

    less = quicksort(less);
    greater = quicksort(greater);

    std::vector<T> result;
    result.insert(result.end(), less.begin(), less.end());
    result.insert(result.end(), equal.begin(), equal.end());
    result.insert(result.end(), greater.begin(), greater.end());

    return result;
}