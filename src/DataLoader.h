#pragma once

#include <string>
#include <vector>

#include "Product.h"

struct LoadResult {
    std::vector<Product> products;
    int skippedRows = 0;
    std::string error;
};

LoadResult loadProducts(const std::string& path);
