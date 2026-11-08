#pragma once

#include <vector>

#include "Product.h"

// median of three pivot + hoare partition, returns number of comparisons
long long quickSort(std::vector<Product>& products);
