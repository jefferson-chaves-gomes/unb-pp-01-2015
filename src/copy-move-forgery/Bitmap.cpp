#include "Bitmap.h"

#include <fstream>
#include <iostream>

/* construtores */
Bitmap::Bitmap()
    : valid_(false),
    file_name_(""),
    data_(0),
    bytes_per_pixel_(3),
    length_(0),
    width_(0),
    height_(0),
    row_increment_(0),
    channel_mode_(bgr_mode)
{
    /* vazio */
}

/* carrega imagem jah existente */
Bitmap::Bitmap(const std::string& _filename)
    : file_name_(_filename),
    data_(0),
    bytes_per_pixel_(0),
    length_(0),
    width_(0),
    height_(0),
    row_increment_(0),
    channel_mode_(bgr_mode)
{
    load_bitmap(file_name_);
}

/* cria imagem com as dimensoes informadas */
Bitmap::Bitmap(const unsigned int width, const unsigned int height)
    : file_name_(""),
    data_(0),
    bytes_per_pixel_(3),
    length_(0),
    width_(width),
    height_(height),
    row_increment_(0),
    channel_mode_(bgr_mode)
{
    create_bitmap();
}

/* copia imagem */
Bitmap::Bitmap(const Bitmap& image)
    : file_name_(image.file_name_),
    data_(0),
    bytes_per_pixel_(3),
    width_(image.width_),
    height_(image.height_),
    row_increment_(0),
    channel_mode_(bgr_mode)
{
    create_bitmap();
    std::copy(image.data_, image.data_ + image.length_, data_);
}

/* destrutor */
Bitmap::~Bitmap()
{
    delete[] data_;
}

/* aloca area de memoria para comportar a imagem criada */
void Bitmap::create_bitmap()
{
    length_ = width_ * height_ * bytes_per_pixel_;
    row_increment_ = width_ * bytes_per_pixel_;
    if(0 != data_)
        delete[] data_;

    data_ = new unsigned char[length_];
    valid_ = true;
}

Bitmap Bitmap::getLines(unsigned int initialLine, unsigned int length) const
{
    if(initialLine > height_-1)
        return Bitmap();
    if(initialLine + length > height_-1)
        length = height_ - initialLine;

    int posStart = initialLine * width_ * bytes_per_pixel_;
    int posEnd = posStart + length * width_ * bytes_per_pixel_;

    Bitmap section(width_, length);
    std::copy(data_ + posStart, data_ + posEnd, section.data_);

    return section;
}

Bitmap Bitmap::getBlock(Pos const& pos, int sizeBlk) const
{
    Bitmap block(sizeBlk, sizeBlk);

    for(int i=0; i<sizeBlk; i++)
    {
        int posStart = ((pos.y + i) * row_increment_ + pos.x * bytes_per_pixel_);
        int posEnd = posStart + sizeBlk * bytes_per_pixel_;

        int blkPosStart(i * sizeBlk * bytes_per_pixel_);
        std::copy(data_ + posStart, data_ + posEnd, block.data_ + blkPosStart);
    }

    return block;
}

Bitmap& Bitmap::operator=(const Bitmap& image)
{
    if(this != &image)
    {
        file_name_ = image.file_name_;
        length_ = image.length_;
        bytes_per_pixel_ = image.bytes_per_pixel_;
        width_ = image.width_;
        height_ = image.height_;
        row_increment_ = 0;
        channel_mode_ = image.channel_mode_;
        create_bitmap();
        std::copy(image.data_, image.data_ + image.length_, data_);
    }
    return *this;
}

/* recupera os atributos RGB de um pixel da imagem */
void Bitmap::getPixel(const unsigned int x, const unsigned int y,
    unsigned char& red,
    unsigned char& green,
        unsigned char& blue) const
{
    if(x >= getWidth() || y >= getHeight())
    {
        // isto substitui o padding
        red = green = blue = 0;
    }
    else
    {
        blue = data_[(y * row_increment_) + (x * bytes_per_pixel_ + 0)];
        green = data_[(y * row_increment_) + (x * bytes_per_pixel_ + 1)];
        red = data_[(y * row_increment_) + (x * bytes_per_pixel_ + 2)];
    }
}

/* grava os atributos RGB de um pixel da imagem */
void Bitmap::setPixel(const unsigned int x, const unsigned int y,
    const unsigned char red,
    const unsigned char green,
        const unsigned char blue)
{
    data_[(y * row_increment_) + (x * bytes_per_pixel_ + 0)] = blue;
    data_[(y * row_increment_) + (x * bytes_per_pixel_ + 1)] = green;
    data_[(y * row_increment_) + (x * bytes_per_pixel_ + 2)] = red;
}

/* recupera as dimensoes da imagem */
unsigned int Bitmap::getWidth() const
{
    return width_;
}

unsigned int Bitmap::getHeight() const
{
    return height_;
}

