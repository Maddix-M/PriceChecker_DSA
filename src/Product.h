#pragma once

#include <string>

struct Product {
    std::string store;
    int id = 0;
    std::string name;
    std::string category;
    int priceCents = 0;    // stored in cents to avoid rounding
    int ratingTenths = 0;  // 43 = 4.3 stars
};

// cheapest first, ties broken by id so both sorts give the same order
inline bool comesBefore(const Product& a, const Product& b, long long& comparisons) {
    ++comparisons;
    if (a.priceCents != b.priceCents) {
        return a.priceCents < b.priceCents;
    }
    return a.id < b.id;
}
