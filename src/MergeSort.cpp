#include "MergeSort.h"

#include <utility>

namespace {

// merge [low, mid) and [mid, high)
void merge(std::vector<Product>& items, std::vector<Product>& buffer,
           size_t low, size_t mid, size_t high, long long& comparisons) {
    size_t left = low;
    size_t right = mid;
    size_t out = low;

    while (left < mid && right < high) {
        if (comesBefore(items[right], items[left], comparisons)) {
            buffer[out++] = std::move(items[right++]);
        } else {
            buffer[out++] = std::move(items[left++]);
        }
    }
    while (left < mid) {
        buffer[out++] = std::move(items[left++]);
    }
    while (right < high) {
        buffer[out++] = std::move(items[right++]);
    }
    for (size_t i = low; i < high; ++i) {
        items[i] = std::move(buffer[i]);
    }
}

void mergeSortRange(std::vector<Product>& items, std::vector<Product>& buffer,
                    size_t low, size_t high, long long& comparisons) {
    if (high - low < 2) {
        return;
    }
    size_t mid = low + (high - low) / 2;
    mergeSortRange(items, buffer, low, mid, comparisons);
    mergeSortRange(items, buffer, mid, high, comparisons);
    merge(items, buffer, low, mid, high, comparisons);
}

}  // namespace

long long mergeSort(std::vector<Product>& products) {
    long long comparisons = 0;
    std::vector<Product> buffer(products.size());
    mergeSortRange(products, buffer, 0, products.size(), comparisons);
    return comparisons;
}
