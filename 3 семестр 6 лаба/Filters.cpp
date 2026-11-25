#include "Filters.h"
#include <algorithm>
#include <cmath>

namespace {
// Свёртка 3x3, на границах координаты "прижимаются" к краю
void convolve3x3(BmpImage& img, const float k[9], float divisor) {
    if (img.empty()) return;
    const BmpImage src = img;
    for (int y = 0; y < src.height; ++y) {
        for (int x = 0; x < src.width; ++x) {
            float sum[3] = {0, 0, 0};
            for (int ky = -1; ky <= 1; ++ky) {
                const int yy = std::clamp(y + ky, 0, src.height - 1);
                for (int kx = -1; kx <= 1; ++kx) {
                    const int xx = std::clamp(x + kx, 0, src.width - 1);
                    const float coef = k[(ky + 1) * 3 + (kx + 1)];
                    const std::uint8_t* p = src.px(xx, yy);
                    for (int c = 0; c < 3; ++c) sum[c] += coef * p[c];
                }
            }
            std::uint8_t* d = img.px(x, y);
            for (int c = 0; c < 3; ++c)
                d[c] = static_cast<std::uint8_t>(std::clamp(std::lround(sum[c] / divisor), 0L, 255L));
        }
    }
}
} // namespace

void Filters::grayscale(BmpImage& img) {
    for (int y = 0; y < img.height; ++y)
        for (int x = 0; x < img.width; ++x) {
            std::uint8_t* p = img.px(x, y);
            const auto g = static_cast<std::uint8_t>(
                std::clamp(std::lround(0.299 * p[0] + 0.587 * p[1] + 0.114 * p[2]), 0L, 255L));
            p[0] = p[1] = p[2] = g;
        }
}

void Filters::negative(BmpImage& img) {
    for (auto& v : img.data) v = static_cast<std::uint8_t>(255 - v);
}

void Filters::blur(BmpImage& img) {
    static const float k[9] = {1, 1, 1,
                               1, 1, 1,
                               1, 1, 1};
    convolve3x3(img, k, 9.0f);
}

void Filters::sharpen(BmpImage& img) {
    static const float k[9] = { 0, -1,  0,
                               -1,  5, -1,
                               0, -1,  0};
    convolve3x3(img, k, 1.0f);
}
