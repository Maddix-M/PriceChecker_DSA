#include "Menu.h"

#include <algorithm>
#include <iomanip>
#include <iostream>

#include "Utils.h"

namespace {

const char* kStores[] = {"Amazon", "Walmart", "Target"};
const size_t kPageSize = 15;

std::string describePriceRange(const Filter& f) {
    if (!f.hasPriceRange()) {
        return "any price";
    }
    std::string low = formatPrice(f.minCents);
    std::string high = f.maxCents >= 0 ? formatPrice(f.maxCents) : "no max";
    return low + " - " + high;
}

std::string describeRating(const Filter& f) {
    return f.minRatingTenths > 0 ? formatRating(f.minRatingTenths) + "+ stars" : "any rating";
}

std::string fitWidth(const std::string& text, size_t width) {
    if (text.size() <= width) {
        return text;
    }
    return text.substr(0, width - 2) + "..";
}

}  // namespace

Menu::Menu(const std::vector<Product>& products) : products_(products) {}

bool Menu::readLine(const std::string& prompt, std::string& line) {
    std::cout << prompt;
    if (!std::getline(std::cin, line)) {
        quit_ = true;
        std::cout << "\n";
        return false;
    }
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return true;
}

void Menu::waitForEnter() {
    std::string ignored;
    readLine("\n  Press Enter to go back to the menu...", ignored);
}

void Menu::run() {
    while (!quit_) {
        printHeader();
        std::string choice;
        if (!readLine("  Choose an option: ", choice)) {
            break;
        }
        choice = trim(choice);

        if (choice.empty()) {
            continue;
        } else if (choice == "1") {
            searchProduct();
        } else if (choice == "2") {
            setPriceRange();
        } else if (choice == "3") {
            setMinimumRating();
        } else if (choice == "4") {
            resetFilters();
        } else if (choice == "5") {
            showResults();
        } else if (choice == "6") {
            chooseAlgorithm();
        } else if (choice == "7") {
            compareOnCurrentResults();
        } else if (choice == "8") {
            runBenchmark();
        } else if (choice == "0" || toLower(choice) == "q") {
            std::cout << "  Bye\n";
            break;
        } else {
            std::cout << "  \"" << choice << "\" isn't an option. Enter a number from 0 to 8.\n";
        }
    }
}

void Menu::printHeader() const {
    std::cout << "\n==============================================================\n";
    std::cout << "   PRICE FILTER                                 Gauss Glazers\n";
    std::cout << "   Amazon | Walmart | Target    " << withCommas(static_cast<long long>(products_.size()))
              << " products loaded\n";
    std::cout << "==============================================================\n";
    std::cout << "   Search:       " << (filter_.search.empty() ? "(all products)" : filter_.search) << "\n";
    std::cout << "   Price range:  " << describePriceRange(filter_) << "\n";
    std::cout << "   Min rating:   " << describeRating(filter_) << "\n";
    std::cout << "   Sorting with: " << algorithmName(algorithm_) << "\n";
    std::cout << "--------------------------------------------------------------\n";
    std::cout << "   1. Search for a product\n";
    std::cout << "   2. Set price range\n";
    std::cout << "   3. Set minimum rating\n";
    std::cout << "   4. Reset filters\n";
    std::cout << "   5. Show results (cheapest first)\n";
    std::cout << "   6. Choose sorting algorithm\n";
    std::cout << "   7. Compare Merge Sort vs Quicksort on these results\n";
    std::cout << "   8. Run full benchmark (1K to 100K products)\n";
    std::cout << "   0. Exit\n";
    std::cout << "--------------------------------------------------------------\n";
}

void Menu::printMatchCount() {
    std::vector<Product> matches = applyFilter(products_, filter_);
    if (matches.empty()) {
        std::cout << "  No products match your search and filters.\n";
        return;
    }
    size_t perStore[3] = {0, 0, 0};
    for (const Product& p : matches) {
        for (int s = 0; s < 3; ++s) {
            if (p.store == kStores[s]) {
                ++perStore[s];
            }
        }
    }
    std::cout << "  " << withCommas(static_cast<long long>(matches.size())) << " products match  (Amazon "
              << withCommas(static_cast<long long>(perStore[0])) << ", Walmart "
              << withCommas(static_cast<long long>(perStore[1])) << ", Target "
              << withCommas(static_cast<long long>(perStore[2])) << ").\n";
}

