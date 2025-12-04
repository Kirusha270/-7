#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

struct Person {
    string name;
    int age;
    string bestFriend;   
};

void printPerson(const Person& p) {
    cout << "Имя: " << p.name
        << ", возраст: " << p.age
        << ", лучший друг: ";
    if (p.bestFriend == "") cout << "нет";
    else cout << p.bestFriend;
    cout << endl;
}

// найти человека по имени
int findByName(const vector<Person>& people, const string& name) {  // возвращает индекс человека в массиве
    for (int i = 0; i < people.size(); i++)
        if (people[i].name == name)
            return i;
    return -1;
}

// кто узнает секрет
void showChain(const vector<Person>& people, const string& start) {
    int idx = findByName(people, start);                           // проверка на существование
    if (idx == -1) {
        cout << "Человек не найден!\n";
        return;
    }

    cout << "Цепочка распространения секрета:\n";

    vector<string> visited; // запоминаем того кто прошел секрет
    string current = start; // текущий человек

    while (true) {
        cout << current << " ";   // имя
        visited.push_back(current);

        int i = findByName(people, current); // если нет тот завершаем
        if (i == -1) break;

        string bf = people[i].bestFriend;    
        if (bf == "" || find(visited.begin(), visited.end(), bf) != visited.end()) {
            break; // интроверт или цикл
        }
        current = bf;
    }
    cout << endl;
}

//  интроверты
void showIntroverts(const vector<Person>& people) {
    cout << "Интроверты :\n";
    for (auto& p : people)
        if (p.bestFriend == "")
            cout << p.name << endl;
}

// Пункт 3: взаимные пары друзей (A → B и B → A)
void showBestFriendPairs(const vector<Person>& people) {
    cout << "Very best friends (A → B и B → A):\n";
    for (int i = 0; i < people.size(); i++) {            // перебор пар
        for (int j = i + 1; j < people.size(); j++) {
            if (people[i].bestFriend == people[j].name &&
                people[j].bestFriend == people[i].name)
            {
                cout << people[i].name << " <--> " << people[j].name << endl;
            }
        }
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    system("chcp 1251 > nul");

    vector<Person> people = {
        {"Аня", 20, "Борис"},
        {"Борис", 22, "Аня"},
        {"Катя", 19, ""},
        {"Дима", 25, "Егор"},
        {"Егор", 24, ""},
        {"Федя", 21, "Дима"},
        {"Галя", 23, ""},
        {"Женя", 20, "Аня"}
    };

    int choice;

    do {
        cout << "\n=========== МЕНЮ ===========\n"
            << "1. Показать всех людей\n"
            << "2. Добавить нового человека\n"
            << "3. Показать цепочку распространения секрета\n"
            << "4. Показать всех интровертов\n"
            << "5. Показать пары 'very best friends'\n"
            << "0. Выход\n"
            << "Ваш выбор: ";

        cin >> choice;
        cout << endl;

        if (choice == 1) {
            for (auto& p : people) printPerson(p);
        }

        else if (choice == 2) {
            Person p;
            cout << "Введите имя: ";
            cin >> p.name;
            cout << "Введите возраст: ";
            cin >> p.age;
            cout << "Введите имя лучшего друга (нет): ";
            cin >> p.bestFriend;
            if (p.bestFriend == "-") p.bestFriend = "";

            people.push_back(p);
            cout << "Человек добавлен.\n";
        }

        else if (choice == 3) {
            string name;
            cout << "Введите имя человека, от которого пойдёт секрет: ";
            cin >> name;
            showChain(people, name);
        }

        else if (choice == 4) {
            showIntroverts(people);
        }

        else if (choice == 5) {
            showBestFriendPairs(people);
        }

    } while (choice != 0);

    return 0;
}