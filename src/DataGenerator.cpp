#include "DataGenerator.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <random>
#include <unordered_set>
#include <vector>

namespace {

struct ProductType {
    const char* name;
    int minDollars;
    int maxDollars;
};

struct Category {
    const char* name;
    std::vector<ProductType> types;
    std::vector<const char*> brands;
};

const std::vector<Category>& catalog() {
    static const std::vector<Category> categories = {
        {"Electronics",
         {{"Wireless Headphones", 20, 350}, {"Bluetooth Speaker", 15, 250},
          {"Smart Watch", 40, 450},         {"Phone Charger", 5, 40},
          {"USB-C Cable", 3, 25},           {"Laptop", 250, 1000},
          {"Tablet", 80, 900},              {"Gaming Mouse", 10, 120},
          {"Mechanical Keyboard", 25, 200}, {"Webcam", 20, 150},
          {"Computer Monitor", 90, 700},    {"Earbuds", 15, 280}},
         {"Sony", "Samsung", "JBL", "Anker", "Logitech", "Bose", "Lenovo", "Belkin"}},
        {"Home",
         {{"Bath Towel Set", 12, 80},   {"Throw Pillow", 8, 45},
          {"Desk Lamp", 12, 90},        {"Area Rug", 30, 400},
          {"Shower Curtain", 8, 40},    {"Bed Sheet Set", 20, 150},
          {"Comforter", 30, 250},       {"Wall Clock", 10, 70},
          {"Storage Bin", 6, 40},       {"Vacuum Cleaner", 40, 600}},
         {"Casper", "Rubbermaid", "Bissell", "Shark", "Dyson", "Hoover", "Umbra"}},
        {"Kitchen",
         {{"Coffee Maker", 20, 300},         {"Blender", 25, 400},
          {"Air Fryer", 40, 250},            {"Nonstick Frying Pan", 15, 90},
          {"Knife Set", 20, 300},            {"Toaster", 18, 120},
          {"Water Bottle", 8, 50},           {"Cutting Board", 8, 60},
          {"Stand Mixer", 80, 600},          {"Food Storage Containers", 10, 60}},
         {"Ninja", "KitchenAid", "Cuisinart", "Hamilton Beach", "Keurig", "OXO", "Instant Pot", "Stanley"}},
        {"Toys",
         {{"Building Blocks Set", 10, 200}, {"Remote Control Car", 20, 150},
          {"Board Game", 10, 60},           {"1000 Piece Puzzle", 8, 35},
          {"Stuffed Animal", 6, 40},        {"Action Figure", 8, 50},
          {"Art Kit", 10, 60},              {"Toy Kitchen Set", 40, 200},
          {"Mini Drone", 25, 180},          {"Scooter", 30, 200}},
         {"LEGO", "Hasbro", "Mattel", "Melissa and Doug", "Hot Wheels", "Crayola", "Fisher-Price"}},
        {"Sports",
         {{"Yoga Mat", 10, 90},          {"Dumbbell Set", 25, 400},
          {"Basketball", 15, 80},        {"Soccer Ball", 12, 70},
          {"Tennis Racket", 20, 250},    {"Running Shoes", 40, 200},
          {"Jump Rope", 5, 30},          {"Resistance Bands", 8, 45},
          {"Bike Helmet", 25, 150},      {"Foam Roller", 10, 50}},
         {"Nike", "Adidas", "Wilson", "Spalding", "Under Armour", "Gaiam", "CAP", "Puma"}},
        {"Beauty",
         {{"Shampoo", 4, 40},             {"Conditioner", 4, 40},
          {"Face Moisturizer", 8, 80},    {"Sunscreen SPF 50", 6, 35},
          {"Hair Dryer", 20, 400},        {"Electric Toothbrush", 20, 250},
          {"Lip Balm", 2, 15},            {"Body Wash", 4, 25},
          {"Perfume", 20, 180},           {"Makeup Brush Set", 8, 60}},
         {"CeraVe", "Neutrogena", "Dove", "Olay", "Oral-B", "Revlon", "Maybelline", "Philips"}},
        {"Grocery",
         {{"Ground Coffee", 6, 25},          {"Peanut Butter", 3, 12},
          {"Olive Oil", 6, 30},              {"Protein Bar Box", 12, 40},
          {"Pasta", 1, 8},                   {"Cereal", 3, 10},
          {"Green Tea", 3, 15},              {"Trail Mix", 4, 18},
          {"Honey", 5, 25},                  {"Sparkling Water 12-Pack", 4, 12}},
         {"Folgers", "Jif", "Nature Valley", "Quaker", "Barilla", "Kellogg's", "Lipton"}},
        {"Office",
         {{"Spiral Notebook", 2, 15},   {"Gel Pens", 4, 25},
          {"Desk Organizer", 10, 50},   {"Printer Paper", 6, 50},
          {"Stapler", 6, 35},           {"Office Chair", 60, 800},
          {"Backpack", 20, 150},        {"Sticky Notes", 3, 20},
          {"Whiteboard", 15, 150},      {"Scientific Calculator", 10, 160}},
         {"Five Star", "BIC", "Pilot", "Post-it", "Swingline", "JanSport", "Texas Instruments", "Mead"}},
        {"Clothing",
         {{"T-Shirt", 5, 40},         {"Hoodie", 15, 90},
          {"Jeans", 20, 120},         {"Socks 6-Pack", 6, 30},
          {"Rain Jacket", 30, 250},   {"Baseball Cap", 10, 40},
          {"Sneakers", 35, 180},      {"Pajama Set", 15, 70},
          {"Sweatpants", 15, 80},     {"Winter Gloves", 8, 60}},
         {"Hanes", "Levi's", "Champion", "Columbia", "Carhartt", "Gildan", "Fruit of the Loom", "Nike"}},
        {"Outdoors",
         {{"Camping Tent", 40, 600},   {"Sleeping Bag", 25, 300},
          {"Hiking Backpack", 30, 300},{"Cooler", 25, 450},
          {"Camping Chair", 15, 120},  {"Flashlight", 6, 60},
          {"Hammock", 20, 120},        {"Portable Grill", 30, 400},
          {"Garden Hose", 15, 90},     {"Bug Spray", 4, 15}},
         {"Coleman", "Kelty", "YETI", "Thermos", "Weber", "Black Diamond", "Marmot", "Igloo"}},
    };
    return categories;
}

const char* kVariants[] = {"", "", "", "Pro", "Max", "Mini", "Plus", "Lite", "XL", "2-Pack", "Gen 2", "Deluxe"};
const char* kStores[] = {"Amazon", "Walmart", "Target"};

// not using uniform_int_distribution since it differs between compilers
int randInt(std::mt19937& rng, int low, int high) {
    unsigned int span = static_cast<unsigned int>(high - low + 1);
    return low + static_cast<int>(rng() % span);
}

int clampInt(int value, int low, int high) {
    return std::max(low, std::min(value, high));
}

std::string modelPrefix(const std::string& brand) {
    std::string prefix;
    for (char c : brand) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            prefix += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
            if (prefix.size() == 2) {
                break;
            }
        }
    }
    return prefix;
}

}  // namespace

