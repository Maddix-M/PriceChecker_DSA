#pragma once

#include <string>
#include <vector>

#include "Benchmark.h"
#include "Product.h"
#include "ProductFilter.h"

class Menu {
public:
    explicit Menu(const std::vector<Product>& products);
    void run();

private:
    const std::vector<Product>& products_;
    Filter filter_;
    Algorithm algorithm_ = Algorithm::MergeSort;
    bool quit_ = false;

    void printHeader() const;
    void searchProduct();
    void setPriceRange();
    void setMinimumRating();
    void resetFilters();
    void showResults();
    void chooseAlgorithm();
    void compareOnCurrentResults();
    void runBenchmark();

    void printMatchCount();
    void printTablePage(const std::vector<Product>& items, size_t start, size_t count) const;
    void waitForEnter();
    bool readLine(const std::string& prompt, std::string& line);
};
