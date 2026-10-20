#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>
#include "httplib.h"

using namespace std;

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

public:
    Product() {
        id = 0;
        title = "";
        brand = "";
        price = 0.0;
        stock = 0;
    }

    Product(int i, string t, string b, float p, int s) {
        id = i;
        title = t;
        brand = b;
        price = p;
        stock = s;
    }

    int getId() { return id; }
    string getTitle() { return title; }
    string getBrand() { return brand; }
    float getPrice() { return price; }
    int getStock() { return stock; }

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

    // Virtual Function (Runtime Polymorphism)
    virtual void showInfo() {
        cout << id << "\t" << title << "\t" << brand << "\tRs. " << price << "\tQty: " << stock;
    }

    virtual string toJson() {
        stringstream ss;
        ss << "{\"id\": " << id 
           << ", \"title\": \"" << title 
           << "\", \"brand\": \"" << brand 
           << "\", \"price\": " << price 
           << ", \"stock\": " << stock 
           << ", \"type\": \"product\"}";
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
        : Product(i, t, b, p, s) {
        warrantyYear = w;
    }

    // Function overriding
    void showInfo() {
        Product::showInfo();
        cout << "\t[Warranty: " << warrantyYear << " Year]" << endl;
    }

    string toJson() override {
        stringstream ss;
        ss << "{\"id\": " << id 
           << ", \"title\": \"" << title 
           << "\", \"brand\": \"" << brand 
           << "\", \"price\": " << price 
           << ", \"stock\": " << stock 
           << ", \"warrantyYear\": " << warrantyYear 
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
        : Product(i, t, b, p, s) {
        size = sz;
    }

    // Function overriding
    void showInfo() {
        Product::showInfo();
        cout << "\t[Size: " << size << "]" << endl;
    }

    string toJson() override {
        stringstream ss;
        ss << "{\"id\": " << id 
           << ", \"title\": \"" << title 
           << "\", \"brand\": \"" << brand 
           << "\", \"price\": " << price 
           << ", \"stock\": " << stock 
           << ", \"size\": \"" << size 
           << "\", \"type\": \"clothing\"}";
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
// 5. MAIN PLATFORM / SHOPPING MALL
// ==========================================
class ShoppingMall {
private:
    Product* inventory[6];
    static int orderCounter; // Static data member

public:
    ShoppingMall() {
        // Pre-loaded items
        inventory[0] = new Electronics(1, "iPhone 15", "Apple", 270000, 5, 1);
        inventory[1] = new Electronics(2, "S23 Ultra", "Samsung", 240000, 4, 2);
        inventory[2] = new Electronics(3, "Headphones", "Sony", 45000, 8, 1);
        inventory[3] = new Clothing(4, "Hoodie", "Nike", 6500, 15, "L");
        inventory[4] = new Clothing(5, "Tracksuit", "Adidas", 8500, 10, "M");
        inventory[5] = new Clothing(6, "Formal Shirt", "Outfitters", 4200, 12, "XL");
    }

    ~ShoppingMall() {
        for (int i = 0; i < 6; i++) {
            delete inventory[i];
        }
    }

    // Static function
    static int getOrders() {
        return orderCounter;
    }

    static void addOrder() {
        orderCounter++;
    }

    void displayAll() {
        cout << "\n---------------- AVAILABLE INVENTORY ----------------\n";
        cout << "ID\tItem\t\tBrand\t\tPrice\t\tStock\tDetails\n";
        cout << "-----------------------------------------------------\n";
        for (int i = 0; i < 6; i++) {
            // Polymorphism in action
            inventory[i]->showInfo();
        }
        cout << "-----------------------------------------------------\n";
    }

    Product* searchProduct(int id) {
        for (int i = 0; i < 6; i++) {
            if (inventory[i]->getId() == id) {
                return inventory[i];
            }
        }
        return NULL;
    }

    string getProductsJson() {
        stringstream ss;
        ss << "[";
        for (int i = 0; i < 6; i++) {
            ss << inventory[i]->toJson();
            if (i < 5) ss << ",";
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
        if (p != NULL && qty > 0) {
            p->restock(qty);
            return true;
        }
        return false;
    }

    int getTotalStock() {
        int total = 0;
        for (int i = 0; i < 6; i++) {
            if (inventory[i] != NULL) {
                total += inventory[i]->getStock();
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

    // API to get history
    svr.Get("/api/history", [&](const httplib::Request& req, httplib::Response& res) {
        ifstream file("receipts.txt");
        string history = "No history available.";
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            history = buffer.str();
            file.close();
        }
        
        stringstream ss;
        ss << "{\"history\": \"";
        for (char c : history) {
            if (c == '\n') ss << "\\n";
            else if (c == '"') ss << "\\\"";
            else ss << c;
        }
        ss << "\"}";
        
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

    cout << "Starting server on http://localhost:8080..." << endl;
    if (!svr.listen("0.0.0.0", 8080)) {
        cout << "Failed to start server. Port might be in use." << endl;
    }
    return 0;
}