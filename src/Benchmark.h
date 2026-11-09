#pragma once

#include <string>
#include <vector>

#include "Product.h"

enum class Algorithm { MergeSort, QuickSort };

std::string algorithmName(Algorithm algorithm);

struct SortRun {
    double milliseconds = 0.0;
    long long comparisons = 0;
};

SortRun timeSort(Algorithm algorithm, std::vector<Product>& products);

struct SortComparison {
    size_t itemCount = 0;
    SortRun merge;
    SortRun quick;
    bool mergeIsSorted = false;
    bool quickIsSorted = false;
    bool sameOrder = false;
};

SortComparison compareSorts(const std::vector<Product>& input, int repeats);
bool isSortedCheapestFirst(const std::vector<Product>& products);
void printComparison(const SortComparison& result);
void runFullBenchmark(const std::vector<Product>& allProducts, const std::string& csvPath);
