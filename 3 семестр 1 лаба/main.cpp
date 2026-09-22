#pragma execution_character_set("utf-8")

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

using namespace std;

static uint32_t decodeUtf8(const string& s, size_t i, size_t& len) {
    unsigned char c = (unsigned char)s[i];
    if (c < 0x80) { len = 1; return c; }
    if ((c & 0xE0) == 0xC0) { len = 2; return ((c & 0x1Fu) << 6) | ((unsigned char)s[i + 1] & 0x3Fu); }
    if ((c & 0xF0) == 0xE0) { len = 3; return ((c & 0x0Fu) << 12) | (((unsigned char)s[i + 1] & 0x3Fu) << 6) | ((unsigned char)s[i + 2] & 0x3Fu); }
    if ((c & 0xF8) == 0xF0) { len = 4; return ((c & 0x07u) << 18) | (((unsigned char)s[i + 1] & 0x3Fu) << 12) | (((unsigned char)s[i + 2] & 0x3Fu) << 6) | ((unsigned char)s[i + 3] & 0x3Fu); }
    len = 1; return c;
}

static string encodeUtf8(uint32_t cp) {
    string r;
    if (cp < 0x80) r += (char)cp;
    else if (cp < 0x800) { r += (char)(0xC0 | (cp >> 6)); r += (char)(0x80 | (cp & 0x3F)); }
    else if (cp < 0x10000) { r += (char)(0xE0 | (cp >> 12)); r += (char)(0x80 | ((cp >> 6) & 0x3F)); r += (char)(0x80 | (cp & 0x3F)); }
    else { r += (char)(0xF0 | (cp >> 18)); r += (char)(0x80 | ((cp >> 12) & 0x3F)); r += (char)(0x80 | ((cp >> 6) & 0x3F)); r += (char)(0x80 | (cp & 0x3F)); }
    return r;
}

struct Char {
    uint32_t cp;
    string   utf8;
};

static vector<Char> splitUtf8(const string& s) {
    vector<Char> out;
    size_t i = 0;
    while (i < s.size()) {
        size_t len;
        uint32_t cp = decodeUtf8(s, i, len);
        out.push_back({ cp, s.substr(i, len) });
        i += len;
    }
    return out;
}

static string joinUtf8(const vector<Char>& v) {
    string r;
    for (const auto& c : v) r += c.utf8;
    return r;
}

#ifdef _WIN32
static string wideToUtf8(const wstring& w) {
    if (w.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(),
        nullptr, 0, nullptr, nullptr);
    string out(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(),
        &out[0], size, nullptr, nullptr);
    return out;
}
#endif

static string readLineUtf8() {
#ifdef _WIN32
    wstring wline;
    getline(wcin, wline);
    return wideToUtf8(wline);
#else
    string line;
    getline(cin, line);
    return line;
#endif
}

enum Cat { CAT_VOWEL = 0, CAT_CONSONANT = 1, CAT_DIGIT = 2, CAT_SIGN = 3 };

static uint32_t lowerCp(uint32_t cp) {
    if (cp >= 0x0410 && cp <= 0x042F) return cp + 0x20;  // А..Я -> а..я
    if (cp == 0x0401) return 0x0451;                     // Ё -> ё
    if (cp >= 'A' && cp <= 'Z') return cp + 0x20;
    return cp;
}

static int categoryOf(uint32_t cp) {
    uint32_t lc = lowerCp(cp);
    switch (lc) {
    case 0x0430: case 0x0435: case 0x0451: case 0x0438:
    case 0x043E: case 0x0443: case 0x044B: case 0x044D:
    case 0x044E: case 0x044F:
        return CAT_VOWEL;
    case 0x0431: case 0x0432: case 0x0433: case 0x0434:
    case 0x0436: case 0x0437: case 0x0439: case 0x043A:
    case 0x043B: case 0x043C: case 0x043D: case 0x043F:
    case 0x0440: case 0x0441: case 0x0442: case 0x0444:
    case 0x0445: case 0x0446: case 0x0447: case 0x0448:
    case 0x0449:
        return CAT_CONSONANT;
    }
    if (lc >= '0' && lc <= '9') return CAT_DIGIT;
    return CAT_SIGN;
}

static const char* categoryName(int cat) {
    switch (cat) {
    case CAT_VOWEL:     return "Гласные";
    case CAT_CONSONANT: return "Согласные";
    case CAT_DIGIT:     return "Цифры";
    default:            return "Знаки";
    }
}

static const char* colorFor(int cat) {
    switch (cat) {
    case CAT_VOWEL:     return "\033[91m";
    case CAT_CONSONANT: return "\033[94m";
    case CAT_DIGIT:     return "\033[92m";
    default:            return "\033[93m";
    }
}
static const char* kReset = "\033[0m";
static const char* kWhite = "\033[97m";

struct JaggedMalloc {
    char** rows = nullptr;
    int* lens = nullptr;
    int    n = 4;
};

class MainWindow {
public:
    MainWindow() { setupUi(); }
    ~MainWindow() = default;

    void run() {
        cout << "Введите строку (не более 50 символов, без латиницы):\n> ";
        cout.flush();
        m_input = readLineUtf8();
        trim(m_input);
        onProcessClicked();
    }

private:
    void setupUi() {
#ifdef _WIN32
        _setmode(_fileno(stdin), _O_U16TEXT);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD mode = 0;
            if (GetConsoleMode(hOut, &mode))
                SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
        SetConsoleOutputCP(CP_UTF8);
#endif
    }

