#include <iostream>
#include <fstream>
#include <vector>
#include <iostream>
#pragma pack(push, 1)
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
// BMP 文件头
struct BMPFileHeader {
    uint16_t fileType;       // 文件类型，固定为0x4D42 ('BM')
    uint32_t fileSize;       // 文件大小，以字节为单位
    uint16_t reserved1;      // 保留字段1，固定为0
    uint16_t reserved2;      // 保留字段2，固定为0
    uint32_t dataOffset;     // 数据偏移量，即图像数据开始的位置
};

// BMP 信息头
struct BMPInfoHeader {
    uint32_t headerSize;         // 信息头大小，固定为40
    int32_t width;               // 图像宽度，以像素为单位
    int32_t height;              // 图像高度，以像素为单位
    uint16_t planes;             // 颜色平面数，固定为1
    uint16_t bitCount;           // 每个像素的位数，即每个像素的颜色深度
    uint32_t compression;        // 压缩类型，0表示不压缩
    uint32_t imageSize;          // 图像数据大小，以字节为单位
    int32_t xPixelsPerMeter;     // 水平分辨率，像素/米
    int32_t yPixelsPerMeter;     // 垂直分辨率，像素/米
    uint32_t colorsUsed;         // 使用的颜色索引数，0表示使用所有可能的颜色索引
    uint32_t colorsImportant;    // 重要颜色索引数，0表示所有颜色都重要
};

#pragma pack(pop)

void writeBMP(const std::string& filename, const unsigned char* pixels, int width, int height);