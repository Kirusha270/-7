#ifndef PRODUCT_H
#define PRODUCT_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "Ingredient.h"

using namespace std;

class Product
{
private:

    string name;
    string description;
    double price;
    vector<Ingredient> comp;

    void setName(const string& n);

protected:

    int exp;

public:

    string place;

    Product();
    Product(string n, string d, double p, int e, vector<Ingredient> c, string pl = "склад");
    Product(const Product& other);
    Product& operator=(const Product& other);
    ~Product();

    string getName() const { return name; }
    double getPrice() const { return price; }
    int getExp() const { return exp; }

    vector<Ingredient> getIngredients() const { return comp; }

    void setPrice(double p);

    void show() const;

    void reduceExp(int days);

    Product operator+(const Product& other) const;
    Product operator-(const Product& other) const;
    Product& operator-=(const Product& other);

    void showPlace() const;
};

#endif