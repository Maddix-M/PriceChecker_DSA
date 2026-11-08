step 1: winget install -e --id BrechtSanders.WinLibs.POSIX.UCRT
--accept-source-agreements --accept-package-agreements


step 2: cd $HOME\Downloads
Remove-Item -Recurse -Force PriceFilter -ErrorAction SilentlyContinue
Expand-Archive -Force PriceFilter.zip .
cd PriceFilter
g++ -std=c++17 -O2 -Isrc -o price_filter.exe src/main.cpp
src/Utils.cpp src/DataGenerator.cpp src/DataLoader.cpp
src/MergeSort.cpp src/QuickSort.cpp src/Benchmark.cpp
src/ProductFilter.cpp src/Menu.cpp
.\price_filter.exe

running it later: cd $HOME\Downloads\PriceFilter
.\price_filter.exe