void Menu::searchProduct() {
    std::cout << "\n  Search by product, brand, category or store.\n";
    std::cout << "  Examples: headphones, air fryer, nike running shoes, walmart laptop\n";
    std::string text;
    if (!readLine("  Search (leave blank for all products): ", text)) {
        return;
    }
    filter_.search = trim(stripQuotes(text));
    printMatchCount();
}

void Menu::setPriceRange() {
    std::cout << "\n  Enter prices in dollars, like 25 or 19.99.\n";
    while (!quit_) {
        int minCents = 0;
        int maxCents = -1;
        std::string text;
        std::string error;

        while (true) {
            if (!readLine("  Minimum price (Enter for $0): ", text)) {
                return;
            }
            if (trim(text).empty()) {
                minCents = 0;
                break;
            }
            if (parsePrice(text, minCents, error)) {
                break;
            }
            std::cout << "  " << error << "\n";
        }

        while (true) {
            if (!readLine("  Maximum price (Enter for no maximum): ", text)) {
                return;
            }
            if (trim(text).empty()) {
                maxCents = -1;
                break;
            }
            if (parsePrice(text, maxCents, error)) {
                break;
            }
            std::cout << "  " << error << "\n";
        }

        if (maxCents >= 0 && minCents > maxCents) {
            std::cout << "  The minimum (" << formatPrice(minCents) << ") can't be more than the maximum ("
                      << formatPrice(maxCents) << "). Try again.\n";
            continue;
        }

        filter_.minCents = minCents;
        filter_.maxCents = maxCents;
        std::cout << "  Price range set to " << describePriceRange(filter_) << ".\n";
        printMatchCount();
        return;
    }
}

void Menu::setMinimumRating() {
    std::string text;
    std::string error;
    while (true) {
        if (!readLine("\n  Minimum star rating, 0 to 5 (Enter for any rating): ", text)) {
            return;
        }
        if (trim(text).empty()) {
            filter_.minRatingTenths = 0;
            break;
        }
        int tenths = 0;
        if (parseRating(text, tenths, error)) {
            filter_.minRatingTenths = tenths;
            break;
        }
        std::cout << "  " << error << "\n";
    }
    std::cout << "  Showing " << describeRating(filter_) << ".\n";
    printMatchCount();
}

void Menu::resetFilters() {
    filter_.reset();
    std::cout << "\n  Filters reset. Showing all " << withCommas(static_cast<long long>(products_.size()))
              << " products at any price and rating.\n";
}

void Menu::chooseAlgorithm() {
    std::cout << "\n  1. Merge Sort\n  2. Quicksort\n";
    std::string text;
    while (readLine("  Pick a sorting algorithm: ", text)) {
        text = trim(text);
        if (text == "1") {
            algorithm_ = Algorithm::MergeSort;
            break;
        }
        if (text == "2") {
            algorithm_ = Algorithm::QuickSort;
            break;
        }
        std::cout << "  Enter 1 or 2.\n";
    }
    if (!quit_) {
        std::cout << "  Results will be sorted with " << algorithmName(algorithm_) << ".\n";
    }
}

