#include "ProductFilter.h"

#include "Utils.h"

std::vector<Product> applyFilter(const std::vector<Product>& products, const Filter& filter) {
    std::vector<std::string> words = splitWords(toLower(stripQuotes(filter.search)));
    std::vector<Product> matches;

    for (const Product& p : products) {
        if (p.priceCents < filter.minCents) {
            continue;
        }
        if (filter.maxCents >= 0 && p.priceCents > filter.maxCents) {
            continue;
        }
        if (p.ratingTenths < filter.minRatingTenths) {
            continue;
        }
        if (!words.empty()) {
            std::string haystack = toLower(stripQuotes(p.name + " " + p.category + " " + p.store));
            bool allFound = true;
            for (const std::string& word : words) {
                if (haystack.find(word) == std::string::npos) {
                    allFound = false;
                    break;
                }
            }
            if (!allFound) {
                continue;
            }
        }
        matches.push_back(p);
    }
    return matches;
}
