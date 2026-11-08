// tests - run with make test

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Benchmark.h"
#include "DataGenerator.h"
#include "DataLoader.h"
#include "MergeSort.h"
#include "ProductFilter.h"
#include "QuickSort.h"
#include "Utils.h"

namespace {

int passed = 0;
int failed = 0;

void check(bool condition, const std::string& name) {
    if (condition) {
        ++passed;
    } else {
        ++failed;
        std::cout << "  FAIL: " << name << "\n";
    }
}

Product makeProduct(int id, int priceCents) {
    Product p;
    p.store = "Amazon";
    p.id = id;
    p.name = "Item " + std::to_string(id);
    p.category = "Test";
    p.priceCents = priceCents;
    p.ratingTenths = 40;
    return p;
}

std::vector<int> idsOf(const std::vector<Product>& v) {
    std::vector<int> ids;
    for (const Product& p : v) ids.push_back(p.id);
    return ids;
}

void checkBothSorts(const std::vector<Product>& input, const std::string& label) {
    std::vector<Product> m = input;
    std::vector<Product> q = input;
    mergeSort(m);
    quickSort(q);

    check(m.size() == input.size() && q.size() == input.size(), label + ": sizes unchanged");
    check(isSortedCheapestFirst(m), label + ": merge sort output is sorted");
    check(isSortedCheapestFirst(q), label + ": quicksort output is sorted");
    check(idsOf(m) == idsOf(q), label + ": both algorithms give the same order");

    std::multiset<int> before, after;
    for (const Product& p : input) before.insert(p.id);
    for (const Product& p : m) after.insert(p.id);
    check(before == after, label + ": no products lost or duplicated");
}

void testSortEdgeCases() {
    checkBothSorts({}, "empty list");
    checkBothSorts({makeProduct(1, 500)}, "one item");
    checkBothSorts({makeProduct(2, 900), makeProduct(1, 100)}, "two items");

    std::vector<Product> ties = {makeProduct(30, 999), makeProduct(10, 999), makeProduct(20, 999),
                                 makeProduct(5, 100)};
    std::vector<Product> m = ties;
    mergeSort(m);
    check(idsOf(m) == std::vector<int>({5, 10, 20, 30}), "equal prices are ordered by product ID");
    checkBothSorts(ties, "equal prices");
}

void testSortLargeInputs() {
    std::mt19937 rng(7);
    std::vector<Product> randomItems;
    for (int i = 0; i < 100000; ++i) {
        // lots of duplicate prices
        randomItems.push_back(makeProduct(i + 1, 100 + static_cast<int>(rng() % 2000)));
    }
    checkBothSorts(randomItems, "100,000 random items");

    std::vector<Product> sorted = randomItems;
    mergeSort(sorted);
    checkBothSorts(sorted, "100,000 already sorted");

    std::vector<Product> reversed(sorted.rbegin(), sorted.rend());
    checkBothSorts(reversed, "100,000 reverse sorted");

    std::vector<Product> allSame;
    for (int i = 0; i < 50000; ++i) allSame.push_back(makeProduct(50000 - i, 1999));
    checkBothSorts(allSame, "50,000 items all the same price");

    std::vector<Product> q = sorted;
    long long quickComparisons = quickSort(q);
    check(quickComparisons < 5LL * 100000 * 17, "quicksort on sorted input is not O(n^2)");
}

void testParsePrice() {
    int cents = 0;
    std::string error;
    check(parsePrice("12", cents, error) && cents == 1200, "parse 12");
    check(parsePrice("12.5", cents, error) && cents == 1250, "parse 12.5 as $12.50");
    check(parsePrice("19.99", cents, error) && cents == 1999, "parse 19.99");
    check(parsePrice("$1,299.99", cents, error) && cents == 129999, "parse $1,299.99");
    check(parsePrice(".99", cents, error) && cents == 99, "parse .99");
    check(parsePrice("  0 ", cents, error) && cents == 0, "parse 0 with spaces");
    check(parsePrice("\xE2\x80\x9C" "25" "\xE2\x80\x9D", cents, error) && cents == 2500, "parse curly-quoted 25");
    check(!parsePrice("-5", cents, error), "reject -5");
    check(!parsePrice("$-5", cents, error), "reject $-5");
    check(!parsePrice("abc", cents, error), "reject letters");
    check(!parsePrice("1.234", cents, error), "reject three decimals");
    check(!parsePrice("1.2.3", cents, error), "reject two dots");
    check(!parsePrice("", cents, error), "reject empty");
    check(!parsePrice(".", cents, error), "reject lone dot");
    check(!parsePrice("99999999", cents, error), "reject huge number");
}

void testParseRating() {
    int tenths = 0;
    std::string error;
    check(parseRating("4", tenths, error) && tenths == 40, "rating 4");
    check(parseRating("4.5", tenths, error) && tenths == 45, "rating 4.5");
    check(parseRating("0", tenths, error) && tenths == 0, "rating 0");
    check(parseRating("5.0", tenths, error) && tenths == 50, "rating 5.0");
    check(!parseRating("5.1", tenths, error), "reject 5.1");
    check(!parseRating("6", tenths, error), "reject 6");
    check(!parseRating("-1", tenths, error), "reject -1");
    check(!parseRating("4.55", tenths, error), "reject 4.55");
}

void testFilter() {
    std::vector<Product> items;
    Product a = makeProduct(1, 1000);  a.name = "Sony Wireless Headphones Pro SO-111"; a.store = "Amazon";
    Product b = makeProduct(2, 2500);  b.name = "Sony Wireless Headphones Pro SO-111"; b.store = "Walmart";
    Product c = makeProduct(3, 5000);  c.name = "JBL Bluetooth Speaker JB-222";        c.store = "Target";
    Product d = makeProduct(4, 2500);  d.name = "Levi's Jeans LE-333"; d.category = "Clothing"; d.ratingTenths = 30;
    items = {a, b, c, d};

    Filter f;
    check(applyFilter(items, f).size() == 4, "empty filter returns everything");

    f.search = "SONY headphones";
    check(applyFilter(items, f).size() == 2, "search is case-insensitive and needs every word");

    f.search = "\xE2\x80\x9C" "speaker" "\xE2\x80\x9D";
    check(applyFilter(items, f).size() == 1, "search ignores curly quotes");

    f.search = "levis";
    check(applyFilter(items, f).size() == 1, "levis matches Levi's");

    f.search = "walmart";
    check(applyFilter(items, f).size() == 1, "search matches store name");

    f.reset();
    f.minCents = 2500;
    f.maxCents = 2500;
    check(applyFilter(items, f).size() == 2, "price range bounds are inclusive");

    f.reset();
    f.minCents = 6000;
    check(applyFilter(items, f).empty(), "no matches gives an empty list");

    f.reset();
    f.minRatingTenths = 35;
    check(applyFilter(items, f).size() == 3, "minimum rating filter");

    f.search = "anything";
    f.reset();
    check(f.search.empty() && f.minCents == 0 && f.maxCents == -1 && f.minRatingTenths == 0,
          "reset clears every filter");
}

void testGeneratorAndLoader() {
    const std::string path1 = "test_products_1.csv";
    const std::string path2 = "test_products_2.csv";
    std::string error;
    check(generateProducts(path1, 100000, 16, error), "generator writes 100,000 rows");
    check(generateProducts(path2, 100000, 16, error), "generator runs a second time");

    std::ifstream f1(path1), f2(path2);
    std::stringstream s1, s2;
    s1 << f1.rdbuf();
    s2 << f2.rdbuf();
    check(s1.str() == s2.str(), "same seed produces the exact same file");

    LoadResult loaded = loadProducts(path1);
    check(loaded.error.empty(), "loader opens the file");
    check(loaded.products.size() == 100000, "loader reads 100,000 products");
    check(loaded.skippedRows == 0, "no malformed rows");

    std::set<int> ids;
    bool pricesOk = true, ratingsOk = true, storesOk = true, noCommas = true;
    for (const Product& p : loaded.products) {
        ids.insert(p.id);
        if (p.priceCents < 100 || p.priceCents > 100000) pricesOk = false;
        if (p.ratingTenths < 10 || p.ratingTenths > 50) ratingsOk = false;
        if (p.store != "Amazon" && p.store != "Walmart" && p.store != "Target") storesOk = false;
        if (p.name.find(',') != std::string::npos) noCommas = false;
    }
    check(ids.size() == loaded.products.size(), "product IDs are unique");
    check(pricesOk, "prices are between $1.00 and $1,000.00");
    check(ratingsOk, "ratings are between 1.0 and 5.0");
    check(storesOk, "every store is Amazon, Walmart or Target");
    check(noCommas, "no commas in product names");

    std::remove(path1.c_str());
    std::remove(path2.c_str());
}

}  // namespace

int main() {
    std::cout << "Running tests...\n";
    testSortEdgeCases();
    testSortLargeInputs();
    testParsePrice();
    testParseRating();
    testFilter();
    testGeneratorAndLoader();
    std::cout << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