void Menu::printTablePage(const std::vector<Product>& items, size_t start, size_t count) const {
    std::cout << "\n  " << std::left << std::setw(7) << "#" << std::setw(9) << "Store" << std::right
              << std::setw(11) << "Price" << std::setw(8) << "Rating" << "   " << std::left << std::setw(52)
              << "Product" << std::setw(13) << "Category" << "ID\n";
    std::cout << "  " << std::string(109, '-') << "\n";
    size_t end = std::min(start + count, items.size());
    for (size_t i = start; i < end; ++i) {
        const Product& p = items[i];
        std::cout << "  " << std::left << std::setw(7) << (i + 1) << std::setw(9) << p.store << std::right
                  << std::setw(11) << formatPrice(p.priceCents) << std::setw(8) << formatRating(p.ratingTenths)
                  << "   " << std::left << std::setw(52) << fitWidth(p.name, 50) << std::setw(13) << p.category
                  << p.id << "\n";
    }
    std::cout << "  " << std::string(109, '-') << "\n";
    std::cout << "  Showing " << withCommas(static_cast<long long>(start + 1)) << "-"
              << withCommas(static_cast<long long>(end)) << " of " << withCommas(static_cast<long long>(items.size()))
              << "\n";
}

void Menu::showResults() {
    std::vector<Product> results = applyFilter(products_, filter_);
    if (results.empty()) {
        std::cout << "\n  No products match your search (" << (filter_.search.empty() ? "all" : filter_.search)
                  << "), price range (" << describePriceRange(filter_) << ") and rating ("
                  << describeRating(filter_) << ").\n";
        std::cout << "  Try a wider price range, a different search, or option 4 to reset filters.\n";
        waitForEnter();
        return;
    }

    SortRun run = timeSort(algorithm_, results);

    std::cout << "\n  Found " << withCommas(static_cast<long long>(results.size())) << " products. Sorted with "
              << algorithmName(algorithm_) << " in " << std::fixed << std::setprecision(3) << run.milliseconds
              << " ms (" << withCommas(run.comparisons) << " comparisons).\n";

    std::cout << "\n  Cheapest at each store:\n";
    for (const char* store : kStores) {
        const Product* cheapest = nullptr;
        for (const Product& p : results) {
            if (p.store == store) {
                cheapest = &p;
                break;
            }
        }
        std::cout << "    " << std::left << std::setw(9) << store;
        if (cheapest) {
            std::cout << std::right << std::setw(11) << formatPrice(cheapest->priceCents) << "  "
                      << formatRating(cheapest->ratingTenths) << " stars  " << cheapest->name << "\n";
        } else {
            std::cout << "  no matches\n";
        }
    }
    std::cout << "  Best price overall: " << formatPrice(results[0].priceCents) << " at " << results[0].store
              << "\n";

    size_t start = 0;
    while (!quit_) {
        printTablePage(results, start, kPageSize);
        bool lastPage = start + kPageSize >= results.size();
        std::string input;
        std::string prompt = lastPage ? "  [p] previous page   [Enter] back to menu: "
                                      : "  [Enter] next page   [p] previous page   [q] back to menu: ";
        if (!readLine(prompt, input)) {
            return;
        }
        input = toLower(trim(input));
        if (input == "q" || (input.empty() && lastPage)) {
            return;
        }
        if (input == "p") {
            start = start >= kPageSize ? start - kPageSize : 0;
        } else if (!lastPage) {
            start += kPageSize;
        }
    }
}

void Menu::compareOnCurrentResults() {
    std::vector<Product> results = applyFilter(products_, filter_);
    if (results.empty()) {
        std::cout << "\n  No products match your filters, so there is nothing to sort. Try option 4 to reset.\n";
        waitForEnter();
        return;
    }
    int repeats = results.size() < 10000 ? 25 : 3;
    std::cout << "\n  Sorting the same " << withCommas(static_cast<long long>(results.size()))
              << " products with both algorithms (average of " << repeats << " runs)...\n";
    SortComparison comparison = compareSorts(results, repeats);
    printComparison(comparison);
    if (results.size() < 1000) {
        std::cout << "  Note: with this few products the times are tiny and mostly noise.\n"
                  << "  Option 8 compares them on up to 100,000 products.\n";
    }
    waitForEnter();
}

void Menu::runBenchmark() {
    std::cout << "\n  Running both algorithms on 1K, 10K, 50K and 100K products in three input\n"
              << "  orders. This takes a few seconds...\n";
    runFullBenchmark(products_, "benchmark_results.csv");
    waitForEnter();
}