    static void trim(string& s) {
        size_t a = 0, b = s.size();
        while (a < b && (unsigned char)s[a] <= ' ') ++a;
        while (b > a && (unsigned char)s[b - 1] <= ' ') --b;
        s = s.substr(a, b - a);
    }

    vector<vector<Char>> buildJaggedVector(const vector<Char>& chars) const {
        vector<vector<Char>>  rows(4);
        vector<set<uint32_t>> seen(4);
        for (const auto& ch : chars) {
            if (ch.cp == '@') continue;
            uint32_t lc = lowerCp(ch.cp);
            int cat = categoryOf(lc);
            if (seen[cat].count(lc)) continue;
            seen[cat].insert(lc);
            rows[cat].push_back({ lc, encodeUtf8(lc) });
        }
        return rows;
    }

    JaggedMalloc buildJaggedMalloc(const vector<Char>& chars) const {
        JaggedMalloc j;
        j.rows = (char**)malloc(4 * sizeof(char*));
        j.lens = (int*)calloc(4, sizeof(int));
        for (int i = 0; i < 4; ++i) {
            j.rows[i] = (char*)malloc(1);
            j.rows[i][0] = '\0';
        }
        vector<set<uint32_t>> seen(4);
        for (const auto& ch : chars) {
            if (ch.cp == '@') continue;
            uint32_t lc = lowerCp(ch.cp);
            int cat = categoryOf(lc);
            if (seen[cat].count(lc)) continue;
            seen[cat].insert(lc);

            const string cb = encodeUtf8(lc);
            int oldLen = j.lens[cat];
            j.rows[cat] = (char*)realloc(j.rows[cat], oldLen + cb.size() + 1);
            memcpy(j.rows[cat] + oldLen, cb.data(), cb.size());
            j.lens[cat] = oldLen + (int)cb.size();
            j.rows[cat][j.lens[cat]] = '\0';
        }
        return j;
    }

    void freeJaggedMalloc(JaggedMalloc& j) const {
        if (!j.rows) return;
        for (int i = 0; i < j.n; ++i) free(j.rows[i]);
        free(j.rows);
        free(j.lens);
        j.rows = nullptr;
        j.lens = nullptr;
    }

    bool isInJagged(const vector<vector<Char>>& rows, uint32_t cp) const {
        if (cp == '@') return false;
        uint32_t k = lowerCp(cp);
        for (const auto& row : rows)
            for (const auto& ch : row)
                if (lowerCp(ch.cp) == k) return true;
        return false;
    }

    string coloredAnsi(const vector<Char>& chars) const {
        string out;
        for (const auto& ch : chars) {
            if (isInJagged(m_rowsVec, ch.cp)) {
                out += colorFor(categoryOf(ch.cp));
                out += ch.utf8;
                out += kReset;
            }
            else {
                out += kWhite;
                out += ch.utf8;
                out += kReset;
            }
        }
        return out;
    }

    void onProcessClicked() {
        if (m_input.empty()) {
            cout << "Ошибка: строка не введена.\n";
            return;
        }

        auto chars = splitUtf8(m_input);
        if (chars.size() > 50) chars.resize(50);
        m_input = joinUtf8(chars);

        m_rowsVec = buildJaggedVector(chars);
        JaggedMalloc jm = buildJaggedMalloc(chars);

        m_result = chars;
        {
            string suffix = "+123";
            suffix += encodeUtf8(0x0410);
            suffix += encodeUtf8(0x0411);
            suffix += encodeUtf8(0x0412);
            auto more = splitUtf8(suffix);
            m_result.insert(m_result.end(), more.begin(), more.end());
        }

        cout << "\n";
        cout << "Входная строка: " << m_input << "\n\n";

        cout << "Зубчатый массив (std::vector):\n";
        for (int i = 0; i < 4; ++i) {
            cout << "  " << categoryName(i)
                << " [" << m_rowsVec[i].size() << "]: ";
            for (const auto& c : m_rowsVec[i]) cout << c.utf8 << ' ';
            cout << '\n';
        }

        cout << "\nЗубчатый массив (malloc/realloc):\n";
        for (int i = 0; i < 4; ++i) {
            string row = jm.rows[i] ? string(jm.rows[i]) : string();
            auto rowChars = splitUtf8(row);
            cout << "  " << categoryName(i)
                << " [" << rowChars.size() << "]: ";
            for (const auto& c : rowChars) cout << c.utf8 << ' ';
            cout << '\n';
        }

        cout << "\nРезультат (+\"+123АБВ\"):\n";
        cout << coloredAnsi(m_result) << "\n\n";

        cout << "Легенда: гласные — красные, согласные — голубые, "
            "цифры — зелёные, знаки — жёлтые; "
            "белым — символы, которых нет в зубчатом массиве.\n";

        cout << "\nНажмите Enter для выхода...";
        cout.flush();
        readLineUtf8();

        freeJaggedMalloc(jm);
    }

    string                 m_input;
    vector<Char>           m_result;
    vector<vector<Char>>   m_rowsVec;
};

int main() {
    MainWindow w;
    w.run();
    return 0;
}
