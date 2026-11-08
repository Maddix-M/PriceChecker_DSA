#include "DataLoader.h"

#include <fstream>
#include <sstream>

#include "Utils.h"

namespace {

std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, ',')) {
        fields.push_back(field);
    }
    return fields;
}

bool toInt(const std::string& text, int& value) {
    try {
        size_t used = 0;
        value = std::stoi(text, &used);
        return used == text.size();
    } catch (...) {
        return false;
    }
}

}  // namespace

LoadResult loadProducts(const std::string& path) {
    LoadResult result;
    std::ifstream in(path);
    if (!in) {
        result.error = "Could not open " + path;
        return result;
    }

    std::string line;
    std::getline(in, line);  // header

    result.products.reserve(100000);
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }

        std::vector<std::string> fields = splitCsvLine(line);
        if (fields.size() != 6) {
            ++result.skippedRows;
            continue;
        }

        Product p;
        p.store = fields[0];
        p.name = fields[2];
        p.category = fields[3];
        std::string ratingError;
        if (!toInt(fields[1], p.id) || !toInt(fields[4], p.priceCents) ||
            !parseRating(fields[5], p.ratingTenths, ratingError)) {
            ++result.skippedRows;
            continue;
        }
        result.products.push_back(p);
    }
    return result;
}
