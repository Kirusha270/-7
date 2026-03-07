#ifndef INGREDIENT_HPP
#define INGREDIENT_HPP

#include <string>
using namespace std;

struct Ingredient
{
    string name;
    double volume;

    Ingredient(string n = "", double v = 0)
    {
        name = n;
        volume = v;
    }
};

#endif