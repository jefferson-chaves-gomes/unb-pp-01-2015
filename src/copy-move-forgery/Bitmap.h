#ifndef BITMAP_H_
#define BITMAP_H_

#include <string>
#include "utils.h"
#include <stdint.h>

typedef struct bitmap_file_header
{
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t off_bits;

    uint64_t struct_size()
    {
        return
            sizeof(type) +
            sizeof(size) +
            sizeof(reserved1) +
            sizeof(reserved2) +
            sizeof(off_bits);
    }
} BMPFileHeader;

typedef struct bitmap_information_header
{
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;
    int32_t x_pels_per_meter;
    int32_t y_pels_per_meter;
    uint32_t clr_used;
    uint32_t clr_important;

    unsigned int struct_size()
    {
        return
            sizeof(size) +
            sizeof(width) +
            sizeof(height) +
            sizeof(planes) +
            sizeof(bit_count) +
            sizeof(compression) +
            sizeof(size_image) +
            sizeof(x_pels_per_meter) +
            sizeof(y_pels_per_meter) +
            sizeof(clr_used) +
            sizeof(clr_important);
    }
} BMPInfoHeader;

/* Classe Bitmap: objeto que representa a imagem .bmp a ser manipulada */
class Bitmap
{
public:
    enum channel_mode
    {
        rgb_mode = 0,
        bgr_mode = 1
    };

    enum color_plane
    {
        blue_plane = 0,
        green_plane = 1,
        red_plane = 2
    };

private:
    bool valid_;
    std::string file_name_;
    unsigned int width_;
    unsigned int height_;
    unsigned int row_increment_;
    channel_mode channel_mode_;

public:
    unsigned int bytes_per_pixel_;
    uint8_t* data_;
    unsigned int length_;
    Bitmap();
    Bitmap(const std::string& _filename);
    Bitmap(const unsigned int width, const unsigned int height);
    Bitmap(const Bitmap& image);
    ~Bitmap();
    Bitmap& operator=(const Bitmap& image);
    void getPixel(const unsigned int x, const unsigned int y, unsigned char& red, unsigned char& green, unsigned char& blue) const;
    void setPixel(const unsigned int x, const unsigned int y, const unsigned char red, const unsigned char green,
            const unsigned char blue);
    unsigned int getWidth() const;
    unsigned int getHeight() const;
    void saveImage(const std::string& file_name);
//    void convertToGrayscale();
    const unsigned char* data();
    unsigned char* row(unsigned int row_index) const;
    std::string getPath() const;
    Bitmap getBlock(Pos const& pos, int sizeBlk) const;
    Bitmap getLines(unsigned int initialLine, unsigned int length) const;
    void load_bitmap(const std::string& _filename);

private:
    void create_bitmap();
    void reverse_channels();
    template<typename T>
    T clamp(const T& v, const T& lower_range, const T& upper_range);
};

class BitmapFile
{
    BitmapFile();
    ~BitmapFile();
public:
    /* Funcoes utilizadas para tratamento de arquivo */
    static void readFileHeader(std::ifstream& stream, BMPFileHeader& bfh, BMPInfoHeader &infoHeader);
    static void writeFileHeader(std::ofstream& stream, const BMPFileHeader& bfh, const BMPInfoHeader& bih);

private:
    static void readInfoHeader(std::ifstream& stream, BMPInfoHeader& bih);
    static void writeInfoHeader(std::ofstream& stream, const BMPInfoHeader& bih);

    /* Funcoes uteis para manipulacao do stream de bytes */
    static bool big_endian();
    static int16_t flip(int16_t const& v);
    static uint16_t flip(uint16_t const& v);
    static int32_t flip(int32_t const& v);
    static uint32_t flip(uint32_t const& v);
    template<typename T>
    static void read_from_stream(std::ifstream& stream, T& t);
    template<typename T>
    static void write_to_stream(std::ofstream& stream, const T& t);
};

#endif /* BITMAP_H_ */
