#ifndef PRODUCT_H
#define PRODUCT_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

class Product {
private:
    string name;
    double price;
    vector<string> comp;
    void setName(const string& n);
protected:
    int exp;
public:
    string place;

    Product();
    Product(string n, double p, int e, vector<string> c, string pl = "Склад");
    Product(const Product& other);
    Product& operator=(const Product& other);
    ~Product();

    string getName() const { return name; }
    double getPrice() const { return price; }
    int getExp() const { return exp; }

    void setPrice(double p);
    void show() const;
    void reduceExp(int days);

    Product operator+(const Product& other) const;
    Product operator-(const Product& other) const;

    friend ostream& operator<<(ostream& os, const Product& p);
};

#endif