/* grava a imagem em arquivo */
void Bitmap::saveImage(const std::string& file_name)
{
    std::ofstream stream(file_name.c_str(), std::ios::binary);
    if(!stream)
    {
        std::cout << "Bitmap::save_image(): Error - Could not open file " << file_name << " for writing!" << std::endl;
        return;
    }

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    /* configura o header com os atributos da imagem */
    infoHeader.width = width_;
    infoHeader.height = height_;
    infoHeader.bit_count = static_cast<unsigned short>(bytes_per_pixel_ << 3);
    infoHeader.clr_important = 0;
    infoHeader.clr_used = 0;
    infoHeader.compression = 0;
    infoHeader.planes = 1;
    infoHeader.size = 40;
    infoHeader.x_pels_per_meter = 0;
    infoHeader.y_pels_per_meter = 0;
    infoHeader.size_image = (((infoHeader.width * bytes_per_pixel_) + 3) & 0xFFFC) * infoHeader.height;

    fileHeader.type = 19778;
    fileHeader.size = 55 + infoHeader.size_image;
    fileHeader.reserved1 = 0;
    fileHeader.reserved2 = 0;
    fileHeader.off_bits = infoHeader.struct_size() + fileHeader.struct_size();

    BitmapFile::writeFileHeader(stream, fileHeader, infoHeader);

    unsigned int padding = (4 - ((3 * width_) % 4)) % 4;
    char padding_data[4] = {0x0,0x0,0x0,0x0};
    for(unsigned int i = 0; i < height_; ++i)
    {
        unsigned char* data_ptr = data_ + (row_increment_ * (height_ - i - 1));
        stream.write(reinterpret_cast<char*>(data_ptr), sizeof(unsigned char) * bytes_per_pixel_ * width_);
        stream.write(padding_data, padding);
    }
    stream.close();
}
//
///* tratamento de imagem colorida para tons de cinza */
//void Bitmap::convertToGrayscale()
//{
//    double r_scaler = 0.299;
//    double g_scaler = 0.587;
//    double b_scaler = 0.114;
//
//    if (rgb_mode == channel_mode_)
//    {
//        double tmp = r_scaler;
//        r_scaler = b_scaler;
//        b_scaler = tmp;
//    }
//
//    for(unsigned char* it = data_; it < (data_ + length_);)
//    {
//        unsigned char gray_value = static_cast<unsigned char>(
//            (r_scaler * (*(it + 2))) +
//            (g_scaler * (*(it + 1))) +
//            (b_scaler * (*(it + 0))) );
//        *(it++) = gray_value;
//        *(it++) = gray_value;
//        *(it++) = gray_value;
//    }
//}

/* retorna conteudo da area de memoria da imagem */
const unsigned char* Bitmap::data()
{
    return data_;
}

/* retorna os dados da imagem contidos em uma linha da matriz de pixels */
unsigned char* Bitmap::row(unsigned int row_index) const
{
    return data_ + (row_index * row_increment_);
}

/* carrega imagem existente em arquivo */
void Bitmap::load_bitmap(const std::string& _filename)
{
    file_name_ = _filename;

    std::ifstream stream(file_name_.c_str(), std::ios::binary);
    if(!stream)
    {
        std::cerr << "Bitmap::load_bitmap() ERROR: Couldn't open file " << file_name_ << std::endl;
        return;
    }

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    BitmapFile::readFileHeader(stream, fileHeader, infoHeader);

    if(fileHeader.type != 19778)
    {
        stream.close();
        std::cerr << "Bitmap::load_bitmap() ERROR: Bitmap - Invalid type value " << fileHeader.type << " expected 19778." << std::endl;
        return;
    }

    if(infoHeader.bit_count != 24)
    {
        stream.close();
        std::cerr << "Bitmap::load_bitmap() ERROR: Bitmap - Invalid bit depth " << infoHeader.bit_count << " expected 24." << std::endl;
        return;
    }

    height_ = infoHeader.height;
    width_ = infoHeader.width;

    bytes_per_pixel_ = infoHeader.bit_count >> 3;

    unsigned int padding = (4 - ((3 * width_) % 4)) % 4;
    char padding_data[4] = {0,0,0,0};

    create_bitmap();

    for(unsigned int i = 0; i < height_; ++i)
    {
        /* ler linhas em ordem inversa */
        unsigned char* data_ptr = row(height_ - i - 1);
        stream.read(reinterpret_cast<char*>(data_ptr), sizeof(char) * bytes_per_pixel_ * width_);
        stream.read(padding_data, padding);
    }

    valid_ = true;
}

std::string Bitmap::getPath() const
{
    return file_name_;
}

/* Escreve cabecalho do arquivo em memoria */
void BitmapFile::writeFileHeader(std::ofstream& stream, const BMPFileHeader& fileHeader, BMPInfoHeader const& infoHeader)
{
    if(big_endian())
    {
        flip(fileHeader.type);
        flip(fileHeader.size);
        flip(fileHeader.reserved1);
        flip(fileHeader.reserved2);
        flip(fileHeader.off_bits);
    }
    write_to_stream(stream, fileHeader.type);
    write_to_stream(stream, fileHeader.size);
    write_to_stream(stream, fileHeader.reserved1);
    write_to_stream(stream, fileHeader.reserved2);
    write_to_stream(stream, fileHeader.off_bits);

    writeInfoHeader(stream, infoHeader);
}

