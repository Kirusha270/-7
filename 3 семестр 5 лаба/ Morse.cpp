#include "morse.h"

#include <QRandomGenerator>

namespace Morse {

const QString WORD = QStringLiteral("W");
const QString NL = QStringLiteral("N");

bool isSpecial(const QString &token)
{
    return token == WORD || token == NL;
}

// ---------- Таблицы (map) ----------
// Буквы заданы кодами Юникода (U+0410 = А ... U+042F = Я), чтобы таблица
// компилировалась при любой кодировке исходника.
const map<QChar, QString> &table()
{
    struct Row { char16_t letter; const char *code; };
    static const Row rows[] = {
        {0x0410, "*-"},    // A
        {0x0411, "-***"},  // B
        {0x0412, "*--"},   // V
        {0x0413, "--*"},   // G
        {0x0414, "-**"},   // D
        {0x0415, "*"},     // E
        {0x0416, "***-"},  // ZH
        {0x0417, "--**"},  // Z
        {0x0418, "**"},    // I
        {0x0419, "*---"},  // J
        {0x041A, "-*-"},   // K
        {0x041B, "*-**"},  // L
        {0x041C, "--"},    // M
        {0x041D, "-*"},    // N
        {0x041E, "---"},   // O
        {0x041F, "*--*"},  // P
        {0x0420, "*-*"},   // R
        {0x0421, "***"},   // S
        {0x0422, "-"},     // T
        {0x0423, "**-"},   // U
        {0x0424, "**-*"},  // F
        {0x0425, "****"},  // H
        {0x0426, "-*-*"},  // C
        {0x0427, "---*"},  // CH
        {0x0428, "----"},  // SH
        {0x0429, "--*-"},  // SCH
        {0x042B, "-*--"},  // Y
        {0x042C, "-**-"},  // soft sign
        {0x042D, "**-**"}, // E (reverse)
        {0x042E, "**--"},  // YU
        {0x042F, "*-*-"}   // YA
    };
    static const map<QChar, QString> t = [] {
        map<QChar, QString> m;
        for (const Row &r : rows)
            m[QChar(r.letter)] = QString::fromLatin1(r.code);
        return m;
    }();
    return t;
}

const map<QString, QChar> &reverseTable()
{
    static const map<QString, QChar> r = [] {
        map<QString, QChar> m;
        for (const auto &kv : table())
            m[kv.second] = kv.first;
        return m;
    }();
    return r;
}

// ---------- Парсинг / форматирование ----------
// Формат: сигналы буквы через пробел, буквы через " / ", слова через " // ".
// Пробелы игнорируются, поэтому "*--*/*" и "* - - * / *" читаются одинаково.
vector<QString> parse(const QString &text)
{
    vector<QString> tokens;
    const QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    for (int li = 0; li < lines.size(); ++li) {
        QString line = lines[li];
        line.remove(' ').remove('\t').remove('\r');
        if (line.isEmpty())
            continue;
        const QStringList segs = line.split('/'); // пустые части сохраняются
        for (int i = 0; i < segs.size(); ++i) {
            if (!segs[i].isEmpty())
                tokens.push_back(segs[i]);
            else if (i > 0 && i < segs.size() - 1 && !tokens.empty() && tokens.back() != WORD)
                tokens.push_back(WORD); // "//" = конец слова
        }
        if (!tokens.empty() && tokens.back() == WORD)
            tokens.pop_back();
        if (li + 1 < lines.size())
            tokens.push_back(NL);
    }
    return tokens;
}

QString format(const vector<QString> &tokens)
{
    QString res;
    bool prevLetter = false;
    for (const QString &t : tokens) {
        if (t == WORD) {
            res += QStringLiteral(" // ");
            prevLetter = false;
        } else if (t == NL) {
            res += '\n';
            prevLetter = false;
        } else {
            if (prevLetter)
                res += QStringLiteral(" / ");
            for (int i = 0; i < t.size(); ++i) {
                if (i)
                    res += ' ';
                res += t[i];
            }
            prevLetter = true;
        }
    }
    return res;
}

// ---------- 1. Кодирование ----------
vector<QString> encode(const QString &text, int &unknown)
{
    vector<QString> tokens;
    unknown = 0;
    for (QChar c : text.toUpper()) {
        if (c == QChar(0x0401)) // Ё -> Е
            c = QChar(0x0415);
        if (c == '\r')
            continue;
        if (c == '\n') {
            if (!tokens.empty() && tokens.back() == WORD)
                tokens.pop_back();
            if (!tokens.empty() && tokens.back() != NL)
                tokens.push_back(NL);
        } else if (c.isSpace()) {
            if (!tokens.empty() && !isSpecial(tokens.back()))
                tokens.push_back(WORD);
        } else {
            auto it = table().find(c);
            if (it != table().end())
                tokens.push_back(it->second);
            else
                ++unknown;
        }
    }
    while (!tokens.empty() && isSpecial(tokens.back()))
        tokens.pop_back();
    return tokens;
}

// ---------- 2. Нарушение: «радист в панике» ----------
int applyPanic(vector<QString> &tokens)
{
    int errors = 0;
    for (QString &t : tokens) {
        if (isSpecial(t))
            continue;
        QList<int> pos; // позиции тире, кроме первого символа
        for (int i = 1; i < t.size(); ++i)
            if (t[i] == '-')
                pos.append(i);
        if (pos.isEmpty())
            continue;
        if (QRandomGenerator::global()->bounded(2) == 0) // случайно: ошибка есть / нет
            continue;
        int p = pos[QRandomGenerator::global()->bounded(pos.size())];
        t[p] = QChar('*');
        ++errors;
    }
    return errors;
}

// ---------- 3. Декодирование ----------
QString decode(const vector<QString> &tokens)
{
    QString res;
    for (const QString &t : tokens) {
        if (t == WORD) {
            res += ' ';
        } else if (t == NL) {
            res += '\n';
        } else {
            auto it = reverseTable().find(t);
            res += (it != reverseTable().end()) ? it->second : QChar('?');
        }
    }
    return res;
}

bool hasErrors(const vector<QString> &tokens)
{
    for (const QString &t : tokens)
        if (!isSpecial(t) && !reverseTable().count(t))
            return true;
    return false;
}

// ---------- 4. Восстановление ----------
// Варианты: сама буква (если валидна) + буквы, получаемые заменой одной '*'
// (не на первой позиции) обратно на '-'.
vector<Candidate> candidates(const QString &code, bool includeSelf)
{
    vector<Candidate> r;
    auto it = reverseTable().find(code);
    if (includeSelf && it != reverseTable().end())
        r.push_back({code, it->second});
    for (int i = 1; i < code.size(); ++i) {
        if (code[i] != '*')
            continue;
        QString c = code;
        c[i] = QChar('-');
        auto j = reverseTable().find(c);
        if (j != reverseTable().end())
            r.push_back({c, j->second});
    }
    return r;
}

void recover(vector<QString> &tokens, bool force, const Chooser &choose, QStringList &log)
{
    int letterNo = 0, wordNo = 1;
    for (QString &t : tokens) {
        if (t == WORD) {
            ++wordNo;
            continue;
        }
        if (t == NL)
            continue;

        ++letterNo;
        const bool valid = reverseTable().count(t) > 0;
        if (valid && !force)
            continue;

        vector<Candidate> c = candidates(t, valid);
        if (!valid && c.empty()) {
            log << QStringLiteral("Буква №%1 (слово %2): «%3» восстановить не удалось")
                       .arg(letterNo).arg(wordNo).arg(t);
            continue;
        }
        if (c.empty())
            continue;
        if (c.size() == 1) {
            if (!valid)
                log << QStringLiteral("Буква №%1 (слово %2): «%3» -> %4 (единственный вариант)")
                           .arg(letterNo).arg(wordNo).arg(t, QString(c[0].ch));
            t = c[0].code;
            continue;
        }

        int idx = choose(letterNo, wordNo, t, valid, c);
        if (idx >= 0 && idx < static_cast<int>(c.size())) {
            log << QStringLiteral("Буква №%1 (слово %2): «%3» -> %4 (выбрано из %5 вариантов)")
                       .arg(letterNo).arg(wordNo).arg(t, QString(c[idx].ch)).arg(c.size());
            t = c[idx].code;
        }
    }
}

} // namespace Morse
