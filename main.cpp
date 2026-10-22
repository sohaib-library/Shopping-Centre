#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>
#include "httplib.h"

using namespace std;

inline string jsonEscape(const string& input) {
    string out;
    out.reserve(input.size() + 8);
    for (unsigned char c : input) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': break;
            case '\t': out += "\\t"; break;
            default:
                if (c >= 0x20) out += static_cast<char>(c);
                break;
        }
    }
    return out;
}

inline string fileField(const string& input) {
    string out;
    out.reserve(input.size());
    for (char c : input) {
        if (c == '|' || c == '\n' || c == '\r') out += ' ';
        else out += c;
    }
    return out;
}

// Forward declaration friend function ke liye
class ShoppingMall;
class Customer;

// ==========================================
// 1. BASE CLASS: Product
// ==========================================
class Product {
protected:
    int id;
    string title;
    string brand;
    float price;
    int stock;
    string category;

public:
    Product() {
        id = 0;
        title = "";
        brand = "";
        price = 0.0f;
        stock = 0;
        category = "general";
    }

    Product(int i, string t, string b, float p, int s, string c = "general") {
        id = i;
        title = t;
        brand = b;
        price = p;
        stock = s;
        category = c;
    }

    int getId() const { return id; }
    string getTitle() const { return title; }
    string getBrand() const { return brand; }
    float getPrice() const { return price; }
    int getStock() const { return stock; }
    string getCategory() const { return category; }

    void setTitle(const string& t) { title = t; }
    void setBrand(const string& b) { brand = b; }
    void setPrice(float p) { price = p; }
    void setStock(int s) { stock = s; }
    void setCategory(const string& c) { category = c; }

    bool checkAndDeduct(int qty) {
        if (stock >= qty) {
            stock -= qty;
            return true;
        }
        return false;
    }

    void restock(int qty) {
        if (qty > 0) {
            stock += qty;
        }
    }

    virtual string getExtra() const { return ""; }
    virtual void setExtra(const string& val) {}
    virtual string getType() const { return category; }

    // Virtual Function (Runtime Polymorphism)
    virtual void showInfo() {
        cout << id << "\t" << title << "\t" << brand << "\tRs. " << price << "\tQty: " << stock;
    }

    virtual string toJson() {
        stringstream ss;
        ss << "{\"id\": " << id 
           << ", \"title\": \"" << jsonEscape(title) 
           << "\", \"brand\": \"" << jsonEscape(brand) 
           << "\", \"price\": " << price 
           << ", \"stock\": " << stock 
           << ", \"extra\": \"" << jsonEscape(getExtra()) << "\""
           << ", \"type\": \"" << jsonEscape(getType()) << "\"}";
        return ss.str();
    }

    virtual ~Product() {}
};

// ==========================================
// 2. INHERITANCE: Electronics
// ==========================================
class Electronics : public Product {
private:
    int warrantyYear;

public:
    Electronics(int i, string t, string b, float p, int s, int w)
        : Product(i, t, b, p, s, "electronics"), warrantyYear(w) {}

    string getExtra() const override { return to_string(warrantyYear); }
    void setExtra(const string& val) override {
        try { warrantyYear = stoi(val); } catch (...) { warrantyYear = 1; }
    }
    string getType() const override { return "electronics"; }

    // Function overriding
    void showInfo() override {
        Product::showInfo();
        cout << "\t[Warranty: " << warrantyYear << " Year]" << endl;
    }

    string toJson() override {
        stringstream ss;
        ss << "{\"id\": " << id 
           << ", \"title\": \"" << jsonEscape(title) 
           << "\", \"brand\": \"" << jsonEscape(brand) 
           << "\", \"price\": " << price 
           << ", \"stock\": " << stock 
           << ", \"warrantyYear\": " << warrantyYear 
           << ", \"extra\": \"" << jsonEscape(to_string(warrantyYear) + " Yr Warranty") << "\""
           << ", \"type\": \"electronics\"}";
        return ss.str();
    }
};

// ==========================================
// 3. INHERITANCE: Clothing
// ==========================================
class Clothing : public Product {
private:
    string size;

public:
    Clothing(int i, string t, string b, float p, int s, string sz)
        : Product(i, t, b, p, s, "clothing"), size(sz) {}

    string getExtra() const override { return size; }
    void setExtra(const string& val) override { size = val; }
    string getType() const override { return "clothing"; }

    // Function overriding
    void showInfo() override {
        Product::showInfo();
        cout << "\t[Size: " << size << "]" << endl;
    }

    string toJson() override {
        stringstream ss;
        ss << "{\"id\": " << id 
           << ", \"title\": \"" << jsonEscape(title) 
           << "\", \"brand\": \"" << jsonEscape(brand) 
           << "\", \"price\": " << price 
           << ", \"stock\": " << stock 
           << ", \"size\": \"" << jsonEscape(size) 
           << "\", \"extra\": \"" << jsonEscape("Size: " + size) << "\""
           << ", \"type\": \"clothing\"}";
        return ss.str();
    }
};

// ==========================================
// 3B. INHERITANCE: General / Custom Category Product
// ==========================================
class GeneralProduct : public Product {
private:
    string extraSpec;

public:
    GeneralProduct(int i, string t, string b, float p, int s, string cat, string spec)
        : Product(i, t, b, p, s, cat), extraSpec(spec) {}

    string getExtra() const override { return extraSpec; }
    void setExtra(const string& val) override { extraSpec = val; }
    string getType() const override { return category; }

