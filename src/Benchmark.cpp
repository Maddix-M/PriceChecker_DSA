#include "Benchmark.h"

#include <chrono>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <utility>

#include "MergeSort.h"
#include "QuickSort.h"
#include "Utils.h"

std::string algorithmName(Algorithm algorithm) {
    return algorithm == Algorithm::MergeSort ? "Merge Sort" : "Quicksort";
}

SortRun timeSort(Algorithm algorithm, std::vector<Product>& products) {
    SortRun run;
    auto start = std::chrono::steady_clock::now();
    run.comparisons = (algorithm == Algorithm::MergeSort) ? mergeSort(products) : quickSort(products);
    auto end = std::chrono::steady_clock::now();
    run.milliseconds = std::chrono::duration<double, std::milli>(end - start).count();
    return run;
}

bool isSortedCheapestFirst(const std::vector<Product>& products) {
    long long unused = 0;
    for (size_t i = 1; i < products.size(); ++i) {
        if (comesBefore(products[i], products[i - 1], unused)) {
            return false;
        }
    }
    return true;
}

static bool sameIdOrder(const std::vector<Product>& a, const std::vector<Product>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i].id != b[i].id) {
            return false;
        }
    }
    return true;
}

SortComparison compareSorts(const std::vector<Product>& input, int repeats) {
    if (repeats < 1) {
        repeats = 1;
    }
    SortComparison result;
    result.itemCount = input.size();

    std::vector<Product> mergeCopy;
    std::vector<Product> quickCopy;
    double mergeTotal = 0.0;
    double quickTotal = 0.0;

    for (int r = 0; r < repeats; ++r) {
        mergeCopy = input;
        SortRun m = timeSort(Algorithm::MergeSort, mergeCopy);
        quickCopy = input;
        SortRun q = timeSort(Algorithm::QuickSort, quickCopy);

        mergeTotal += m.milliseconds;
        quickTotal += q.milliseconds;
        result.merge.comparisons = m.comparisons;
        result.quick.comparisons = q.comparisons;
    }
    result.merge.milliseconds = mergeTotal / repeats;
    result.quick.milliseconds = quickTotal / repeats;

    result.mergeIsSorted = isSortedCheapestFirst(mergeCopy);
    result.quickIsSorted = isSortedCheapestFirst(quickCopy);
    result.sameOrder = sameIdOrder(mergeCopy, quickCopy);
    return result;
}

static std::string fixed3(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(3) << value;
    return out.str();
}

void printComparison(const SortComparison& r) {
    std::cout << "\n  Algorithm comparison on " << withCommas(static_cast<long long>(r.itemCount))
              << " products\n";
    std::cout << "  ---------------------------------------------------\n";
    std::cout << "  " << std::left << std::setw(14) << "Algorithm" << std::right << std::setw(14)
              << "Time (ms)" << std::setw(18) << "Comparisons" << "\n";
    std::cout << "  " << std::left << std::setw(14) << "Merge Sort" << std::right << std::setw(14)
              << fixed3(r.merge.milliseconds) << std::setw(18) << withCommas(r.merge.comparisons) << "\n";
    std::cout << "  " << std::left << std::setw(14) << "Quicksort" << std::right << std::setw(14)
              << fixed3(r.quick.milliseconds) << std::setw(18) << withCommas(r.quick.comparisons) << "\n";
    std::cout << "  ---------------------------------------------------\n";

    if (r.itemCount > 1) {
        double n = static_cast<double>(r.itemCount);
        std::cout << "  For reference, n * log2(n) = " << withCommas(static_cast<long long>(n * std::log2(n)))
                  << "\n";
    }
    if (r.merge.milliseconds > 0 && r.quick.milliseconds > 0) {
        bool mergeFaster = r.merge.milliseconds < r.quick.milliseconds;
        double ratio = mergeFaster ? r.quick.milliseconds / r.merge.milliseconds
                                   : r.merge.milliseconds / r.quick.milliseconds;
        std::cout << "  " << (mergeFaster ? "Merge Sort" : "Quicksort") << " was " << std::fixed
                  << std::setprecision(2) << ratio << "x faster.\n";
    }
    std::cout << "  Both lists sorted correctly: " << (r.mergeIsSorted && r.quickIsSorted ? "YES" : "NO")
              << "\n";
    std::cout << "  Both gave the same order:    " << (r.sameOrder ? "YES" : "NO") << "\n";
}

void runFullBenchmark(const std::vector<Product>& allProducts, const std::string& csvPath) {
    // fisher-yates shuffle with fixed seed
    std::vector<Product> shuffled = allProducts;
    std::mt19937 rng(2026);
    for (size_t i = shuffled.size(); i > 1; --i) {
        size_t j = rng() % i;
        std::swap(shuffled[i - 1], shuffled[j]);
    }

    const int kRepeats = 3;
    std::vector<size_t> sizes = {1000, 10000, 50000, 100000};

    std::ofstream csv(csvPath);
    csv << "size,input_order,merge_ms,quick_ms,merge_comparisons,quick_comparisons,n_log2_n,same_order\n";

    std::cout << "\n  Full benchmark (average of " << kRepeats << " runs each)\n";
    std::cout << "  " << std::string(104, '-') << "\n";
    std::cout << "  " << std::left << std::setw(10) << "Size" << std::setw(16) << "Input order" << std::right
              << std::setw(12) << "Merge ms" << std::setw(12) << "Quick ms" << std::setw(16) << "Merge comps"
              << std::setw(16) << "Quick comps" << std::setw(14) << "n*log2(n)" << std::setw(8) << "Same?"
              << "\n";
    std::cout << "  " << std::string(104, '-') << "\n";

    for (size_t size : sizes) {
        if (size > shuffled.size()) {
            size = shuffled.size();
        }
        std::vector<Product> randomOrder(shuffled.begin(), shuffled.begin() + static_cast<std::ptrdiff_t>(size));
        std::vector<Product> sortedOrder = randomOrder;
        mergeSort(sortedOrder);
        std::vector<Product> reversedOrder(sortedOrder.rbegin(), sortedOrder.rend());

        struct Case {
            const char* label;
            const std::vector<Product>* data;
        };
        Case cases[] = {{"Random", &randomOrder}, {"Already sorted", &sortedOrder}, {"Reverse sorted", &reversedOrder}};

        for (const Case& c : cases) {
            SortComparison r = compareSorts(*c.data, kRepeats);
            long long nLogN = static_cast<long long>(size * std::log2(static_cast<double>(size)));
            bool ok = r.sameOrder && r.mergeIsSorted && r.quickIsSorted;

            std::cout << "  " << std::left << std::setw(10) << withCommas(static_cast<long long>(size))
                      << std::setw(16) << c.label << std::right << std::setw(12) << fixed3(r.merge.milliseconds)
                      << std::setw(12) << fixed3(r.quick.milliseconds) << std::setw(16)
                      << withCommas(r.merge.comparisons) << std::setw(16) << withCommas(r.quick.comparisons)
                      << std::setw(14) << withCommas(nLogN) << std::setw(8) << (ok ? "YES" : "NO") << "\n";

            csv << size << ',' << c.label << ',' << fixed3(r.merge.milliseconds) << ','
                << fixed3(r.quick.milliseconds) << ',' << r.merge.comparisons << ',' << r.quick.comparisons
                << ',' << nLogN << ',' << (ok ? "yes" : "no") << '\n';
        }
        if (size == shuffled.size()) {
            break;
        }
    }
    std::cout << "  " << std::string(104, '-') << "\n";
    std::cout << "  Saved these numbers to " << csvPath << "\n";
}
