#include"bmp.h"
void writeBMP(const std::string& filename, const unsigned char* pixels, int width, int height) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    // BMP 文件头
    BMPFileHeader fileHeader;
    fileHeader.fileType = 0x4D42;
    fileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + width * height * 3; // RGB
    fileHeader.reserved1 = 0;
    fileHeader.reserved2 = 0;
    fileHeader.dataOffset = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

    // BMP 信息头
    BMPInfoHeader infoHeader;
    infoHeader.headerSize = sizeof(BMPInfoHeader);
    infoHeader.width = width;
    infoHeader.height = height;
    infoHeader.planes = 1;
    infoHeader.bitCount = 24; // 24位色深度，即每个像素由3个字节组成
    infoHeader.compression = 0;
    infoHeader.imageSize = width * height * 3; // RGB
    infoHeader.xPixelsPerMeter = 0;
    infoHeader.yPixelsPerMeter = 0;
    infoHeader.colorsUsed = 0;
    infoHeader.colorsImportant = 0;

    // 写入文件头
    file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(BMPFileHeader));

    // 写入信息头
    file.write(reinterpret_cast<const char*>(&infoHeader), sizeof(BMPInfoHeader));

    // 写入像素数据（BGR顺序）
    for (int i = 0; i < width * height; ++i) {
        file.put(pixels[i * 3 + 2]); // B
        file.put(pixels[i * 3 + 1]); // G
        file.put(pixels[i * 3]);     // R
    }

    std::cout << "BMP image saved to: " << filename << std::endl;
}