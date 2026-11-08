#include "Utils.h"

#include <cctype>
#include <sstream>

std::string withCommas(long long number) {
    bool negative = number < 0;
    std::string digits = std::to_string(negative ? -number : number);
    std::string result;
    int count = 0;
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        result.insert(result.begin(), digits[i]);
        if (++count % 3 == 0 && i > 0) {
            result.insert(result.begin(), ',');
        }
    }
    return negative ? "-" + result : result;
}

std::string formatPrice(int cents) {
    int dollars = cents / 100;
    int remainder = cents % 100;
    std::string centsPart = (remainder < 10 ? "0" : "") + std::to_string(remainder);
    return "$" + withCommas(dollars) + "." + centsPart;
}

std::string formatRating(int tenths) {
    return std::to_string(tenths / 10) + "." + std::to_string(tenths % 10);
}

std::string toLower(std::string text) {
    for (char& c : text) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return text;
}

std::string trim(const std::string& text) {
    size_t start = 0;
    while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start]))) {
        ++start;
    }
    size_t end = text.size();
    while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
        --end;
    }
    return text.substr(start, end - start);
}

std::string stripQuotes(const std::string& text) {
    std::string result;
    for (size_t i = 0; i < text.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        if (c == '"' || c == '\'') {
            continue;
        }
        // curly quotes (utf-8)
        if (c == 0xE2 && i + 2 < text.size() &&
            static_cast<unsigned char>(text[i + 1]) == 0x80) {
            unsigned char third = static_cast<unsigned char>(text[i + 2]);
            if (third == 0x98 || third == 0x99 || third == 0x9C || third == 0x9D) {
                i += 2;
                continue;
            }
        }
        result += text[i];
    }
    return result;
}

std::vector<std::string> splitWords(const std::string& text) {
    std::vector<std::string> words;
    std::istringstream stream(text);
    std::string word;
    while (stream >> word) {
        words.push_back(word);
    }
    return words;
}

static bool allDigits(const std::string& s) {
    for (char c : s) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

bool parsePrice(const std::string& text, int& cents, std::string& error) {
    std::string s = trim(stripQuotes(text));
    if (s.empty()) {
        error = "Please enter a price.";
        return false;
    }
    if (s.find('-') != std::string::npos) {
        error = "Prices can't be negative.";
        return false;
    }
    if (s[0] == '$') {
        s.erase(0, 1);
    }
    std::string cleaned;
    for (char c : s) {
        if (c != ',') {
            cleaned += c;
        }
    }

    size_t dot = cleaned.find('.');
    std::string dollarPart = cleaned.substr(0, dot);
    std::string centPart = (dot == std::string::npos) ? "" : cleaned.substr(dot + 1);

    if (dollarPart.empty() && centPart.empty()) {
        error = "That isn't a price. Try something like 25 or 19.99.";
        return false;
    }
    if (!allDigits(dollarPart) || !allDigits(centPart)) {
        error = "That isn't a price. Try something like 25 or 19.99.";
        return false;
    }
    if (centPart.size() > 2) {
        error = "Use at most two decimal places, like 19.99.";
        return false;
    }
    if (dollarPart.size() > 7) {
        error = "That price is too large.";
        return false;
    }

    int dollars = dollarPart.empty() ? 0 : std::stoi(dollarPart);
    int extraCents = 0;
    if (centPart.size() == 1) {
        extraCents = (centPart[0] - '0') * 10;
    } else if (centPart.size() == 2) {
        extraCents = std::stoi(centPart);
    }
    cents = dollars * 100 + extraCents;
    return true;
}

bool parseRating(const std::string& text, int& tenths, std::string& error) {
    std::string s = trim(stripQuotes(text));
    if (s.empty()) {
        error = "Please enter a rating.";
        return false;
    }
    if (s.find('-') != std::string::npos) {
        error = "Ratings can't be negative.";
        return false;
    }
    size_t dot = s.find('.');
    std::string wholePart = s.substr(0, dot);
    std::string decimalPart = (dot == std::string::npos) ? "" : s.substr(dot + 1);

    if ((wholePart.empty() && decimalPart.empty()) || !allDigits(wholePart) ||
        !allDigits(decimalPart) || wholePart.size() > 1) {
        error = "Enter a rating from 0 to 5, like 4 or 4.5.";
        return false;
    }
    if (decimalPart.size() > 1) {
        error = "Use one decimal place at most, like 4.5.";
        return false;
    }
    int whole = wholePart.empty() ? 0 : wholePart[0] - '0';
    int decimal = decimalPart.empty() ? 0 : decimalPart[0] - '0';
    int value = whole * 10 + decimal;
    if (value > 50) {
        error = "Ratings only go up to 5.";
        return false;
    }
    tenths = value;
    return true;
}
