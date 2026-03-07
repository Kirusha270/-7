#include <iostream>
#include <clocale>
#include "product3.h"
#include "commission.h"

using namespace std;

void review()
{
    cout << "\nОценка лабораторной:\n";
    cout << "Интерес: 8/10\n";
    cout << "Наполненность: 9/10\n";
    cout << "Сложность: 7/10\n";
}

int main()
{
    setlocale(LC_ALL, "RU");

    vector<Ingredient> c1 = {
        {"молоко",1},
        {"соль",0.1}
    };

    vector<Ingredient> c2 = {
        {"мука",1},
        {"вода",0.5}
    };

    Product p1("Сыр", "молочный", 500, 30, c1);
    Product p2("Хлеб", "выпечка", 50, 5, c2);

    p1.show();
    p2.show();

    Product mix = p1 + p2;
    mix.show();

    vector<string> b = { "яд" };

    Commission com("Москва", b, 5);

    if (com.checkProduct(mix))
        cout << "Разрешено\n";
    else
        cout << "Запрещено\n";

    review();

    return 0;
}