#ifndef MORSE_H
#define MORSE_H

#include <QChar>
#include <QString>
#include <QStringList>
#include <functional>
#include <map>
#include <vector>

using namespace std;

// Логика лабораторной №5 (вариант 3 - «радист в панике»), без GUI.
namespace Morse {

// Служебные токены: конец слова и конец строки.
// Обычный токен - код одной буквы, например "*--*".
extern const QString WORD;
extern const QString NL;
bool isSpecial(const QString &token);

// Таблицы на map
const map<QChar, QString> &table();        // буква -> код
const map<QString, QChar> &reverseTable(); // код -> буква

// Работа с текстом кода Морзе
vector<QString> parse(const QString &text);
QString format(const vector<QString> &tokens);

// 1. Кодирование. unknown - количество пропущенных неизвестных символов
vector<QString> encode(const QString &text, int &unknown);

// 2. Передача: «радист в панике». Не больше одной ошибки на букву,
// одно из '-' (кроме первого символа) превращается в '*'.
// Выбор букв и позиции ошибки - случайный.
// Возвращает число внесённых ошибок.
int applyPanic(vector<QString> &tokens);

// 3. Декодирование
QString decode(const vector<QString> &tokens);
bool hasErrors(const vector<QString> &tokens);

// 4. Восстановление
struct Candidate {
    QString code;
    QChar ch;
};

vector<Candidate> candidates(const QString &code, bool includeSelf);

// Функция выбора варианта пользователем.
// Возвращает индекс выбранного варианта или -1 (оставить как есть).
using Chooser = function<int(int letterNo, int wordNo, const QString &received,
                                  bool valid, const vector<Candidate> &variants)>;

// force = true: предлагать варианты и для корректных букв.
// log - сообщения о ходе восстановления.
void recover(vector<QString> &tokens, bool force, const Chooser &choose, QStringList &log);

} // namespace Morse

#endif // MORSE_H
