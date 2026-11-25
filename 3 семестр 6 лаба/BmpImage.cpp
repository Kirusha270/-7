#include "BmpImage.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iterator>

namespace {
std::uint16_t rd16(const std::vector<std::uint8_t>& b, size_t p) {
    return static_cast<std::uint16_t>(b[p] | (b[p + 1] << 8));
}
std::uint32_t rd32(const std::vector<std::uint8_t>& b, size_t p) {
    return static_cast<std::uint32_t>(b[p]) | (static_cast<std::uint32_t>(b[p + 1]) << 8) |
           (static_cast<std::uint32_t>(b[p + 2]) << 16) | (static_cast<std::uint32_t>(b[p + 3]) << 24);
}
std::int32_t rd32s(const std::vector<std::uint8_t>& b, size_t p) {
    return static_cast<std::int32_t>(rd32(b, p));
}
void wr16(std::vector<std::uint8_t>& b, size_t p, std::uint16_t v) {
    b[p] = v & 0xFF;
    b[p + 1] = (v >> 8) & 0xFF;
}
void wr32(std::vector<std::uint8_t>& b, size_t p, std::uint32_t v) {
    for (int i = 0; i < 4; ++i) b[p + i] = (v >> (8 * i)) & 0xFF;
}
} // namespace

bool BmpImage::load(const std::filesystem::path& path, std::string& error) {
    std::ifstream f(path, std::ios::binary);
    if (!f) { error = "Не удалось открыть файл"; return false; }
    std::vector<std::uint8_t> buf((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    // --- BITMAPFILEHEADER (14 байт) ---
    if (buf.size() < 54 || buf[0] != 'B' || buf[1] != 'M') {
        error = "Это не BMP (нет сигнатуры 'BM')";
        return false;
    }
    const std::uint32_t pixelOffset = rd32(buf, 10);

    // --- BITMAPINFOHEADER (40+ байт) ---
    const std::uint32_t headerSize = rd32(buf, 14);
    if (headerSize < 40) { error = "Неподдерживаемый заголовок (старый формат)"; return false; }
    const int w = rd32s(buf, 18);
    int h = rd32s(buf, 22);
    const int bpp = rd16(buf, 28);
    const std::uint32_t compression = rd32(buf, 30);
    std::uint32_t colorsUsed = rd32(buf, 46);

    const bool topDown = h < 0; // отрицательная высота = строки сверху вниз
    h = std::abs(h);

    if (w <= 0 || h <= 0) { error = "Некорректные размеры"; return false; }
    if (compression != 0 && !(compression == 3 && bpp == 32)) {
        error = "Сжатые BMP не поддерживаются";
        return false;
    }
    if (bpp != 8 && bpp != 24 && bpp != 32) {
        error = "Поддерживаются только 8, 24 и 32 бита на пиксель";
        return false;
    }

    // --- Палитра (для 8 бит) ---
    std::vector<std::uint8_t> palette; // BGRA
    if (bpp == 8) {
        if (colorsUsed == 0) colorsUsed = 256;
        const size_t palPos = 14 + headerSize;
        if (palPos + colorsUsed * 4 > buf.size()) { error = "Повреждённая палитра"; return false; }
        palette.assign(buf.begin() + palPos, buf.begin() + palPos + colorsUsed * 4);
    }

    // --- Пиксельные данные: строки выровнены до 4 байт ---
    const size_t stride = ((static_cast<size_t>(w) * bpp + 31) / 32) * 4;
    if (pixelOffset + stride * h > buf.size()) { error = "Файл обрезан"; return false; }

    width = w;
    height = h;
    data.assign(static_cast<size_t>(w) * h * 3, 0);

    for (int y = 0; y < h; ++y) {
        const int srcRow = topDown ? y : (h - 1 - y);
        const std::uint8_t* row = &buf[pixelOffset + stride * srcRow];
        for (int x = 0; x < w; ++x) {
            std::uint8_t* dst = px(x, y);
            if (bpp == 8) {
                const std::uint32_t idx = row[x];
                if (idx < colorsUsed) {
                    dst[0] = palette[idx * 4 + 2]; // R
                    dst[1] = palette[idx * 4 + 1]; // G
                    dst[2] = palette[idx * 4 + 0]; // B
                }
            } else {
                const int bytes = bpp / 8;
                dst[0] = row[x * bytes + 2]; // R (в файле порядок B, G, R)
                dst[1] = row[x * bytes + 1];
                dst[2] = row[x * bytes + 0];
            }
        }
    }
    return true;
}

bool BmpImage::save(const std::filesystem::path& path, std::string& error) const {
    if (empty()) { error = "Нет изображения"; return false; }
    const size_t stride = ((static_cast<size_t>(width) * 24 + 31) / 32) * 4;
    const size_t imageSize = stride * height;
    std::vector<std::uint8_t> out(54 + imageSize, 0);

    out[0] = 'B'; out[1] = 'M';
    wr32(out, 2, static_cast<std::uint32_t>(out.size()));
    wr32(out, 10, 54);
    wr32(out, 14, 40);
    wr32(out, 18, static_cast<std::uint32_t>(width));
    wr32(out, 22, static_cast<std::uint32_t>(height)); // положительная = снизу вверх
    wr16(out, 26, 1);
    wr16(out, 28, 24);
    wr32(out, 34, static_cast<std::uint32_t>(imageSize));

    for (int y = 0; y < height; ++y) {
        std::uint8_t* row = &out[54 + stride * (height - 1 - y)];
        for (int x = 0; x < width; ++x) {
            const std::uint8_t* s = px(x, y);
            row[x * 3 + 0] = s[2]; // B
            row[x * 3 + 1] = s[1]; // G
            row[x * 3 + 2] = s[0]; // R
        }
    }

    std::ofstream f(path, std::ios::binary);
    if (!f) { error = "Не удалось создать файл"; return false; }
    f.write(reinterpret_cast<const char*>(out.data()), static_cast<std::streamsize>(out.size()));
    return static_cast<bool>(f);
}
