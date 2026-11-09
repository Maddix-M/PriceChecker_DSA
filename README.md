# Price Filter

COP3530 Project 2 - Gauss Glazers (Yousef Dakkak, Maddix Mehaffie, Ryan Bos)

Search 100,000 product listings from Amazon, Walmart and Target, filter them by price range and rating, and see the matches from cheapest to most expensive. The results are sorted with merge sort or quicksort, both written from scratch without std::sort, and the program can time and compare the two.

## How to run

You just need a C++17 compiler.

Download the full PriceChecker_DSA-main.zip folder structure into the downloads folder as a zip file

On Mac or Linux, open a terminal in the project folder and run make, then ./PriceChecker_DSA-main

On Windows, run this in PowerShell from the project folder:

g++ -std=c++17 -O2 -Isrc -o price_filter.exe src/main.cpp src/Utils.cpp src/DataGenerator.cpp src/DataLoader.cpp src/MergeSort.cpp src/QuickSort.cpp src/Benchmark.cpp src/ProductFilter.cpp src/Menu.cpp

g++ -std=c++17 (Get-ChildItem .\src\*.cpp | ForEach-Object FullName) -I .\include -o price_filter.exe

then .\price_filter.exe

The first time it runs it creates products.csv with all 100,000 products. You can also run it with --regenerate to rebuild that file, or --benchmark to just run the benchmark. make test runs the tests.

## Using it

The menu lets you search for a product, set a price range, set a minimum rating, reset the filters, show the results, pick which sort to use, compare both sorts on your results, and run the full benchmark.

Search works on the product name, category and store, so things like sony headphones, air fryer or walmart laptop all work. Prices can be typed like 25 or 19.99. It won't take negative prices, letters, or a minimum that's higher than the maximum. If nothing matches it tells you instead of showing an empty list.

## Who did what

Yousef - data generator, CSV loader, merge sort
Maddix - quicksort, benchmark and timing, tests
Ryan - price filter, console menu, build files

## The data

Each row has the store name, product ID, product name, category, price in cents and rating. Prices go from $1.00 to $1,000.00 and are stored in cents so there's no rounding. Product IDs are unique but prices repeat a lot. Most products are sold at more than one store at slightly different prices, so you can compare where it's cheapest. The seed is fixed so everyone gets the same file.

## How the sorts work

Both sort by price, cheapest first. If two products cost the same, the lower product ID goes first. We did that because quicksort isn't stable, so without it the two sorts could put same-priced items in a different order and our check that both give the same result would fail.

Merge sort splits the list in half, sorts each half and merges them back together. It's always O(n log n) and needs an extra array the same size as the input.

Quicksort picks a pivot using the median of the first, middle and last items, then uses Hoare partitioning. We used median of three because picking the first item as the pivot makes already sorted input O(n^2). It also recurses on the smaller side so the stack doesn't get too deep with 100,000 items.

## Results

Times are from one run with -O2. They'll be different on other computers but the comparison counts stay the same.

| Size | Input order | Merge ms | Quick ms | Merge comparisons | Quick comparisons | n log2 n |
|---|---|---|---|---|---|---|
| 1,000 | Random | 0.44 | 0.29 | 8,721 | 15,175 | 9,965 |
| 10,000 | Random | 5.87 | 4.10 | 120,481 | 200,138 | 132,877 |
| 50,000 | Random | 28.69 | 16.96 | 718,242 | 1,118,517 | 780,482 |
| 100,000 | Random | 61.29 | 36.53 | 1,536,249 | 2,341,806 | 1,660,964 |
| 100,000 | Already sorted | 44.19 | 10.77 | 815,024 | 2,068,924 | 1,660,964 |
| 100,000 | Reverse sorted | 45.13 | 12.56 | 853,904 | 2,068,925 | 1,660,964 |

Both grow like n log n. Going from 50,000 to 100,000 about doubles the time.

Quicksort makes about 50% more comparisons but is still about 1.7 times faster on random data. Comparisons aren't the only cost. Merge sort copies every product into the extra array and back at every level, and each product has three strings in it. Quicksort sorts in place and only swaps items that are on the wrong side of the pivot.

On sorted input merge sort does about half the comparisons but still copies everything, so it only gets around 30% faster. Quicksort gets about 3 times faster because median of three picks the exact middle every time and almost nothing gets swapped.

Overall, merge sort is more predictable and stable but uses more memory. Quicksort was faster for our data and uses less memory, but it has an O(n^2) worst case and isn't stable. For small searches with only a few dozen results, both finish so fast the difference doesn't matter.

## Tests

The tests check both sorts on empty lists, one item, equal prices, 100,000 random items, already sorted and reverse sorted lists. Each case checks the output is sorted, both sorts give the same order, and nothing got lost. They also test the price input (things like 12.5 and $1,299.99 work, and negative numbers, letters and too many decimals get rejected), the rating input, the search and price filter, and that the generator makes 100,000 valid rows and the same file every time.
