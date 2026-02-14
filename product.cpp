#include "Product.h"

void Product::setName(const string& n) {
    if (n.empty()) throw "Имя пустое";
    name = n;
}

Product::Product() : name("Товар"), price(0), exp(0), place("Склад") {
    comp.push_back("основа");
}

Product::Product(string n, double p, int e, vector<string> c, string pl)
    : exp(e), place(pl) {
    setName(n);
    setPrice(p);
    comp = c;
}

Product::Product(const Product& other)
    : name(other.name), price(other.price), exp(other.exp),
    place(other.place), comp(other.comp) {
}

Product& Product::operator=(const Product& other) {
    if (this != &other) {
        comp.clear();
        name = other.name;
        price = other.price;
        exp = other.exp;
        place = other.place;
        comp = other.comp;
    }
    return *this;
}

Product::~Product() {
    cout << "Удален: " << name << endl;
    comp.clear();
}

void Product::setPrice(double p) {
    if (p < 0) throw "Цена < 0";
    price = p;
}

void Product::show() const {
    cout << "\n" << name << ": " << price << "руб, " << exp << "дн, Состав: ";
    for (size_t i = 0; i < comp.size(); ++i) {
        cout << comp[i];
        if (i < comp.size() - 1) cout << ", ";
    }
    cout << endl;
}

void Product::reduceExp(int days) {
    if (days < 0) throw "Дни < 0";
    exp = max(0, exp - days);
}

Product Product::operator+(const Product& other) const {
    vector<string> newComp = comp;
    for (const string& item : other.comp) {
        if (find(newComp.begin(), newComp.end(), item) == newComp.end()) {
            newComp.push_back(item);
        }
    }
    return Product("смесь " + name + " и " + other.name,
        (price + other.price) * 0.9,
        min(exp, other.exp),
        newComp, place);
}

Product Product::operator-(const Product& other) const {
    vector<string> newComp;
    for (const string& item : comp) {
        if (find(other.comp.begin(), other.comp.end(), item) == other.comp.end()) {
            newComp.push_back(item);
        }
    }
    newComp.push_back("консервант Т1000");
    return Product("выбор " + name + " без " + other.name,
        price * 0.9,
        max(0, exp - 2),
        newComp, place);
}

ostream& operator<<(ostream& os, const Product& p) {
    os << "Product{" << p.name << "}";
    return os;
}