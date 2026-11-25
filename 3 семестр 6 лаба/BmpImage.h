#pragma once
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

// Изображение в памяти: RGB, 3 байта на пиксель, строки сверху вниз.
struct BmpImage {
    int width = 0;
    int height = 0;
    std::vector<std::uint8_t> data;

    bool empty() const { return data.empty(); }
    std::uint8_t* px(int x, int y) { return &data[(static_cast<size_t>(y) * width + x) * 3]; }
    const std::uint8_t* px(int x, int y) const { return &data[(static_cast<size_t>(y) * width + x) * 3]; }

    // Ручное чтение BMP (8/24/32 бит, без сжатия)
    bool load(const std::filesystem::path& path, std::string& error);
    // Запись 24-битного BMP
    bool save(const std::filesystem::path& path, std::string& error) const;
};
