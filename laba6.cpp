#include <iostream>
#include <stdlib.h>
using namespace std;
void printMat(int** mat, int r, int c) {                     // создаем матрицу
    cout << "Матрица " << r << "x" << c << endl;
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++)
            cout << mat[i][j] << "   ";
        cout << endl;
    }
}

int* findZeroCol(int** mat, int r, int c, int& count) {     // ищем индексы столбцов в которых есть хотя бы один ноль 
    count = 0;
    int* zeroC = (int*)calloc(c, sizeof(int));
    if (!zeroC) return nullptr;

    for (int j = 0; j < c; j++) {
        for (int i = 0; i < r; i++) {
            if (mat[i][j] == 0) {
                zeroC[count] = j;
                count += 1;
                break;
            }
        }
    }

    if (count > 0) {                                        // Если нашли хотя бы один столбец с нулем, перераспределяем память под массив zeroC, чтобы он был размером count
        zeroC = (int*)realloc(zeroC, count * sizeof(int));
        cout << "Столбцы для удаления: [ ";
        for (int i = 0; i < count; i++) cout << zeroC[i] << " ";
        cout << "]" << endl;
    }
    return zeroC;
}

void removeCol(int** mat, int r, int& c, int* col, int count) {    // удаляем из матрицы столбцы
    if ((count == 0) || (col == nullptr)) return;

    if (count == c) {
        for (int i = 0; i < r; i++) {
            free(mat[i]);
            mat[i] = nullptr;
        }
        c = 0;
        return;
    }

    for (int i = 0; i < r; i++) {
        int newJ = 0;
        for (int j = 0; j < c; j++) {
            bool save = true;
            for (int k = 0; k < count; k++)
                if (j == col[k]) {
                    save = false;
                    break;
                }
            if (save == true) {
                mat[i][newJ] = mat[i][j];
                newJ++;
            }
        }

        int* newR = (int*)realloc(mat[i], (c - count) * sizeof(int));
        if (newR)
            mat[i] = newR;
    }
    c -= count;
}

void p2() {                                 // работа с указателями
    float* a = new float;
    float* b = new float;
    cout << "Введите a: "; cin >> *a;
    cout << "Введите b: "; cin >> *b;
    float* pa = a;
    float* pb = b;
    cout << "До: a= " << *pa << " b= " << *pb << endl;
    *pa = *pa * 3;
    float temp = *pa;
    *pa = *pb;
    *pb = temp;
    cout << "После: a=" << *pa << " b=" << *pb << endl;
    delete a;
    delete b;
}
int main() {
    setlocale(LC_ALL, "Rus");
    int A, B, C, D;

    do {
        cout << "Введите A: ";  cin >> A;
        cout << "Введите B: ";  cin >> B;
        if ((A < 0) || (B < 0)) {
            cout << "Ошибка, A и B должны быть неотрицательные" << endl;
        }
    } while ((A < 0) || (B < 0));

    cout << "Введите C: "; cin >> C;
    cout << "Введите D: "; cin >> D;

    int** mat = (int**)malloc(2 * sizeof(int*));
    if (!mat) return 1;

    for (int i = 0; i < 2; i++) {
        mat[i] = (int*)malloc(2 * sizeof(int));
        if (!mat[i]) {
            for (int j = 0; j < i; j++)
                free(mat[j]);
            free(mat);
            return 2;
        }
    }

    mat[0][0] = A;
    mat[0][1] = B;
    mat[1][0] = C;
    mat[1][1] = D;

    printMat(mat, 2, 2);

    int r = 2 + A;
    int c = 2 + B;

    mat = (int**)realloc(mat, r * sizeof(int*));
    if (!mat) return 3;

    for (int i = 0; i < r; i++) {
        if (i < 2) {
            int* newR = (int*)realloc(mat[i], c * sizeof(int));
            if (!newR) {
                for (int j = 0; j < i; j++) free(mat[j]);
                free(mat);
                return 4;
            }
            mat[i] = newR;
        }
        else {
            mat[i] = (int*)malloc(c * sizeof(int));
            if (!mat[i]) {
                for (int j = 0; j < i; j++) free(mat[j]);
                free(mat);
                return 5;
            }
        }
    }
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            if (i >= 2 || j >= 2) {
                mat[i][j] = (i - 1) * C + (j - 1) * D;
            }
        }
    }
    printMat(mat, r, c);
    int count = 0;
    int* zeroC = findZeroCol(mat, r, c, count);
    if ((zeroC != nullptr) && (count > 0)) {
        removeCol(mat, r, c, zeroC, count);
        printMat(mat, r, c);
        free(zeroC);
    }
    else {
        cout << "Удаление не требуется." << endl;
    }

    for (int i = 0; i < r; i++) {
        free(mat[i]);
    }
    free(mat);

    p2();
    return 0;
}