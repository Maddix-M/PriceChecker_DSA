#pragma once

#include <string>
#include <vector>

std::string formatPrice(int cents);        // 123456 -> $1,234.56
std::string formatRating(int tenths);      // 43 -> 4.3
std::string withCommas(long long number);  // 100000 -> 100,000

std::string toLower(std::string text);
std::string trim(const std::string& text);
std::string stripQuotes(const std::string& text);  // removes straight and curly quotes
std::vector<std::string> splitWords(const std::string& text);

// return false and set error if the input is invalid
bool parsePrice(const std::string& text, int& cents, std::string& error);
bool parseRating(const std::string& text, int& tenths, std::string& error);
