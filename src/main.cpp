// Price Filter - COP3530 Project 2
// Gauss Glazers: Yousef Dakkak, Maddix Mehaffie, Ryan Bos
// usage: ./price_filter [--regenerate] [--benchmark]

#include <fstream>
#include <iostream>
#include <string>

#include "Benchmark.h"
#include "DataGenerator.h"
#include "DataLoader.h"
#include "Menu.h"
#include "Utils.h"

namespace {
const std::string kDataFile = "products.csv";
const int kProductCount = 100000;
const unsigned int kSeed = 16;
}  // namespace

int main(int argc, char* argv[]) {
    bool regenerate = false;
    bool benchmarkOnly = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--regenerate") {
            regenerate = true;
        } else if (arg == "--benchmark") {
            benchmarkOnly = true;
        } else {
            std::cout << "Unknown option: " << arg << "\n"
                      << "Options: --regenerate, --benchmark\n";
            return 1;
        }
    }

    bool fileExists = static_cast<bool>(std::ifstream(kDataFile));
    if (regenerate || !fileExists) {
        std::cout << "Generating " << withCommas(kProductCount) << " products into " << kDataFile << "...\n";
        std::string error;
        if (!generateProducts(kDataFile, kProductCount, kSeed, error)) {
            std::cout << "Error: " << error << "\n";
            return 1;
        }
    }

    LoadResult loaded = loadProducts(kDataFile);
    if (!loaded.error.empty()) {
        std::cout << "Error: " << loaded.error << "\n";
        return 1;
    }
    if (loaded.skippedRows > 0) {
        std::cout << "Warning: skipped " << loaded.skippedRows << " malformed rows in " << kDataFile << "\n";
    }
    if (loaded.products.empty()) {
        std::cout << "Error: " << kDataFile << " has no products. Run with --regenerate.\n";
        return 1;
    }
    std::cout << "Loaded " << withCommas(static_cast<long long>(loaded.products.size())) << " products from "
              << kDataFile << ".\n";

    if (benchmarkOnly) {
        runFullBenchmark(loaded.products, "benchmark_results.csv");
        return 0;
    }

    Menu menu(loaded.products);
    menu.run();
    return 0;
}