bool generateProducts(const std::string& path, int count, unsigned int seed, std::string& error) {
    std::ofstream out(path);
    if (!out) {
        error = "Could not create " + path;
        return false;
    }

    const int kMinCents = 100;     // $1.00
    const int kMaxCents = 100000;  // $1,000.00
    const int kVariantCount = sizeof(kVariants) / sizeof(kVariants[0]);

    std::mt19937 rng(seed);
    std::unordered_set<std::string> usedNames;
    const auto& categories = catalog();

    out << "store_name,product_id,product_name,category,price_cents,rating\n";

    int written = 0;
    int nextId = 100001;
    while (written < count) {
        const Category& category = categories[randInt(rng, 0, static_cast<int>(categories.size()) - 1)];
        const ProductType& type = category.types[randInt(rng, 0, static_cast<int>(category.types.size()) - 1)];
        std::string brand = category.brands[randInt(rng, 0, static_cast<int>(category.brands.size()) - 1)];
        std::string variant = kVariants[randInt(rng, 0, kVariantCount - 1)];
        int modelNumber = randInt(rng, 100, 999);

        std::string name = brand + " " + type.name;
        if (!variant.empty()) {
            name += " " + variant;
        }
        name += " " + modelPrefix(brand) + "-" + std::to_string(modelNumber);
        if (!usedNames.insert(name).second) {
            continue;
        }

        int basePrice = randInt(rng, type.minDollars * 100, type.maxDollars * 100);
        int baseRating = (randInt(rng, 1, 100) <= 80) ? randInt(rng, 35, 49)
                                                       : randInt(rng, 10, 34);

        // each store sells it ~65% of the time
        bool sells[3];
        bool any = false;
        for (bool& s : sells) {
            s = randInt(rng, 1, 100) <= 65;
            any = any || s;
        }
        if (!any) {
            sells[randInt(rng, 0, 2)] = true;
        }

        for (int s = 0; s < 3 && written < count; ++s) {
            if (!sells[s]) {
                continue;
            }
            int price = static_cast<int>(static_cast<long long>(basePrice) * randInt(rng, 85, 115) / 100);
            if (randInt(rng, 1, 100) <= 60) {
                price = (price / 100) * 100 + 99;
            }
            price = clampInt(price, kMinCents, kMaxCents);
            int rating = clampInt(baseRating + randInt(rng, -3, 3), 10, 50);

            out << kStores[s] << ',' << nextId << ',' << name << ',' << category.name << ','
                << price << ',' << rating / 10 << '.' << rating % 10 << '\n';
            ++nextId;
            ++written;
        }
    }

    if (!out) {
        error = "Something went wrong while writing " + path;
        return false;
    }
    return true;
}
