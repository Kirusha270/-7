#include <iostream>
#include <clocale>
#include "Product.h"

int main() {
    setlocale(LC_ALL, "RU");

    vector<string> c1 = { "молоко", "соль" };
    vector<string> c2 = { "мука", "вода", "соль" };

    Product p1("Сыр", 500, 30, c1);
    Product p2("Хлеб", 50, 5, c2);

    cout << "=== Исходные ===" << endl;
    p1.show();
    p2.show();

    cout << "\n=== p1 + p2 ===" << endl;
    (p1 + p2).show();

    cout << "\n=== p1 - p2 ===" << endl;
    (p1 - p2).show();

    return 0;
}