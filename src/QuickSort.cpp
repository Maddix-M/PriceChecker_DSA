#include "QuickSort.h"

#include <utility>

namespace {

int medianOfThree(std::vector<Product>& items, int low, int high, long long& comparisons) {
    int mid = low + (high - low) / 2;
    if (comesBefore(items[mid], items[low], comparisons)) {
        std::swap(items[mid], items[low]);
    }
    if (comesBefore(items[high], items[low], comparisons)) {
        std::swap(items[high], items[low]);
    }
    if (comesBefore(items[high], items[mid], comparisons)) {
        std::swap(items[high], items[mid]);
    }
    return mid;
}

int partition(std::vector<Product>& items, int low, int high, long long& comparisons) {
    int pivotIndex = medianOfThree(items, low, high, comparisons);
    const Product pivot = items[pivotIndex];

    int i = low - 1;
    int j = high + 1;
    while (true) {
        do {
            ++i;
        } while (comesBefore(items[i], pivot, comparisons));
        do {
            --j;
        } while (comesBefore(pivot, items[j], comparisons));

        if (i >= j) {
            return j;
        }
        std::swap(items[i], items[j]);
    }
}

void quickSortRange(std::vector<Product>& items, int low, int high, long long& comparisons) {
    while (low < high) {
        int split = partition(items, low, high, comparisons);
        // recurse on smaller side so the stack stays small
        if (split - low < high - split) {
            quickSortRange(items, low, split, comparisons);
            low = split + 1;
        } else {
            quickSortRange(items, split + 1, high, comparisons);
            high = split;
        }
    }
}

}  // namespace

long long quickSort(std::vector<Product>& products) {
    long long comparisons = 0;
    if (products.size() > 1) {
        quickSortRange(products, 0, static_cast<int>(products.size()) - 1, comparisons);
    }
    return comparisons;
}