    void showInfo() override {
        Product::showInfo();
        cout << "\t[" << category << ": " << extraSpec << "]" << endl;
    }

    string toJson() override {
        stringstream ss;
        ss << "{\"id\": " << id 
           << ", \"title\": \"" << jsonEscape(title) 
           << "\", \"brand\": \"" << jsonEscape(brand) 
           << "\", \"price\": " << price 
           << ", \"stock\": " << stock 
           << ", \"extra\": \"" << jsonEscape(extraSpec) << "\""
           << ", \"type\": \"" << jsonEscape(category) << "\"}";
        return ss.str();
    }
};

// ==========================================
// 4. CUSTOMER CLASS
// ==========================================
class Customer {
private:
    string name;
    int cartIds[10];
    int cartQty[10];
    int itemCount;

public:
    Customer(string n) {
        name = n;
        itemCount = 0;
    }

    void addItem(int pId, int qty) {
        if (itemCount < 10) {
            cartIds[itemCount] = pId;
            cartQty[itemCount] = qty;
            itemCount++;
        } else {
            cout << "Cart is full!\n";
        }
    }

    void emptyCart() {
        itemCount = 0;
    }

    // Friend function declaration
    friend void printReceipt(Customer &c, ShoppingMall &mall);
};

// ==========================================
// 4B. CATEGORY MODEL
// ==========================================
struct CategoryInfo {
    string key;
    string label;
};

// ==========================================
// 5. MAIN PLATFORM / SHOPPING MALL
// ==========================================
class ShoppingMall {
private:
    vector<Product*> inventory;
    vector<CategoryInfo> categories;
    int nextProductId;
    static int orderCounter; // Static data member

public:
    ShoppingMall() {
        nextProductId = 1;
        loadCategories();
        loadProducts();
    }

    ~ShoppingMall() {
        clearInventory();
    }

    void clearInventory() {
        for (Product* p : inventory) {
            delete p;
        }
        inventory.clear();
    }

    // Static function
    static int getOrders() {
        return orderCounter;
    }

    static void addOrder() {
        orderCounter++;
    }

