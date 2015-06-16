#ifndef BITMAP_H_
#define BITMAP_H_

#include <string>
#include "utils.h"

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
    unsigned char* data_;
    unsigned int bytes_per_pixel_;
    unsigned int length_;
    unsigned int width_;
    unsigned int height_;
    unsigned int row_increment_;
    channel_mode channel_mode_;

public:
    Bitmap();
    Bitmap(const std::string& _filename);
    Bitmap(const unsigned int width, const unsigned int height);
    Bitmap(const Bitmap& image);
    ~Bitmap();
    Bitmap& operator=(const Bitmap& image);
    void getPixel(const unsigned int x, const unsigned int y, unsigned char& red, unsigned char& green,
            unsigned char& blue) const;
    void setPixel(const unsigned int x, const unsigned int y, const unsigned char red, const unsigned char green,
            const unsigned char blue);
    unsigned int getWidth() const;
    unsigned int getHeight() const;
    void saveImage(const std::string& file_name);
//    void convertToGrayscale();
    const unsigned char* data();
    unsigned char* row(unsigned int row_index) const;
    std::string getPath();

private:
    void create_bitmap();
    void load_bitmap();
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
    static unsigned short flip(const unsigned short& v);
    static unsigned int flip(const unsigned int& v);
    template<typename T>
    static void read_from_stream(std::ifstream& stream, T& t);
    template<typename T>
    static void write_to_stream(std::ofstream& stream, const T& t);
};

#endif /* BITMAP_H_ */
