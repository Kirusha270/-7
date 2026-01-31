#include <iostream>
#include <string>
#include <clocale>
using namespace std;

class Product {
private:
    string name;
    string desc;
    double price;

    void setName(const string& n) {       // сеттер для привата
        if (n.empty()) throw "Имя пустое";
        name = n;
    }
    void setDesc(const string& d) {
        desc = d;
    }

protected:
    int days;

public:
    string place;
    Product() : name("Товар"), desc("Описание"), price(0), days(0), place("Склад") {  // конструкуторы
        cout << "Создан товар по умолчанию" << endl;
    }
    Product(string n, string d, double p, int dg, string pl)
        : days(dg), place(pl) {
        setName(n);
        setDesc(d);
        setPrice(p);
        cout << "Создан товар " << name << endl;
    }
    Product(const Product& other)
        : name(other.name), desc(other.desc), price(other.price),
        days(other.days), place(other.place) {
        cout << "Скопирован товар " << name << endl;
    }
    ~Product() {                                  // деструктор
        cout << "Удален товар: " << name << endl;
    }

    string getName() const { return name; }
    string getDesc() const { return desc; }
    double getPrice() const { return price; }
    int getDays() const { return days; }
    string getPlace() const { return place; }

    void setPrice(double p) {
        if (p < 0) throw "Цена < 0";
        price = p;
    }
    void setDays(int d) {
        if (d < 0) throw "Срок < 0";
        days = d;
    }
    void showAll() const {    // методы
        cout << "\nТовар: " << name
            << "\nОписание: " << desc
            << "\nЦена: " << price << " руб."
            << "\nСрок: " << days << " дней"
            << "\nМесто: " << place << endl;
    }
    void showPlace() const {
        cout << name << " находится в " << place << endl;
    }
    void reduceDays(int n) {
        if (n < 0) throw "Дни < 0";
        days -= n;
        if (days < 0) days = 0;
    }
};
int main() {
    setlocale(LC_ALL, "RU");

    Product p1;
    Product p2("Молоко", "2.5%", 85, 7, "Холодильник");

    cout << "Имя: " << p2.getName() << endl;
    cout << "Цена: " << p2.getPrice() << endl;
    cout << "Срок: " << p2.getDays() << endl;
    cout << "Место: " << p2.getPlace() << endl;

    p2.showAll();
    p2.showPlace();

    p2.setPrice(90);
    p2.reduceDays(3);

    cout << "\nПосле изменений:" << endl;
    cout << "Цена: " << p2.getPrice() << endl;
    cout << "Срок: " << p2.getDays() << endl;

    return 0;
}