    // ------------------------------------------
    // Category Management & Persistence
    // ------------------------------------------
    void loadCategories() {
        categories.clear();
        ifstream file("categories.txt");
        if (!file.is_open()) {
            categories.push_back({"electronics", "Electronics"});
            categories.push_back({"clothing", "Fashion & Apparel"});
            saveCategories();
            return;
        }

        string line;
        while (getline(file, line)) {
            while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' ')) line.pop_back();
            if (line.empty()) continue;
            stringstream ss(line);
            string k, l;
            getline(ss, k, '|');
            getline(ss, l, '|');
            if (!k.empty()) {
                if (l.empty()) l = k;
                categories.push_back({k, l});
            }
        }
        file.close();
        if (categories.empty()) {
            categories.push_back({"electronics", "Electronics"});
            categories.push_back({"clothing", "Fashion & Apparel"});
            saveCategories();
        }
    }

    void saveCategories() {
        ofstream out("categories.txt");
        for (const auto& cat : categories) {
            out << cat.key << "|" << cat.label << "\n";
        }
        out.close();
    }

    bool addCategory(const string& key, const string& label) {
        if (key.empty()) return false;
        for (const auto& cat : categories) {
            if (cat.key == key) return false;
        }
        categories.push_back({key, label.empty() ? key : label});
        saveCategories();
        return true;
    }

    string getCategoriesJson() {
        stringstream ss;
        ss << "[";
        for (size_t i = 0; i < categories.size(); ++i) {
            ss << "{\"key\":\"" << jsonEscape(categories[i].key) << "\",\"label\":\"" << jsonEscape(categories[i].label) << "\"}";
            if (i + 1 < categories.size()) ss << ",";
        }
        ss << "]";
        return ss.str();
    }

    // ------------------------------------------
    // Product Persistence & CRUD Operations
    // ------------------------------------------
    void loadProducts() {
        clearInventory();
        ifstream file("products.txt");
        if (!file.is_open()) {
            // Seed initial default products
            inventory.push_back(new Electronics(1, "iPhone 15", "Apple", 270000, 5, 1));
            inventory.push_back(new Electronics(2, "S23 Ultra", "Samsung", 240000, 4, 2));
            inventory.push_back(new Electronics(3, "Headphones", "Sony", 45000, 8, 1));
            inventory.push_back(new Clothing(4, "Hoodie", "Nike", 6500, 15, "L"));
            inventory.push_back(new Clothing(5, "Tracksuit", "Adidas", 8500, 10, "M"));
            inventory.push_back(new Clothing(6, "Formal Shirt", "Outfitters", 4200, 12, "XL"));
            nextProductId = 7;
            saveProducts();
            return;
        }

        string line;
        int maxId = 0;
        while (getline(file, line)) {
            while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) line.pop_back();
            if (line.empty()) continue;

            stringstream ss(line);
            string idStr, title, brand, priceStr, stockStr, cat, extra;
            getline(ss, idStr, '|');
            getline(ss, title, '|');
            getline(ss, brand, '|');
            getline(ss, priceStr, '|');
            getline(ss, stockStr, '|');
            getline(ss, cat, '|');
            getline(ss, extra, '|');

            if (idStr.empty() || title.empty()) continue;

            int id = 0;
            float price = 0.0f;
            int stock = 0;
            try { id = stoi(idStr); } catch (...) { continue; }
            try { price = stof(priceStr); } catch (...) { price = 0.0f; }
            try { stock = stoi(stockStr); } catch (...) { stock = 0; }

            if (id > maxId) maxId = id;

            if (cat == "electronics") {
                int w = 1;
                try { w = stoi(extra); } catch (...) { w = 1; }
                inventory.push_back(new Electronics(id, title, brand, price, stock, w));
            } else if (cat == "clothing") {
                if (extra.empty()) extra = "M";
                inventory.push_back(new Clothing(id, title, brand, price, stock, extra));
            } else {
                inventory.push_back(new GeneralProduct(id, title, brand, price, stock, cat, extra));
            }
        }
        file.close();

        if (inventory.empty()) {
            inventory.push_back(new Electronics(1, "iPhone 15", "Apple", 270000, 5, 1));
            inventory.push_back(new Electronics(2, "S23 Ultra", "Samsung", 240000, 4, 2));
            inventory.push_back(new Electronics(3, "Headphones", "Sony", 45000, 8, 1));
            inventory.push_back(new Clothing(4, "Hoodie", "Nike", 6500, 15, "L"));
            inventory.push_back(new Clothing(5, "Tracksuit", "Adidas", 8500, 10, "M"));
            inventory.push_back(new Clothing(6, "Formal Shirt", "Outfitters", 4200, 12, "XL"));
            maxId = 6;
            saveProducts();
        }
        nextProductId = maxId + 1;
    }

    void saveProducts() {
        ofstream out("products.txt");
        for (Product* p : inventory) {
            if (p != nullptr) {
                out << p->getId() << "|" 
                    << fileField(p->getTitle()) << "|" 
                    << fileField(p->getBrand()) << "|" 
                    << p->getPrice() << "|" 
                    << p->getStock() << "|" 
                    << fileField(p->getType()) << "|" 
                    << fileField(p->getExtra()) << "\n";
            }
        }
        out.close();
    }

    Product* searchProduct(int id) {
        for (Product* p : inventory) {
            if (p != nullptr && p->getId() == id) {
                return p;
            }
        }
        return nullptr;
    }

    Product* addProduct(const string& title, const string& brand, float price, int stock, const string& category, const string& extra) {
        int id = nextProductId++;
        Product* newProd = nullptr;
        if (category == "electronics") {
            int w = 1;
            try { w = stoi(extra); } catch (...) { w = 1; }
            newProd = new Electronics(id, title, brand, price, stock, w);
        } else if (category == "clothing") {
            newProd = new Clothing(id, title, brand, price, stock, extra.empty() ? "M" : extra);
        } else {
            newProd = new GeneralProduct(id, title, brand, price, stock, category, extra);
        }

        inventory.push_back(newProd);
        saveProducts();
        return newProd;
    }

    bool updateProduct(int id, const string& title, const string& brand, float price, int stock, const string& category, const string& extra) {
        for (size_t i = 0; i < inventory.size(); ++i) {
            if (inventory[i] != nullptr && inventory[i]->getId() == id) {
                if (inventory[i]->getType() != category) {
                    delete inventory[i];
                    if (category == "electronics") {
                        int w = 1;
                        try { w = stoi(extra); } catch (...) { w = 1; }
                        inventory[i] = new Electronics(id, title, brand, price, stock, w);
                    } else if (category == "clothing") {
                        inventory[i] = new Clothing(id, title, brand, price, stock, extra.empty() ? "M" : extra);
                    } else {
                        inventory[i] = new GeneralProduct(id, title, brand, price, stock, category, extra);
                    }
                } else {
                    inventory[i]->setTitle(title);
                    inventory[i]->setBrand(brand);
                    inventory[i]->setPrice(price);
                    inventory[i]->setStock(stock);
                    inventory[i]->setExtra(extra);
                }
                saveProducts();
                return true;
            }
        }
        return false;
    }

    bool deleteProduct(int id) {
        for (auto it = inventory.begin(); it != inventory.end(); ++it) {
            if (*it != nullptr && (*it)->getId() == id) {
                delete *it;
                inventory.erase(it);
                saveProducts();
                return true;
            }
        }
        return false;
    }

    void displayAll() {
        cout << "\n---------------- AVAILABLE INVENTORY ----------------\n";
        cout << "ID\tItem\t\tBrand\t\tPrice\t\tStock\tDetails\n";
        cout << "-----------------------------------------------------\n";
        for (Product* p : inventory) {
            // Polymorphism in action
            if (p != nullptr) p->showInfo();
        }
        cout << "-----------------------------------------------------\n";
    }

    string getProductsJson() {
        stringstream ss;
        ss << "[";
        for (size_t i = 0; i < inventory.size(); ++i) {
            if (inventory[i] != nullptr) {
                ss << inventory[i]->toJson();
                if (i + 1 < inventory.size()) ss << ",";
            }
        }
        ss << "]";
        return ss.str();
    }

    // Function Overloading (Bill Calculation)
    float calcBill(float amount) {
        return amount;
    }

    float calcBill(float amount, float discountPercent) {
        return amount - (amount * (discountPercent / 100.0));
    }

    bool restockProduct(int id, int qty) {
        Product* p = searchProduct(id);
        if (p != nullptr && qty > 0) {
            p->restock(qty);
            saveProducts();
            return true;
        }
        return false;
    }

    int getTotalStock() {
        int total = 0;
        for (Product* p : inventory) {
            if (p != nullptr) {
                total += p->getStock();
            }
        }
        return total;
    }

    // Friend function declaration
    friend void printReceipt(Customer &c, ShoppingMall &mall);
};

