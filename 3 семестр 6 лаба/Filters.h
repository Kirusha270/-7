#pragma once
#include "BmpImage.h"

namespace Filters {
void grayscale(BmpImage& img); // gray = 0.299*R + 0.587*G + 0.114*B
void negative(BmpImage& img);  // 255 - value
void blur(BmpImage& img);      // усреднение соседей 3x3
void sharpen(BmpImage& img);   // ядро с центральным плюсом и отрицательными соседями
}
