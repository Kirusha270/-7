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
const map<QChar, QString> &table()
{
    static const map<QChar, QString> t = {
        {u'А', "*-"},    {u'Б', "-***"},  {u'В', "*--"},   {u'Г', "--*"},
        {u'Д', "-**"},   {u'Е', "*"},     {u'Ж', "***-"},  {u'З', "--**"},
        {u'И', "**"},    {u'Й', "*---"},  {u'К', "-*-"},   {u'Л', "*-**"},
        {u'М', "--"},    {u'Н', "-*"},    {u'О', "---"},   {u'П', "*--*"},
        {u'Р', "*-*"},   {u'С', "***"},   {u'Т', "-"},     {u'У', "**-"},
        {u'Ф', "**-*"},  {u'Х', "****"},  {u'Ц', "-*-*"},  {u'Ч', "---*"},
        {u'Ш', "----"},  {u'Щ', "--*-"},  {u'Ы', "-*--"},  {u'Ь', "-**-"},
        {u'Э', "**-**"}, {u'Ю', "**--"},  {u'Я', "*-*-"}
    };
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
