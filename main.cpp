#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
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

// ==========================================
// 7. MAIN FUNCTION
// ==========================================
int main() {
    ShoppingMall myStore;
    httplib::Server svr;

    // Serve static files (HTML, CSS, JS) from the current directory
    svr.set_mount_point("/", "./");

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
                // Record the purchase using existing function
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

    // API to get history (just returns a simple string for now, could be enhanced to JSON)
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

    cout << "Starting server on http://localhost:8080..." << endl;
    if (!svr.listen("0.0.0.0", 8080)) {
        cout << "Failed to start server. Port might be in use." << endl;
    }
    return 0;
}