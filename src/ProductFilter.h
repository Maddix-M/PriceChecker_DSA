#pragma once

#include <string>
#include <vector>

#include "Product.h"

struct Filter {
    std::string search;
    int minCents = 0;
    int maxCents = -1;  // -1 = no max
    int minRatingTenths = 0;

    void reset() { *this = Filter(); }
    bool hasPriceRange() const { return minCents > 0 || maxCents >= 0; }
};

// every search word has to be in the name, category or store
std::vector<Product> applyFilter(const std::vector<Product>& products, const Filter& filter);
