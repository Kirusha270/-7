#include "commission.h"
#include "product3.h"

Commission::Commission()
{
    add = "нет";
    checkDay = 0;
}

Commission::Commission(string a, vector<string> b, int d)
{
    add = a;
    banIn = b;
    checkDay = d;
}

Commission::Commission(const Commission& other)
{
    add = other.add;
    banIn = other.banIn;
    checkDay = other.checkDay;
}

Commission::~Commission()
{
    banIn.clear();
}

bool Commission::checkProduct(const Product& p) const
{
    if (p.getExp() <= 0)
        return false;

    auto comp = p.getIngredients();

    for (auto i : comp)
        for (auto b : banIn)
            if (i.name == b)
                return false;

    return true;
}

void Commission::show() const
{
    cout << "Адрес: " << add << endl;
}