// Static variable initialize karna lazmi hota hai bahar
int ShoppingMall::orderCounter = 0;

// ==========================================
// 6. FRIEND FUNCTION & FILE HANDLING
// ==========================================
void printReceipt(Customer &c, ShoppingMall &mall) {
    if (c.itemCount == 0) {
        cout << "\n[Cart Khali Hai, Kuch Add Karein!]\n";
        return;
    }

    float total = 0.0;
    cout << "\n========== CUSTOMER BILL ==========\n";
    cout << "Customer Name: " << c.name << "\n";
    cout << "Purchased Items:\n";

    for (int i = 0; i < c.itemCount; i++) {
        Product* p = mall.searchProduct(c.cartIds[i]);
        if (p != NULL) {
            float itemTotal = p->getPrice() * c.cartQty[i];
            total += itemTotal;
            cout << "- " << p->getTitle() << " (" << p->getBrand() << ") x " 
                 << c.cartQty[i] << " = Rs. " << itemTotal << "\n";
        }
    }

    // Overloaded function call (5% discount lagaya)
    float finalBill = mall.calcBill(total, 5.0);

    cout << "-----------------------------------\n";
    cout << "Total Amount:      Rs. " << total << "\n";
    cout << "Bill After 5% Off: Rs. " << finalBill << "\n";
    cout << "===================================\n";

    // File Handling (Stream write)
    ofstream file("receipts.txt", ios::app);
    if (file.is_open()) {
        file << "Order #" << (ShoppingMall::getOrders() + 1) << " | Name: " << c.name << "\n";
        file << "Final Total: Rs. " << finalBill << "\n";
        file << "--------------------------------------\n";
        file.close();
        cout << ">> Bill successfully saved to 'receipts.txt'\n";
    }

    ShoppingMall::addOrder();
    c.emptyCart();
}

// File Reading (Stream read)
void showHistory() {
    ifstream file("receipts.txt");
    if (!file.is_open()) {
        cout << "\n[Koi record mojood nahi hai.]\n";
        return;
    }

    string line;
    cout << "\n---------- SALES HISTORY ----------\n";
    while (getline(file, line)) {
        cout << line << endl;
    }
    cout << "-----------------------------------\n";
    file.close();
}

// Struct for order records
struct OrderRecord {
    string orderNum;
    string customer;
    float total;
};

// Helper to parse receipts.txt
vector<OrderRecord> parseReceipts() {
    vector<OrderRecord> orders;
    ifstream file("receipts.txt");
    if (!file.is_open()) return orders;

    string line;
    OrderRecord cur;
    bool hasOrder = false;

    while (getline(file, line)) {
        if (line.find("Order #") == 0) {
            size_t hashPos = line.find("#");
            size_t barPos = line.find("|");
            size_t namePos = line.find("Name: ");
            if (hashPos != string::npos && barPos != string::npos) {
                cur.orderNum = line.substr(hashPos + 1, barPos - (hashPos + 1));
                while (!cur.orderNum.empty() && (cur.orderNum.back() == ' ' || cur.orderNum.back() == '\t')) cur.orderNum.pop_back();
            }
            if (namePos != string::npos) {
                cur.customer = line.substr(namePos + 6);
                while (!cur.customer.empty() && (cur.customer.back() == ' ' || cur.customer.back() == '\r')) cur.customer.pop_back();
            }
            hasOrder = true;
        } else if (line.find("Final Total: Rs. ") == 0) {
            string totalStr = line.substr(17);
            try {
                cur.total = stof(totalStr);
            } catch (...) {
                cur.total = 0.0f;
            }
        } else if (line.find("---") != string::npos && hasOrder) {
            orders.push_back(cur);
            cur = OrderRecord();
            hasOrder = false;
        }
    }
    if (hasOrder) {
        orders.push_back(cur);
    }
    file.close();
    return orders;
}

// ==========================================
// USER ACCOUNTS & PERSISTENCE
// ==========================================
struct UserAccount {
    string name;
    string username;
    string email;
    string password;
    string role; // "customer" or "admin"
};

inline vector<UserAccount> loadUsers() {
    vector<UserAccount> users;
    ifstream file("users.txt");
    if (!file.is_open()) {
        // Create initial default seed users
        ofstream out("users.txt");
        out << "Ahmed Khan|ahmed|ahmed@example.com|user123|customer\n";
        out << "sohaib|sohaib|sohaib@example.com|user123|customer\n";
        out << "osama|osama|osama@example.com|user123|customer\n";
        out << "Administrator|admin|admin@novacart.com|admin123|admin\n";
        out.close();
        file.open("users.txt");
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string name, username, email, pass, role;
        getline(ss, name, '|');
        getline(ss, username, '|');
        getline(ss, email, '|');
        getline(ss, pass, '|');
        getline(ss, role, '|');
        if (!username.empty()) {
            if (role.empty()) role = "customer";
            users.push_back({name, username, email, pass, role});
        }
    }
    file.close();
    return users;
}

inline bool saveNewUser(const UserAccount& newUser) {
    ofstream out("users.txt", ios::app);
    if (!out.is_open()) return false;
    out << newUser.name << "|" << newUser.username << "|" << newUser.email << "|" << newUser.password << "|" << newUser.role << "\n";
    out.close();
    return true;
}

