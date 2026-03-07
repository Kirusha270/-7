#ifndef COMMISSION_HPP
#define COMMISSION_HPP

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Product;

class Commission
{
private:

    string add;
    vector<string> banIn;
    int checkDay;

public:

    Commission();
    Commission(string a, vector<string> b, int d);
    Commission(const Commission& other);
    ~Commission();

    bool checkProduct(const Product& p) const;

    void show() const;
};

#endif