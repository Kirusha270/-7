#include "product3.h"

void Product::setName(const string& n)
{
    if (n.empty()) throw "Имя пустое";
    name = n;
}

Product::Product()
{
    name = "товар";
    description = "нет описания";
    price = 0;
    exp = 0;
    place = "склад";
}

Product::Product(string n, string d, double p, int e, vector<Ingredient> c, string pl)
{
    setName(n);
    description = d;
    setPrice(p);
    exp = e;
    comp = c;
    place = pl;
}

Product::Product(const Product& other)
{
    name = other.name;
    description = other.description;
    price = other.price;
    exp = other.exp;
    place = other.place;
    comp = other.comp;
}

Product& Product::operator=(const Product& other)
{
    if (this != &other)
    {
        comp.clear();

        name = other.name;
        description = other.description;
        price = other.price;
        exp = other.exp;
        place = other.place;

        comp = other.comp;
    }

    return *this;
}

Product::~Product()
{
    comp.clear();
}

void Product::setPrice(double p)
{
    if (p < 0)
        throw "Цена < 0";

    price = p;
}

void Product::show() const
{
    cout << "\nНазвание: " << name << endl;
    cout << "Описание: " << description << endl;
    cout << "Цена: " << price << endl;
    cout << "Срок годности: " << exp << endl;
    cout << "Место: " << place << endl;

    cout << "Состав: ";

    for (auto i : comp)
        cout << i.name << "(" << i.volume << ") ";

    cout << endl;
}

void Product::reduceExp(int days)
{
    if (days < 0)
        throw "Дни < 0";

    exp = max(0, exp - days);
}

Product Product::operator+(const Product& other) const
{
    vector<Ingredient> newComp;

    for (auto i : comp)
    {
        bool exists = false;

        for (auto j : newComp)
            if (j.name == i.name)
                exists = true;

        if (!exists)
            newComp.push_back(i);
    }

    for (auto i : other.comp)
    {
        bool exists = false;

        for (auto j : newComp)
            if (j.name == i.name)
                exists = true;

        if (!exists)
            newComp.push_back(i);
    }

    return Product(
        "Смесь " + name + " и " + other.name,
        description,
        (price + other.price) * 0.9,
        min(exp, other.exp),
        newComp,
        place
    );
}

Product Product::operator-(const Product& other) const
{
    vector<Ingredient> newComp;

    for (auto i : comp)
    {
        bool exists = false;

        for (auto j : other.comp)
            if (j.name == i.name)
                exists = true;

        if (!exists)
            newComp.push_back(i);
    }

    newComp.push_back({ "консервант Т1000",1 });

    return Product(
        "Выбор " + name,
        description,
        price * 0.9,
        max(0, exp - 2),
        newComp,
        place
    );
}

Product& Product::operator-=(const Product& other)
{
    for (auto it = comp.begin(); it != comp.end();)
    {
        bool remove = false;

        for (auto j : other.comp)
            if (j.name == it->name)
                remove = true;

        if (remove)
            it = comp.erase(it);
        else
            ++it;
    }

    comp.push_back({ "консервант Т1000",1 });

    price *= 0.9;
    exp = max(0, exp - 2);

    return *this;
}

void Product::showPlace() const
{
    cout << "Место получения: " << place << endl;
}