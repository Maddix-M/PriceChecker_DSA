#pragma once

#include <string>

// writes count rows to a csv, same seed = same file
bool generateProducts(const std::string& path, int count, unsigned int seed, std::string& error);
