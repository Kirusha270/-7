#pragma execution_character_set("utf-8")

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

using namespace std;

uint32_t decodeUtf8(const string& s, size_t i, size_t& len)
{
    unsigned char c = s[i];

    if (c < 0x80) { len = 1; return c; }
    if ((c & 0xE0) == 0xC0)
    {
        len = 2;
        return ((c & 0x1F) << 6) | (s[i + 1] & 0x3F);
    }
    if ((c & 0xF0) == 0xE0)
    {
        len = 3;
        return ((c & 0x0F) << 12) |
               ((s[i + 1] & 0x3F) << 6) |
               (s[i + 2] & 0x3F);
    }

    len = 4;
    return ((c & 0x07) << 18) |
           ((s[i + 1] & 0x3F) << 12) |
           ((s[i + 2] & 0x3F) << 6) |
           (s[i + 3] & 0x3F);
}

string encodeUtf8(uint32_t c)
{
    string r;

    if (c < 0x80)
        r += char(c);
    else if (c < 0x800)
    {
        r += char(0xC0 | (c >> 6));
        r += char(0x80 | (c & 0x3F));
    }
    else if (c < 0x10000)
    {
        r += char(0xE0 | (c >> 12));
        r += char(0x80 | ((c >> 6) & 0x3F));
        r += char(0x80 | (c & 0x3F));
    }
    else
    {
        r += char(0xF0 | (c >> 18));
        r += char(0x80 | ((c >> 12) & 0x3F));
        r += char(0x80 | ((c >> 6) & 0x3F));
        r += char(0x80 | (c & 0x3F));
    }

    return r;
}

vector<uint32_t> splitUtf8(const string& s)
{
    vector<uint32_t> r;

    for (size_t i = 0; i < s.size();)
    {
        size_t len;
        r.push_back(decodeUtf8(s, i, len));
        i += len;
    }

    return r;
}

class JaggedArray
{
    vector<vector<string>> a;

public:
    vector<string>& operator[](int i)
    {
        return a[i];
    }

    int size() const
    {
        return a.size();
    }

    void addLine(const vector<string>& row)
    {
        a.push_back(row);
    }

    void deleteElement(int i, int j)
    {
        if (i >= 0 && i < size() &&
            j >= 0 && j < a[i].size())
            a[i].erase(a[i].begin() + j);
    }

    void deleteElement(const string& item)
    {
        for (auto& row : a)
            row.erase(remove(row.begin(), row.end(), item), row.end());
    }

    void add_endline(int k, const string& item)
    {
        if (k >= 0 && k < size())
            a[k].push_back(item);
    }

    void sortLines()
    {
        for (auto& row : a)
            sort(row.begin(), row.end());
    }

    JaggedArray operator-() const
    {
        JaggedArray r;

        for (auto row : a)
        {
            vector<string> temp;

            for (auto& x : row)
                if (find(temp.begin(), temp.end(), x) == temp.end())
                    temp.push_back(x);

            r.addLine(temp);
        }

        return r;
    }

    JaggedArray& operator--()
    {
        for (auto& row : a)
        {
            for (auto& x : row)
            {
                vector<uint32_t> chars = splitUtf8(x);
                string temp;

                for (auto c : chars)
                    if (c < '0' || c > '9')
                        temp += encodeUtf8(c);

                x = temp;
            }
        }

        return *this;
    }

    void print() const
    {
        const char* colors[] =
        {
            "\033[91m",
            "\033[92m",
            "\033[93m",
            "\033[94m",
            "\033[95m",
            "\033[96m"
        };

        for (int i = 0; i < size(); i++)
        {
            cout << colors[i % 6];
            cout << "Строка " << i << ": ";

            for (auto& x : a[i])
                cout << x << " ";

            cout << "\033[0m\n";
        }
    }
};

bool loadFile(const string& name, JaggedArray& a)
{
    ifstream file(name);

    if (!file)
        return false;

    string line;

    while (getline(file, line))
    {
        vector<string> row;
        string word;

        for (char c : line)
        {
            if (c == ' ')
            {
                if (!word.empty())
                {
                    row.push_back(word);
                    word.clear();
                }
            }
            else
                word += c;
        }

        if (!word.empty())
            row.push_back(word);

        if (!row.empty())
            a.addLine(row);
    }

    return true;
}

int main()
{
#ifdef _WIN32
    _setmode(_fileno(stdin), _O_U16TEXT);

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;

    if (GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    SetConsoleOutputCP(CP_UTF8);
#endif

    JaggedArray a;

    if (!loadFile("data.txt", a))
    {
        cout << "Ошибка открытия data.txt\n";
        return 1;
    }

    cout << "Исходный массив:\n";
    a.print();

    cout << "\nA[0][0]: " << a[0][0] << "\n";

    a.add_endline(0, "NEW");
    cout << "\nПосле add_endline:\n";
    a.print();

    a.deleteElement(0, 0);
    cout << "\nПосле delete(0, 0):\n";
    a.print();

    a.deleteElement("NEW");
    cout << "\nПосле delete(\"NEW\"):\n";
    a.print();

    a.sortLines();
    cout << "\nПосле сортировки:\n";
    a.print();

    a = -a;
    cout << "\nПосле оператора -:\n";
    a.print();

    --a;
    cout << "\nПосле оператора --:\n";
    a.print();

    return 0;
}
