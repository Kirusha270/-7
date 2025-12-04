#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <ctime>

using namespace std;

// ------------------- Вывод vector -------------------

void printVector(const vector<int>& v) {    //  передаем по ссылке
    cout << "[ ";
    for (int x : v) cout << x << " ";   
    cout << "]";
}

// ------------------- Вывод array --------------------

void printArray(const array<int, 10>& a) {
    cout << "[ ";
    for (int x : a) cout << x << " ";
    cout << "]";
}

// ----------- разложение K = 1+2+3+... ----------

vector<int> expandK(int K) {   //  к как сумма нат чисел
    vector<int> res;           
    int sum = 0, i = 1;

    while (sum < K) {
        sum += i;
        res.push_back(i);
        i++;
    }

    if (sum == K) return res;
    return {};      // разложить нельзя
}

// ---------------- Пункт 2:----------------

// — По значению (копия)
void sortValue(array<int, 10> a) {    // копия входного массива
    sort(a.begin(), a.end());         // ее сортировка
    cout << "Сортировка по значению : ";
    printArray(a);
    cout << endl;
}

// — По ссылке
void sortRef(array<int, 10>& a) {     // сортировка настоящего массива
    sort(a.begin(), a.end());
}

// — По указателю
void sortPtr(array<int, 10>* a) {    //  адрес массива
    sort(a->begin(), a->end());      // обращение к реальному массиву
}

// --------------------------- MAIN ---------------------------

int main() {
    setlocale(LC_ALL, "ru");

    vector<int> arr = { 1, 3, 5 };
    int choice;

    do {
        cout << "\n========== МЕНЮ ==========\n"
            << "0. Выход\n"
            << "1. Просмотр массива (vector)\n"
            << "2. Добавить элемент в начало\n"
            << "3. Добавить элемент в конец\n"
            << "4. Очистить массив\n"
            << "5. Поиск элемента\n"
            << "6. разложение K\n"
            << "7. Пункт 2 (array + сортировки)\n"
            << "Ваш выбор: ";
        cin >> choice;
        cout << endl;

        // ---------- 1. Просмотр ----------
        if (choice == 1) {
            printVector(arr);
            cout << endl;
        }

        // ---------- 2. Добавление в начало ----------
        else if (choice == 2) {
            int x;
            cout << "Введите элемент: ";
            cin >> x;

            cout << "До: "; printVector(arr);
            arr.insert(arr.begin(), x);            // добавляем в начало
            cout << "\nПосле: "; printVector(arr);
            cout << endl;
        }

        // ---------- 3. Добавление в конец ----------
        else if (choice == 3) {
            int x;
            cout << "Введите элемент: ";
            cin >> x;

            cout << "До: "; printVector(arr);
            arr.push_back(x);
            cout << "\nПосле: "; printVector(arr);
            cout << endl;
        }

        // ---------- 4. Очистка ----------
        else if (choice == 4) {
            cout << "До: "; printVector(arr);
            arr.clear();
            cout << "\nПосле: "; printVector(arr);
            cout << endl;
        }

        // ---------- 5. Поиск ----------
        else if (choice == 5) {
            int x;
            cout << "Введите искомое число: ";
            cin >> x;

            vector<int> pos;
            for (int i = 0; i < arr.size(); i++)    // если нашли сохраняем индекс
                if (arr[i] == x) pos.push_back(i);

            cout << "Индексы: [ ";
            for (int p : pos) cout << p << " ";
            cout << "]\n";
        }

        // ---------- 6. Вариант 5 ----------
        else if (choice == 6) {
            int K;
            cout << "Введите K: ";
            cin >> K;

            cout << "До: "; printVector(arr);

            auto seq = expandK(K);     //  раскладываем к

            if (!seq.empty()) {
                cout << "\nK можно разложить: ";
                printVector(seq);
                cout << "\nДобавляем в конец...\n";

                for (int x : seq) arr.push_back(x); // добавляем в конец
            }
            else {
                cout << "\nРазложить нельзя — дублируем элементы на чётных позициях.\n";

                vector<int> newArr;
                newArr.reserve(arr.size() * 2);

                for (size_t i = 0; i < arr.size(); ++i) {
                    newArr.push_back(arr[i]);        // копируем оригинал
                    if (i % 2 == 1) {               
                        newArr.push_back(arr[i]);   // дублируем только эти
                    }
                }
                arr = move(newArr);
            }

            cout << "После: "; printVector(arr);
            cout << endl;
        }

        // ---------- 7.  пункт 2 ----------
        else if (choice == 7) {
            array<int, 10> a;

            for (int& x : a) x = rand() % 21 - 10;

            cout << "Исходный array: ";
            printArray(a);
            cout << "\n";

            //  передачи по значению
            sortValue(a);

            cout << "После sortValue (оригинал): ";
            printArray(a);
            cout << "\n";

            // по ссылке
            sortRef(a);
            cout << "После sortRef (оригинал отсортирован): ";
            printArray(a);
            cout << "\n";

            // перемешиваем
            for (int& x : a) x = rand() % 21 - 10;
            cout << "\nПеремешан снова: ";
            printArray(a);
            cout << "\n";

            // по указателю
            sortPtr(&a);
            cout << "После sortPtr (оригинал отсортирован): ";
            printArray(a);
            cout << "\n";
        }

    } while (choice != 0);

    return 0;
}