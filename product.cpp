#include "Product.h"

void Product::setName(const string& n) {
    if (n.empty()) throw "Имя пустое";
    name = n;
}

Product::Product() : name("товар"), price(0), exp(0), place("склад") {
    comp.push_back("вода");
}

Product::Product(string n, double p, int e, vector<string> c, string pl)
    : exp(e), place(pl) {
    setName(n);
    setPrice(p);
    comp = c;
}

Product::Product(const Product& other)
    : name(other.name), price(other.price),
    exp(other.exp), place(other.place),
    comp(other.comp) {
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
    comp.clear();
    cout << "Удаление: " << name << endl;
}

void Product::setPrice(double p) {
    if (p < 0) throw "Цена < 0";
    price = p;
}

void Product::show() const {
    cout << "\n" << name << ": " << price << " руб, "
        << exp << " дн, состав: ";

    for (size_t i = 0; i < comp.size(); i++) {
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

    vector<string> newComp;

    for (const auto& item : comp) {
        if (find(newComp.begin(), newComp.end(), item) == newComp.end())
            newComp.push_back(item);
    }

    for (const auto& item : other.comp) {
        if (find(newComp.begin(), newComp.end(), item) == newComp.end())
            newComp.push_back(item);
    }

    return Product(
        "ñìåñü " + name + " и " + other.name,
        (price + other.price) * 0.9,
        min(exp, other.exp),
        newComp,
        place
    );
}

Product& Product::operator-=(const Product& other) {

    for (auto it = comp.begin(); it != comp.end();) {

        if (find(other.comp.begin(), other.comp.end(), *it) != other.comp.end())
            it = comp.erase(it);
        else
            ++it;
    }

    comp.push_back("консервант Т1000");

    name = "выбор " + name + " без " + other.name;
    price = price * 0.9;
    exp = max(0, exp - 2);

    return *this;
}

ostream& operator<<(ostream& os, const Product& p) {
    os << "Product{" << p.name << "}";
    return os;

}