/* Le cabecalho do arquivo em memoria */
void BitmapFile::readFileHeader(std::ifstream& stream, BMPFileHeader& fileHeader, BMPInfoHeader &infoHeader)
{
    read_from_stream(stream, fileHeader.type);
    read_from_stream(stream, fileHeader.size);
    read_from_stream(stream, fileHeader.reserved1);
    read_from_stream(stream, fileHeader.reserved2);
    read_from_stream(stream, fileHeader.off_bits);

    if(big_endian())
    {
        flip(fileHeader.type);
        flip(fileHeader.size);
        flip(fileHeader.reserved1);
        flip(fileHeader.reserved2);
        flip(fileHeader.off_bits);
    }

    readInfoHeader(stream, infoHeader);
}

/* Le informacoes do arquivo em memoria */
void BitmapFile::readInfoHeader(std::ifstream& stream, BMPInfoHeader& infoHeader)
{
    read_from_stream(stream, infoHeader.size);
    read_from_stream(stream, infoHeader.width);
    read_from_stream(stream, infoHeader.height);
    read_from_stream(stream, infoHeader.planes);
    read_from_stream(stream, infoHeader.bit_count);
    read_from_stream(stream, infoHeader.compression);
    read_from_stream(stream, infoHeader.size_image);
    read_from_stream(stream, infoHeader.x_pels_per_meter);
    read_from_stream(stream, infoHeader.y_pels_per_meter);
    read_from_stream(stream, infoHeader.clr_used);
    read_from_stream(stream, infoHeader.clr_important);

    if(big_endian())
    {
        flip(infoHeader.size);
        flip(infoHeader.width);
        flip(infoHeader.height);
        flip(infoHeader.planes);
        flip(infoHeader.bit_count);
        flip(infoHeader.compression);
        flip(infoHeader.size_image);
        flip(infoHeader.x_pels_per_meter);
        flip(infoHeader.y_pels_per_meter);
        flip(infoHeader.clr_used);
        flip(infoHeader.clr_important);
    }
}

/* Escreve informacoes do arquivo em memoria */
void BitmapFile::writeInfoHeader(std::ofstream& stream, const BMPInfoHeader& infoHeader)
{
    if(big_endian())
    {
        flip(infoHeader.size);
        flip(infoHeader.width);
        flip(infoHeader.height);
        flip(infoHeader.planes);
        flip(infoHeader.bit_count);
        flip(infoHeader.compression);
        flip(infoHeader.size_image);
        flip(infoHeader.x_pels_per_meter);
        flip(infoHeader.y_pels_per_meter);
        flip(infoHeader.clr_used);
        flip(infoHeader.clr_important);
    }

    write_to_stream(stream, infoHeader.size);
    write_to_stream(stream, infoHeader.width);
    write_to_stream(stream, infoHeader.height);
    write_to_stream(stream, infoHeader.planes);
    write_to_stream(stream, infoHeader.bit_count);
    write_to_stream(stream, infoHeader.compression);
    write_to_stream(stream, infoHeader.size_image);
    write_to_stream(stream, infoHeader.x_pels_per_meter);
    write_to_stream(stream, infoHeader.y_pels_per_meter);
    write_to_stream(stream, infoHeader.clr_used);
    write_to_stream(stream, infoHeader.clr_important);
}

/*
 void Bitmap::reverse_channels()
 {
 if(3 != bytes_per_pixel_) return;
 for(unsigned char* it = data_; it < (data_ + length_); it += bytes_per_pixel_)
 {
 unsigned char tmp = *(it + 0);
 *(it + 0) = *(it + 2);
 *(it + 2) = tmp;
 }
 }

 template<typename T>
 T Bitmap::clamp(const T& v, const T& lower_range, const T& upper_range)
 {
 if (v < lower_range)
 return lower_range;
 else if (v > upper_range)
 return upper_range;
 else
 return v;
 }
 */

bool BitmapFile::big_endian()
{
    unsigned int v = 0x01;
    return (1 != reinterpret_cast<char*>(&v)[0]);
}

unsigned short BitmapFile::flip(const unsigned short& v)
{
    return ((v >> 8) | (v << 8));
}

unsigned int BitmapFile::flip(const unsigned int& v)
{
    return (((v & 0xFF000000) >> 0x18) | ((v & 0x000000FF) << 0x18) |
        ((v & 0x00FF0000) >> 0x08) | ((v & 0x0000FF00) << 0x08));
}

template<typename T>
void BitmapFile::read_from_stream(std::ifstream& stream, T& t)
{
    stream.read(reinterpret_cast<char*>(&t), sizeof(T));
}

template<typename T>
void BitmapFile::write_to_stream(std::ofstream& stream, const T& t)
{
    stream.write(reinterpret_cast<const char*>(&t), sizeof(T));
}