// ==========================================
// JSON PARSING UTILITIES
// ==========================================
inline string jsonExtractStr(const string& body, const string& key) {
    string search = "\"" + key + "\"";
    size_t pos = body.find(search);
    if (pos == string::npos) return "";
    size_t colon = body.find(":", pos + search.length());
    if (colon == string::npos) return "";
    size_t firstQuote = body.find("\"", colon + 1);
    if (firstQuote == string::npos) return "";
    size_t secondQuote = body.find("\"", firstQuote + 1);
    if (secondQuote == string::npos) return "";
    return body.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

inline float jsonExtractFloat(const string& body, const string& key) {
    string search = "\"" + key + "\"";
    size_t pos = body.find(search);
    if (pos == string::npos) return 0.0f;
    size_t colon = body.find(":", pos + search.length());
    if (colon == string::npos) return 0.0f;
    size_t start = colon + 1;
    while (start < body.length() && (body[start] == ' ' || body[start] == '\t' || body[start] == '"')) start++;
    size_t end = start;
    while (end < body.length() && (isdigit(body[end]) || body[end] == '.' || body[end] == '-')) end++;
    if (start >= end) return 0.0f;
    try {
        return stof(body.substr(start, end - start));
    } catch (...) {
        return 0.0f;
    }
}

inline int jsonExtractInt(const string& body, const string& key) {
    return (int)jsonExtractFloat(body, key);
}

// ==========================================
// 7. MAIN FUNCTION
// ==========================================
int main() {
    ShoppingMall myStore;
    httplib::Server svr;

    // Serve static files (HTML, CSS, JS) from the current directory
    svr.set_mount_point("/", "./");

    // ==========================================
    // MIDDLEWARE LAYER (Logging, Security & Auth)
    // ==========================================
    svr.set_pre_routing_handler([&](const httplib::Request& req, httplib::Response& res) {
        // 1. Logger Middleware
        cout << "[MIDDLEWARE LOG] " << req.method << " " << req.path << " from " << req.remote_addr << endl;

        // 2. CORS & Security Headers Middleware
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Admin-Key");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");

        if (req.method == "OPTIONS") {
            res.status = 200;
            return httplib::Server::HandlerResponse::Handled;
        }

        // 3. Admin Authentication Middleware for /api/admin/* (except login)
        if (req.path.find("/api/admin/") == 0 && req.path != "/api/admin/login") {
            string adminKey = req.get_header_value("X-Admin-Key");
            if (adminKey != "novacart-admin-key-2026") {
                res.status = 401;
                res.set_content("{\"status\": \"error\", \"message\": \"401 Unauthorized: Admin middleware key required\"}", "application/json");
                return httplib::Server::HandlerResponse::Handled;
            }
        }

        return httplib::Server::HandlerResponse::Unhandled;
    });

    // API to get products
    svr.Get("/api/products", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_content(myStore.getProductsJson(), "application/json");
    });

    // API to buy a product
    svr.Post("/api/buy", [&](const httplib::Request& req, httplib::Response& res) {
        string body = req.body;
        int pId = -1;
        int qty = -1;
        string customerName = "Guest";
        
        size_t pidPos = body.find("\"pId\":");
        if(pidPos != string::npos) pId = stoi(body.substr(pidPos + 6));
        
        size_t qtyPos = body.find("\"qty\":");
        if(qtyPos != string::npos) qty = stoi(body.substr(qtyPos + 6));
        
        size_t custPos = body.find("\"customer\":\"");
        if(custPos != string::npos) {
            size_t start = custPos + 12;
            size_t end = body.find("\"", start);
            if(end != string::npos) customerName = body.substr(start, end - start);
        }

        Product* item = myStore.searchProduct(pId);
        if (item != NULL) {
            if (item->checkAndDeduct(qty)) {
                myStore.saveProducts();
                Customer currentCustomer(customerName);
                currentCustomer.addItem(pId, qty);
                printReceipt(currentCustomer, myStore);
                
                res.set_content("{\"status\": \"success\", \"message\": \"Purchase successful!\"}", "application/json");
            } else {
                res.set_content("{\"status\": \"error\", \"message\": \"Out of stock or invalid quantity!\"}", "application/json");
            }
        } else {
            res.set_content("{\"status\": \"error\", \"message\": \"Invalid Product ID!\"}", "application/json");
        }
    });

    // API to get history (All for admin, or filtered by customer for user-side)
    svr.Get("/api/history", [&](const httplib::Request& req, httplib::Response& res) {
        string targetCustomer = "";
        if (req.has_param("customer")) {
            targetCustomer = req.get_param_value("customer");
        } else if (req.has_header("X-Customer-User")) {
            targetCustomer = req.get_header_value("X-Customer-User");
        }

        auto trim = [](string s) -> string {
            while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || s.back() == '\n')) s.pop_back();
            size_t start = 0;
            while (start < s.length() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n')) start++;
            return s.substr(start);
        };

        auto toLower = [](string s) -> string {
            for (char& c : s) c = tolower(c);
            return s;
        };

        targetCustomer = trim(targetCustomer);
        string lowerTarget = toLower(targetCustomer);

        ifstream file("receipts.txt");
        if (!file.is_open()) {
            res.set_content("{\"status\": \"success\", \"history\": \"No history available.\", \"orders\": [], \"count\": 0}", "application/json");
            return;
        }

        string line;
        string currentBlock = "";
        string currentCustomer = "";
        string currentOrderNum = "";
        float currentTotal = 0.0f;
        string filteredHistory = "";
        vector<OrderRecord> matchedOrders;

        while (getline(file, line)) {
            currentBlock += line + "\n";
            if (line.find("Order #") == 0) {
                size_t hashPos = line.find("#");
                size_t barPos = line.find("|");
                size_t namePos = line.find("Name: ");
                if (hashPos != string::npos && barPos != string::npos) {
                    currentOrderNum = trim(line.substr(hashPos + 1, barPos - (hashPos + 1)));
                }
                if (namePos != string::npos) {
                    currentCustomer = trim(line.substr(namePos + 6));
                }
            } else if (line.find("Final Total: Rs. ") == 0) {
                string totalStr = line.substr(17);
                try {
                    currentTotal = stof(totalStr);
                } catch (...) {
                    currentTotal = 0.0f;
                }
            } else if (line.find("---") != string::npos) {
                bool match = false;
                if (lowerTarget.empty()) {
                    match = true;
                } else {
                    string lowerCur = toLower(currentCustomer);
                    if (lowerCur == lowerTarget || 
                        (!lowerTarget.empty() && lowerCur.find(lowerTarget) != string::npos) ||
                        (!lowerCur.empty() && lowerTarget.find(lowerCur) != string::npos)) {
                        match = true;
                    }
                }

                if (match) {
                    filteredHistory += currentBlock;
                    OrderRecord rec;
                    rec.orderNum = currentOrderNum;
                    rec.customer = currentCustomer;
                    rec.total = currentTotal;
                    matchedOrders.push_back(rec);
                }

                currentBlock = "";
                currentCustomer = "";
                currentOrderNum = "";
                currentTotal = 0.0f;
            }
        }
        file.close();

        if (filteredHistory.empty()) {
            if (!targetCustomer.empty()) {
                filteredHistory = "No purchase transcripts recorded for customer: " + targetCustomer + "\\nPlace an order from the catalog to generate your first bill & receipt!";
            } else {
                filteredHistory = "No history available.";
            }
        }

        stringstream ss;
        ss << "{\"status\": \"success\", ";
        ss << "\"customer\": \"" << targetCustomer << "\", ";
        ss << "\"count\": " << matchedOrders.size() << ", ";
        ss << "\"history\": \"";
        for (char c : filteredHistory) {
            if (c == '\n') ss << "\\n";
            else if (c == '"') ss << "\\\"";
            else if (c == '\r') continue;
            else ss << c;
        }
        ss << "\", \"orders\": [";
        for (size_t i = 0; i < matchedOrders.size(); ++i) {
            ss << "{\"orderNum\": \"" << matchedOrders[i].orderNum << "\", "
               << "\"customer\": \"" << matchedOrders[i].customer << "\", "
               << "\"total\": " << matchedOrders[i].total << "}";
            if (i + 1 < matchedOrders.size()) ss << ", ";
        }
        ss << "]}";

        res.set_content(ss.str(), "application/json");
    });

    // ==========================================
    // AUTHENTICATION API (Register & Login)
    // ==========================================
    
    // Customer Registration Endpoint
    svr.Post("/api/auth/register", [&](const httplib::Request& req, httplib::Response& res) {
        string body = req.body;
        auto extractStr = [&](const string& key) -> string {
            string search = "\"" + key + "\"";
            size_t pos = body.find(search);
            if (pos == string::npos) return "";
            size_t colon = body.find(":", pos + search.length());
            if (colon == string::npos) return "";
            size_t firstQuote = body.find("\"", colon + 1);
            if (firstQuote == string::npos) return "";
            size_t secondQuote = body.find("\"", firstQuote + 1);
            if (secondQuote == string::npos) return "";
            return body.substr(firstQuote + 1, secondQuote - firstQuote - 1);
        };

        string name = extractStr("name");
        string username = extractStr("username");
        string email = extractStr("email");
        string password = extractStr("password");

        if (name.empty()) name = username;
        if (username.empty() || password.empty()) {
            res.status = 400;
            res.set_content("{\"status\": \"error\", \"message\": \"Username and password are required.\"}", "application/json");
            return;
        }

        vector<UserAccount> users = loadUsers();
        for (const auto& u : users) {
            if (u.username == username) {
                res.status = 400;
                res.set_content("{\"status\": \"error\", \"message\": \"Username already taken! Please choose another.\"}", "application/json");
                return;
            }
            if (!email.empty() && u.email == email) {
                res.status = 400;
                res.set_content("{\"status\": \"error\", \"message\": \"Email address is already registered!\"}", "application/json");
                return;
            }
        }

        UserAccount newAcc = {name, username, email, password, "customer"};
        if (saveNewUser(newAcc)) {
            res.set_content("{\"status\": \"success\", \"role\": \"customer\", \"name\": \"" + name + "\", \"username\": \"" + username + "\", \"token\": \"novacart-customer-session\", \"message\": \"Account registered successfully!\"}", "application/json");
        } else {
            res.status = 500;
            res.set_content("{\"status\": \"error\", \"message\": \"Server error saving user account.\"}", "application/json");
        }
    });

    // Customer & Admin Login Endpoint
    svr.Post("/api/auth/login", [&](const httplib::Request& req, httplib::Response& res) {
        string body = req.body;
        string user = "";
        string pass = "";
        string role = "customer";

        auto extractStr = [&](const string& key) -> string {
            string search = "\"" + key + "\"";
            size_t pos = body.find(search);
            if (pos == string::npos) return "";
            size_t colon = body.find(":", pos + search.length());
            if (colon == string::npos) return "";
            size_t firstQuote = body.find("\"", colon + 1);
            if (firstQuote == string::npos) return "";
            size_t secondQuote = body.find("\"", firstQuote + 1);
            if (secondQuote == string::npos) return "";
            return body.substr(firstQuote + 1, secondQuote - firstQuote - 1);
        };

        string u = extractStr("username");
        string p = extractStr("password");
        string r = extractStr("role");

        if (!u.empty()) user = u;
        if (!p.empty()) pass = p;
        if (!r.empty()) role = r;

        if (role == "admin" || user == "admin") {
            if (pass == "admin123" || pass == "admin") {
                res.set_content("{\"status\": \"success\", \"role\": \"admin\", \"token\": \"novacart-admin-key-2026\", \"name\": \"Administrator\", \"username\": \"admin\"}", "application/json");
            } else {
                res.status = 401;
                res.set_content("{\"status\": \"error\", \"message\": \"Invalid administrator credentials! Password is 'admin123'\"}", "application/json");
            }
        } else {
            // Customer Login: verify against registered users
            vector<UserAccount> users = loadUsers();
            string displayName = user;
            for (const auto& acc : users) {
                if (acc.username == user || acc.email == user || acc.name == user) {
                    displayName = acc.name;
                    if (!acc.password.empty() && !pass.empty() && acc.password != pass) {
                        res.status = 401;
                        res.set_content("{\"status\": \"error\", \"message\": \"Incorrect password for user account!\"}", "application/json");
                        return;
                    }
                    break;
                }
            }
            if (displayName.empty()) displayName = "Customer";
            res.set_content("{\"status\": \"success\", \"role\": \"customer\", \"token\": \"novacart-customer-session\", \"name\": \"" + displayName + "\", \"username\": \"" + user + "\"}", "application/json");
        }
    });

    // ==========================================
    // ADMIN API ENDPOINTS (Protected by Middleware)
    // ==========================================
    
    // Admin login
    svr.Post("/api/admin/login", [&](const httplib::Request& req, httplib::Response& res) {
        string body = req.body;
        string pass = "";
        size_t pos = body.find("\"password\":\"");
        if (pos != string::npos) {
            size_t start = pos + 12;
            size_t end = body.find("\"", start);
            if (end != string::npos) pass = body.substr(start, end - start);
        }

        if (pass == "admin123" || pass == "admin") {
            res.set_content("{\"status\": \"success\", \"token\": \"novacart-admin-key-2026\", \"role\": \"SuperAdmin\"}", "application/json");
        } else {
            res.status = 401;
            res.set_content("{\"status\": \"error\", \"message\": \"Invalid admin password! Try 'admin123'\"}", "application/json");
        }
    });

    // Admin get orders list
    svr.Get("/api/admin/orders", [&](const httplib::Request& req, httplib::Response& res) {
        vector<OrderRecord> orders = parseReceipts();
        stringstream ss;
        ss << "[";
        for (size_t i = 0; i < orders.size(); i++) {
            ss << "{\"orderNum\":\"" << orders[i].orderNum 
               << "\",\"customer\":\"" << orders[i].customer 
               << "\",\"total\":" << fixed << setprecision(2) << orders[i].total << "}";
            if (i + 1 < orders.size()) ss << ",";
        }
        ss << "]";
        res.set_content(ss.str(), "application/json");
    });

    // Admin get users / customers aggregation (both registered accounts and order history)
    svr.Get("/api/admin/users", [&](const httplib::Request& req, httplib::Response& res) {
        vector<OrderRecord> orders = parseReceipts();
        map<string, pair<int, float>> userStats;
        
        // Seed with registered customers
        vector<UserAccount> registered = loadUsers();
        for (const auto& u : registered) {
            if (u.role != "admin") {
                userStats[u.name] = {0, 0.0f};
            }
        }

        // Aggregate actual orders from receipts.txt
        for (const auto& ord : orders) {
            if (ord.customer.empty()) continue;
            userStats[ord.customer].first += 1;
            userStats[ord.customer].second += ord.total;
        }

        stringstream ss;
        ss << "[";
        size_t idx = 0;
        for (const auto& pair : userStats) {
            ss << "{\"name\":\"" << pair.first 
               << "\",\"orderCount\":" << pair.second.first 
               << ",\"totalSpent\":" << fixed << setprecision(2) << pair.second.second << "}";
            if (++idx < userStats.size()) ss << ",";
        }
        ss << "]";
        res.set_content(ss.str(), "application/json");
    });

    // Admin get analytics summary
    svr.Get("/api/admin/analytics", [&](const httplib::Request& req, httplib::Response& res) {
        vector<OrderRecord> orders = parseReceipts();
        map<string, int> users;
        float grossRevenue = 0.0f;
        for (const auto& ord : orders) {
            grossRevenue += ord.total;
            if (!ord.customer.empty()) users[ord.customer]++;
        }

        stringstream ss;
        ss << "{\"totalOrders\":" << orders.size() 
           << ",\"grossRevenue\":" << fixed << setprecision(2) << grossRevenue 
           << ",\"totalCustomers\":" << users.size() 
           << ",\"totalStock\":" << myStore.getTotalStock() << "}";
        res.set_content(ss.str(), "application/json");
    });

    // Admin restock product
    svr.Post("/api/admin/stock", [&](const httplib::Request& req, httplib::Response& res) {
        string body = req.body;
        int pId = -1;
        int qty = 0;
        
        size_t pidPos = body.find("\"pId\":");
        if (pidPos != string::npos) pId = stoi(body.substr(pidPos + 6));
        
        size_t qtyPos = body.find("\"qty\":");
        if (qtyPos != string::npos) qty = stoi(body.substr(qtyPos + 6));

        if (myStore.restockProduct(pId, qty)) {
            res.set_content("{\"status\": \"success\", \"message\": \"Stock updated successfully!\"}", "application/json");
        } else {
            res.status = 400;
            res.set_content("{\"status\": \"error\", \"message\": \"Failed to restock. Invalid Product ID or quantity.\"}", "application/json");
        }
    });

    // ==========================================
    // CATEGORY MANAGEMENT ENDPOINTS
    // ==========================================
    // Public & Admin: Fetch all registered categories
    svr.Get("/api/categories", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_content(myStore.getCategoriesJson(), "application/json");
    });

    // Admin: Add a new product category
    svr.Post("/api/admin/categories/add", [&](const httplib::Request& req, httplib::Response& res) {
        string key = jsonExtractStr(req.body, "key");
        string label = jsonExtractStr(req.body, "label");

        auto trimStr = [](string s) -> string {
            while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r')) s.pop_back();
            size_t start = 0;
            while (start < s.length() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r')) start++;
            return s.substr(start);
        };
        key = trimStr(key);
        label = trimStr(label);
        for (char& c : key) c = tolower(c);
        for (size_t i = 0; i < key.length(); i++) {
            if (key[i] == ' ' || key[i] == '_') key[i] = '-';
        }

        if (key.empty()) {
            res.status = 400;
            res.set_content("{\"status\": \"error\", \"message\": \"Category key cannot be empty.\"}", "application/json");
            return;
        }

        if (label.empty()) label = key;

        if (myStore.addCategory(key, label)) {
            res.set_content("{\"status\": \"success\", \"message\": \"Category added successfully!\", \"categories\": " + myStore.getCategoriesJson() + "}", "application/json");
        } else {
            res.status = 400;
            res.set_content("{\"status\": \"error\", \"message\": \"Category key already exists or is invalid.\"}", "application/json");
        }
    });

    // ==========================================
    // PRODUCT CRUD ENDPOINTS (Admin Only)
    // ==========================================
    // Admin: Add a new product
    svr.Post("/api/admin/products/add", [&](const httplib::Request& req, httplib::Response& res) {
        string title = jsonExtractStr(req.body, "title");
        string brand = jsonExtractStr(req.body, "brand");
        float price = jsonExtractFloat(req.body, "price");
        int stock = jsonExtractInt(req.body, "stock");
        string category = jsonExtractStr(req.body, "category");
        string extra = jsonExtractStr(req.body, "extra");

        if (title.empty() || brand.empty() || price <= 0) {
            res.status = 400;
            res.set_content("{\"status\": \"error\", \"message\": \"Product title, brand, and a valid price (> 0) are required.\"}", "application/json");
            return;
        }

        if (category.empty()) category = "general";
        if (stock < 0) stock = 0;

        Product* p = myStore.addProduct(title, brand, price, stock, category, extra);
        if (p != nullptr) {
            res.set_content("{\"status\": \"success\", \"message\": \"Product added successfully!\", \"product\": " + p->toJson() + "}", "application/json");
        } else {
            res.status = 500;
            res.set_content("{\"status\": \"error\", \"message\": \"Failed to create product in inventory.\"}", "application/json");
        }
    });

    // Admin: Update existing product
    svr.Post("/api/admin/products/update", [&](const httplib::Request& req, httplib::Response& res) {
        int id = jsonExtractInt(req.body, "id");
        string title = jsonExtractStr(req.body, "title");
        string brand = jsonExtractStr(req.body, "brand");
        float price = jsonExtractFloat(req.body, "price");
        int stock = jsonExtractInt(req.body, "stock");
        string category = jsonExtractStr(req.body, "category");
        string extra = jsonExtractStr(req.body, "extra");

        if (id <= 0 || title.empty() || brand.empty() || price <= 0) {
            res.status = 400;
            res.set_content("{\"status\": \"error\", \"message\": \"Valid product ID, title, brand, and price are required.\"}", "application/json");
            return;
        }

        if (category.empty()) category = "general";
        if (stock < 0) stock = 0;

        if (myStore.updateProduct(id, title, brand, price, stock, category, extra)) {
            res.set_content("{\"status\": \"success\", \"message\": \"Product updated successfully!\"}", "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"status\": \"error\", \"message\": \"Product not found with ID " + to_string(id) + "\"}", "application/json");
        }
    });

    // Admin: Delete product
    svr.Post("/api/admin/products/delete", [&](const httplib::Request& req, httplib::Response& res) {
        int id = jsonExtractInt(req.body, "id");
        if (id <= 0) {
            res.status = 400;
            res.set_content("{\"status\": \"error\", \"message\": \"Valid product ID is required.\"}", "application/json");
            return;
        }

        if (myStore.deleteProduct(id)) {
            res.set_content("{\"status\": \"success\", \"message\": \"Product removed from catalog successfully!\"}", "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"status\": \"error\", \"message\": \"Product with ID " + to_string(id) + " not found.\"}", "application/json");
        }
    });

    cout << "Starting server on http://localhost:8080..." << endl;
    if (!svr.listen("0.0.0.0", 8080)) {
        cout << "Failed to start server. Port might be in use." << endl;
    }
    return 0;
}