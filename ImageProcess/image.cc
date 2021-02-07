/*
        SAVE_AS_PNG FUNCTION AND LOAD_PNG FUNCTION IS WRITTEN BY MUSTAFA ÖZUYSAL
*/

#include "image.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <cmath>
#include <vector>

#include <png.h>

#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

#ifndef png_infopp_NULL
#define png_infopp_NULL (png_infopp) NULL
#endif

#ifndef int_p_NULL
#define int_p_NULL (int *)NULL
#endif

using namespace std;

using std::cerr;
using std::clog;
using std::cos;
using std::endl;
using std::exit;
using std::fclose;
using std::fopen;
using std::ios;
using std::memcpy;
using std::ofstream;
using std::round;
using std::sin;
using std::string;
using std::vector;

namespace ceng391
{

        Image::Image(int width, int height, int n_channels, int step)
            : m_width(width), m_height(height), m_n_channels(n_channels)
        {
                if (step < width * n_channels)
                        step = width * n_channels;
                m_step = step;
                m_data = new uchar[m_height * m_step];
        }

        Image::~Image()
        {
                // clog << "Deleting image" << endl;
                delete[] m_data;
        }

        Image *Image::new_gray(int width, int height)
        {
                return new Image(width, height, 1);
        }

        Image *Image::from_pnm(const std::string &filename)
        {
                // Allocate small temporary image
                Image *img = new_gray(1, 1);
                if (!img->load_pnm(filename))
                {
                        exit(EXIT_FAILURE);
                }
                return img;
        }

        Image *Image::from_png(const std::string &filename, bool load_as_grayscale)
        {
                // Allocate small temporary image
                Image *img = new_gray(1, 1);
                if (!img->load_png(filename, load_as_grayscale))
                {
                        exit(EXIT_FAILURE);
                }
                return img;
        }

        void Image::set_rect(int tlx, int tly, int width, int height, uchar value)
        {

                if (tlx < 0)
                {
                        width += tlx;
                        tlx = 0;
                }

                if (tly < 0)
                {
                        height += tly;
                        tly = 0;
                }

                for (int y = tly; y < tly + height; ++y)
                {
                        if (y >= m_height)
                        {
                                break;
                        }

                        uchar *row = m_data + y * m_step;

                        //x start from zero to number of channels * width
                        for (int x = tlx * m_n_channels; x < m_n_channels * (tlx + width); x += m_n_channels)
                        {
                                if (x >= m_n_channels * m_width)
                                {
                                        break;
                                }

                                /*
                                 If channel 1 : roow[x]= value
                                 channel 3 : row[x]= value, row[x+1]= value, row[x+2]= value
                                 channel 4 : row[x]= value, row[x+1]= value, row[x+2]= value, row[x+3]= value
                                */

                                int channel = 0;
                                while (channel < m_n_channels)
                                {
                                        row[x + channel] = value;
                                        channel++;
                                }
                        }
                }
        }

        Image Image::read_pnm(const std::string &filename)
        {

                /*
                Open image file and get first data to detect the magicHead
                */

                cerr << "read_pnm function \n";
                int rowNumber = 0, columnNumber = 0, max_val = 0;
                ifstream fileRead(filename, ios::binary);
                stringstream stream_string;
                string line = "";
                getline(fileRead, line); // get the first line to detect the magicHead
                cout << "Version : " << line << endl;

                if (line.compare("P5") == 0)
                {
                        /*
                        If magicHead is P5, get the w,h value from file. After that create an 1 channel image with w,h values.
                        Get the pixel value that is read from file and assign pixel to the image m_data row index.
                        */
                        stream_string << fileRead.rdbuf();
                        stream_string >> columnNumber >> rowNumber >> max_val; // get image w,h,bits values.
                        cout << "Read_Image : filename " << filename << " " << columnNumber << " columns and " << rowNumber << " rows and " << max_val << " bits" << endl;

                        Image *imgNew = new Image(columnNumber, rowNumber, 1);
                        cout << "New_Image : " << imgNew->h() << " height and " << imgNew->w() << " width" << endl;
                        unsigned char pixel;
                        for (int y = 0; y < imgNew->h(); ++y)
                        {

                                uchar *row = imgNew->m_data + y * imgNew->m_step;
                                for (int x = 0; x < imgNew->w(); ++x)
                                {

                                        stream_string >> pixel; // get pixel value from file
                                        row[x] = pixel;         // assign pixel value to the m_data row index
                                }
                        }
                        fileRead.close();
                        return *imgNew;
                }

                else if (line.compare("P6") == 0)
                {
                        /*
                        If magicHead is P6, get the w,h value from file. After that create an 4 channel image with w,h values.
                        Get the R,G,B pixel values that are read from file and assign them to the image m_data row indexes.
                        */
                        stream_string << fileRead.rdbuf();
                        stream_string >> columnNumber >> rowNumber >> max_val; // get image w,h,bits values.
                        cout << "Read_Image : filename  " << filename << " " << columnNumber << " columns and " << rowNumber << " rows and " << max_val << " bits" << endl;

                        Image *imgNew = new Image(columnNumber, rowNumber, 4);
                        cout << "New_Image : " << imgNew->h() << " height and " << imgNew->w() << " width" << endl;
                        unsigned char pixelR, pixelG, pixelB;
                        for (int y = 0; y < imgNew->h(); ++y)
                        {

                                uchar *row = imgNew->m_data + y * imgNew->m_step;
                                for (int x = 0; x < imgNew->w() * imgNew->m_n_channels; x += imgNew->m_n_channels)
                                {

                                        stream_string >> pixelR >> pixelG >> pixelB; // get R,G;B pixel values from file

                                        // assign them to the m_data row indexes
                                        row[x] = pixelR;
                                        row[x + 1] = pixelG;
                                        row[x + 2] = pixelB;
                                        if (imgNew->m_n_channels == 4)
                                        {
                                                row[x + 3] = 255;
                                        }
                                }
                        }
                        fileRead.close();
                        return *imgNew;
                }
                else
                {
                        return *(new Image(0, 0, 0));
                }
        }

        void Image::save_as_pnm(const std::string &filename)
        {
                if (m_n_channels != 1)
                {
                        ofstream fout;
                        string magic_head = "P6";
                        string extended_name = filename + ".ppm";

                        fout.open(extended_name.c_str(), ios::out | ios::binary);
                        fout << magic_head << "\n";
                        fout << m_width << " " << m_height << " 255\n";

                        //for channel 3 just save data normally
                        if (m_n_channels == 3)
                        {
                                for (int y = 0; y < m_height; ++y)
                                {
                                        const uchar *row_data = this->data(y);

                                        fout.write(reinterpret_cast<const char *>(row_data), m_n_channels * m_width * sizeof(uchar));
                                }
                        }

                        else if (m_n_channels == 4)
                        {

                                for (int y = 0; y < m_height; ++y)
                                {

                                        const uchar *row_data = this->data(y);
                                        int index = 0;
                                        uchar *temp_row = new uchar[m_width * 3];

                                        //for channel 4, get R,G,B values, assign them to the temp row and save temp_row data
                                        for (int x = 0; x < m_step; x++)
                                        {
                                                if ((x + 1) % 4 == 0)
                                                {
                                                        continue; // just pass A values
                                                }
                                                else
                                                {
                                                        temp_row[index] = row_data[x];
                                                        index = index + 1;
                                                }
                                        }

                                        fout.write(reinterpret_cast<const char *>(temp_row), 3 * m_width * sizeof(uchar));
                                }
                        }
                        else
                        {
                                cerr << "Unexpected m_n_channel value : " << m_n_channels;
                                EXIT_FAILURE;
                        }

                        cerr << "saved as : " << extended_name;

                        fout.close();
                }
                else
                {
                        const string magic_head = "P5";
                        ofstream fout;
                        string extended_name = filename + ".pgm";
                        fout.open(extended_name.c_str(), ios::out | ios::binary);
                        fout << magic_head << "\n";
                        fout << m_width << " " << m_height << " 255\n";
                        for (int y = 0; y < m_height; ++y)
                        {
                                const uchar *row_data = this->data(y);
                                fout.write(reinterpret_cast<const char *>(row_data), m_width * sizeof(uchar));
                        }
                        cerr << "saved as : " << extended_name;

                        fout.close();
                }
        }

        /*
                SAVE_AS_PNG FUNCTION IS WRITTEN BY MUSTAFA ÖZUYSAL
        */
        bool Image::save_as_png(const std::string &filename)
        {
                // We open the output file with C style IO since we are using libpng
                // C-API
                FILE *fout = fopen(filename.c_str(), "w+b");
                if (!fout)
                {
                        cerr << "[ERROR][CENG391::Image] Failed open file for writing: " << filename << endl;
                        return false;
                }

                png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                              0, 0, 0);
                if (!png_ptr)
                {
                        cerr << "[ERROR][CENG391::Image] Failed to create PNG write structure!" << endl;
                        fclose(fout);
                        return false;
                }

                png_infop info_ptr = png_create_info_struct(png_ptr);
                if (!info_ptr)
                {
                        cerr << "[ERROR][CENG391::Image] Failed to create PNG info structure!" << endl;
                        png_destroy_write_struct(&png_ptr, png_infopp_NULL);
                        fclose(fout);
                        return false;
                }

                if (setjmp(png_jmpbuf(png_ptr)))
                {
                        cerr << "[ERROR][CENG391::Image] Failed to create PNG jump buffer!" << endl;
                        png_destroy_write_struct(&png_ptr, &info_ptr);
                        fclose(fout);
                        return false;
                }

                int color_type;
                switch (this->m_n_channels)
                {
                case 1:
                        color_type = PNG_COLOR_TYPE_GRAY;
                        break;
                case 4:
                        color_type = PNG_COLOR_TYPE_RGBA;
                        break;
                default:
                        cerr << "[ERROR][CENG391::Image] Unsupported image type for saving as PNG!" << endl;
                        png_destroy_write_struct(&png_ptr, &info_ptr);
                        fclose(fout);
                        return false;
                }

                png_init_io(png_ptr, fout);
                png_set_IHDR(png_ptr, info_ptr, this->m_width, this->m_height, 8,
                             color_type, PNG_INTERLACE_NONE,
                             PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
                png_write_info(png_ptr, info_ptr);

                png_bytepp row_pointers = (png_bytepp)malloc(this->m_height * sizeof(png_bytep));
                if (!row_pointers)
                {
                        cerr << "[ERROR][CENG391::Image]Error creating PNG row pointers" << endl;
                        png_destroy_write_struct(&png_ptr, &info_ptr);
                        fclose(fout);
                        return false;
                }

                for (png_int_32 k = 0; k < this->m_height; k++)
                {
                        row_pointers[k] = (png_bytep)(this->data(k));
                }

                png_write_image(png_ptr, row_pointers);
                png_write_end(png_ptr, info_ptr);

                png_destroy_write_struct(&png_ptr, &info_ptr);
                free(row_pointers);
                fclose(fout);

                return true;
        }

        /*
                LOAD_PNG FUNCTION IS WRITTEN BY MUSTAFA ÖZUYSAL
        */
        bool Image::load_png(const std::string &filename, bool load_as_grayscale)
        {
                // We open the output file with C style IO since we are using libpng
                // C-API
                FILE *fin = fopen(filename.c_str(), "r+b");
                if (!fin)
                {
                        cerr << "[ERROR][CENG391::Image] Failed to open file for reading: " << filename << endl;
                        return false;
                }

                png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                             NULL, NULL, NULL);
                if (!png_ptr)
                {
                        cerr << "[ERROR][CENG391::Image] Could not create PNG read structure" << endl;
                        fclose(fin);
                        return false;
                }

                png_infop info_ptr = png_create_info_struct(png_ptr);
                if (!info_ptr)
                {
                        cerr << "[ERROR][CENG391::Image] Could not create PNG info pointer" << endl;
                        png_destroy_read_struct(&png_ptr, png_infopp_NULL,
                                                png_infopp_NULL);
                        fclose(fin);
                        return false;
                }

                if (setjmp(png_jmpbuf(png_ptr)))
                {
                        cerr << "[ERROR][CENG391::Image] Could not set jump point for reading PNG file" << endl;
                        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
                        fclose(fin);
                        return false;
                }

                png_init_io(png_ptr, fin);
                png_read_info(png_ptr, info_ptr);

                png_uint_32 stream_width, stream_height;
                int bit_depth, color_type, interlace_type;
                png_get_IHDR(png_ptr, info_ptr, &stream_width, &stream_height, &bit_depth, &color_type,
                             &interlace_type, int_p_NULL, int_p_NULL);

                png_set_strip_16(png_ptr);
                if (color_type == PNG_COLOR_TYPE_GA)
                {
                        png_set_strip_alpha(png_ptr); /*(not recommended). */
                }

                png_set_packing(png_ptr);
                if (color_type == PNG_COLOR_TYPE_PALETTE)
                {
                        png_set_palette_to_rgb(png_ptr);
                }

                png_set_expand(png_ptr);

                // Depending on the type of image in the file and the load_as_grayscale
                // flag, we determine the desired number of channels of the output
                // image.
                int desired_n_channels = 4;
                if (load_as_grayscale)
                {
                        desired_n_channels = 1;
                        png_set_rgb_to_gray_fixed(png_ptr, 1, 30000, 59000);
                        png_set_strip_alpha(png_ptr); /*(not recommended). */
                }
                else
                {
                        if (color_type == PNG_COLOR_TYPE_GRAY ||
                            color_type == PNG_COLOR_TYPE_GA)
                        {
                                desired_n_channels = 1;
                        }

                        if (color_type == PNG_COLOR_TYPE_RGB ||
                            color_type == PNG_COLOR_TYPE_PALETTE)
                        {
                                png_set_add_alpha(png_ptr, 255, PNG_FILLER_AFTER);
                        }
                }

                // If the current image dimensions do not match the image to be loaded,
                // then reallocate with the desired dimensions.
                reallocate(stream_width, stream_height, desired_n_channels);

                png_bytepp row_pointers = (png_bytepp)malloc(this->m_height * sizeof(png_bytep));
                if (!row_pointers)
                {
                        cerr << "[ERROR][CENG391::Image]Error creating PNG row pointers" << endl;
                        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
                        fclose(fin);
                        return false;
                }
                for (int k = 0; k < this->m_height; k++)
                {
                        row_pointers[k] = (png_bytep)(this->data(k));
                }

                png_read_image(png_ptr, row_pointers);
                png_read_end(png_ptr, info_ptr);

                png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

                free(row_pointers);

                fclose(fin);

                return true;
        }

        Image Image::to_grayscale()
        {
                cerr << "to_grayscale() function" << endl;
                int rows = m_height;
                int cols = m_width;
                Image *imgNew = new Image(cols, rows, 1); // Create an image object for grayImage
                imgNew->set_zero();
                int cn = m_n_channels;

                unsigned char pixelR, pixelG, pixelB;
                for (int y = 0; y < imgNew->h(); ++y)
                {

                        uchar *rowImgRGBA = m_data + y * m_step;                 //m_data row of rgba image
                        uchar *rowImgGray = imgNew->m_data + y * imgNew->m_step; // m_data row of gray image
                        for (int x = 0; x < imgNew->w(); ++x)
                        {
                                // read m_data of rgbaImage and calculate the gray value.
                                //After that initialize it to the m_data row index of gray Image
                                pixelR = rowImgRGBA[x * cn];
                                pixelG = rowImgRGBA[x * cn + 1];
                                pixelB = rowImgRGBA[x * cn + 2];
                                int gray = (pixelR * 0.3 + pixelB * 0.11 + pixelG * 0.59);
                                rowImgGray[x] = gray;
                        }
                }

                return *imgNew;
        }
        Image Image::to_rgba()
        {
                cerr << "to_rgba() function" << endl;
                int rows = m_height;
                int cols = m_width;
                Image *imgRGBA = new Image(cols, rows, 4); // Create an image object for rgbaImage
                imgRGBA->set_zero();

                unsigned char pixel;
                for (int y = 0; y < imgRGBA->h(); ++y)
                {

                        uchar *rowImgGray = m_data + y * m_step;                   // m_data row of gray image
                        uchar *rowImgRGBA = imgRGBA->m_data + y * imgRGBA->m_step; //m_data row of rgba image
                        for (int x = 0; x < imgRGBA->w(); ++x)
                        {
                                // read m_data of grayImage
                                //After that initialize it to the m_data row indexes of rgba Image
                                rowImgRGBA[x * 4] = rowImgGray[x];
                                rowImgRGBA[x * 4 + 1] = rowImgGray[x];
                                rowImgRGBA[x * 4 + 2] = rowImgGray[x];
                                rowImgRGBA[x * 4 + 3] = 255;
                        }
                }

                return *imgRGBA;
        }

        Image *Image::filter_2d(int n, vector<vector<float>> kernel)
        {
                if (m_n_channels != 1)
                {
                        cerr << "[ERROR][CENG391::Image] Currently only grayscale images can be filtered2d!\n";
                        exit(EXIT_FAILURE);
                }

                //if n is not an odd number, make it nearest odd.
                if ((n - 1) % 2 != 0)
                {
                        n = n - 1;
                }

                //Creating a new image with new weight,height according to filter size
                Image *imgNew = new_gray(this->w() - n + 1, this->h() - n + 1);

                for (int i = 0; i < imgNew->m_height; i++)
                {
                        for (int j = 0; j < imgNew->m_width; j++)
                        {
                                float sum_of_filter = 0;

                                //multiplication of filter matrix data and image data, added to the sum_of_filter
                                for (int k = 0; k < n; k++)
                                {
                                        for (int l = 0; l < n; l++)
                                        {
                                                //boundary control
                                                if (i + k >= 0 && i + k < m_height && j + l >= 0 && j + l < m_width)
                                                {
                                                        sum_of_filter += ((float)m_data[(i + k) * m_step + (j + l)]) * ((float)kernel[k][l]);
                                                }
                                        }
                                }

                                //sum_of_filter control
                                if (sum_of_filter > 255)
                                {
                                        sum_of_filter = 255;
                                }
                                else if (sum_of_filter < 0)
                                {
                                        sum_of_filter = 0;
                                }

                                //assign the sum_of_filter to the imgNew data
                                imgNew->m_data[i * imgNew->m_step + j] = sum_of_filter;
                        }
                }
                return imgNew;
        }

        short *Image::deriv_x()
        {

                if (m_n_channels != 1)
                {
                        cerr << "[ERROR][CENG391::Image] Currently only grayscale images can be derived!\n";
                        exit(EXIT_FAILURE);
                }

                //Derivative X matrix
                vector<vector<short>> vect = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

                short *buffer = new short[this->w() * this->h()]; //returned short array

                Image *imgOut = new_gray(this->m_width, this->m_height); // derivation x output image of normal image.

                for (int y = 0; y < m_height; y++)
                {
                        uchar *row_data = imgOut->data(y);

                        for (int x = 0; x < m_width; x++)
                        {

                                short sum = 0; //holds value

                                //image boundary controlling
                                if (y == 0 || y == m_height - 1)
                                {
                                        sum = 0;
                                }

                                else if (x == 0 || x == m_width - 1)
                                {
                                        sum = 0;
                                }

                                else
                                {
                                        //multiplication of matrix data and image data, added to the sum
                                        for (int i = 0; i < 3; i++)
                                        {
                                                for (int j = 0; j < 3; j++)
                                                {
                                                        sum = sum + (m_data[((y + i) * m_width) + (x + j)] * vect[i][j]);
                                                }
                                        }
                                }
                                buffer[(y * m_width) + x] = sum; //assign the value to the buffer short array

                                int imageOut_value = sum;

                                //imageOut value controlling
                                if (imageOut_value > 255)
                                {
                                        imageOut_value = 255;
                                }

                                if (imageOut_value < 0)
                                {
                                        imageOut_value = 0;
                                }

                                row_data[x] = imageOut_value;
                        }
                }

                string output_filename = "/tmp/output_deriv_x.png";
                imgOut->save_as_png(output_filename);

                clog << "deriv_x() function output image :" << output_filename << endl;

                return buffer;
        }

        short *Image::deriv_y()
        {

                if (m_n_channels != 1)
                {
                        cerr << "[ERROR][CENG391::Image] Currently only grayscale images can be derived!\n";
                        exit(EXIT_FAILURE);
                }

                //Derivative Y matrix
                vector<vector<short>> vect = {
                    {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}}};

                short *buffer = new short[this->w() * this->h()]; //returned short array

                Image *imgOut = new_gray(this->m_width, this->m_height); // derivation y output image of normal image.

                for (int y = 0; y < (m_height); y++)
                {
                        uchar *row_data = imgOut->data(y);

                        for (int x = 0; x < (m_width); x++)
                        {

                                short sum = 0; //holds value

                                //image boundary controlling
                                if (y == 0 || y == m_height - 1)
                                {
                                        sum = 0;
                                }

                                else if (x == 0 || x == m_width - 1)
                                {
                                        sum = 0;
                                }

                                else
                                {
                                        //multiplication of matrix data and image data, added to the sum
                                        for (int i = 0; i < 3; i++)
                                        {
                                                for (int j = 0; j < 3; j++)
                                                {

                                                        sum = sum + (m_data[((y + i) * m_width) + (x + j)] * vect[i][j]);
                                                }
                                        }
                                }

                                buffer[(y * m_width) + x] = sum; //assign the value to the buffer short array

                                int imageOut_value = sum;

                                //imageOut value controlling
                                if (imageOut_value > 255)
                                {
                                        imageOut_value = 255;
                                }

                                if (imageOut_value < 0)
                                {
                                        imageOut_value = 0;
                                }

                                row_data[x] = imageOut_value;
                        }
                }

                string output_filename = "/tmp/output_deriv_y.png";
                imgOut->save_as_png(output_filename);

                clog << "deriv_y() function output image :" << output_filename << endl;

                return buffer;
        }

        void Image::warp_affine(vector<float> matrixA, vector<float> matrixT, Image *out, bool bilinear)
        {

                // modified rotation function codes,accepting boolean rotated by center = true

                if (m_n_channels != 1 || out->m_n_channels != 1)
                {
                        cerr << "[ERROR][CENG391::Image] Currently only grayscale images can be derived!\n";
                        exit(EXIT_FAILURE);
                }

                double hw = m_width / 2.0;
                double hh = m_height / 2.0;
                double hwr = out->m_width / 2.0;
                double hhr = out->m_height / 2.0;

                for (int y = 0; y < out->m_height; ++y)
                {
                        uchar *row_r = out->data(y);
                        for (int x = 0; x < out->m_width; ++x)
                        {
                                double xr = x;
                                double yr = y;
                                if (true)
                                {
                                        xr -= hwr;
                                        yr -= hhr;
                                }

                                // x-y calculation with formula
                                double x_result = xr * matrixA[0] + yr * matrixA[2] + matrixT[0];
                                double y_result = xr * matrixA[1] + yr * matrixA[3] + matrixT[1];

                                if (true)
                                {
                                        x_result += hw;
                                        y_result += hh;
                                }

                                // do nearest neighbor
                                int xi = round(x_result);
                                int yi = round(y_result);

                                //bilinear sampling : a-b values in the formula
                                int x_floor = floor(x_result);
                                int y_floor = floor(y_result);

                                uchar value = 0; // affine transform value

                                if (xi >= 0 && yi >= 0 && xi < m_width && yi < m_height)
                                {
                                        if (!bilinear)
                                        {
                                                value = m_data[yi * m_step + xi]; // affine transform value
                                        }
                                        else
                                        {
                                                double xVal = x_result - x_floor;
                                                double yVal = y_result - y_floor;
                                                // affine transform value according to bilinear formula
                                                value = ((1 - xVal) * (1 - yVal) * m_data[y_floor * m_width + x_floor]) + (xVal * (1 - yVal) * m_data[y_floor * m_width + x_floor + 1]) + ((1 - xVal) * yVal * m_data[(y_floor + 1) * m_width + x_floor]) + (xVal * yVal * m_data[(y_floor + 1) * m_width + x_floor + 1]);
                                        }
                                }

                                row_r[x] = value; //assign the value to the out image data
                        }
                }

                string output_filename = "/tmp/output_warp_affine.png";
                out->save_as_png(output_filename);

                clog << "warp affine function output :" << output_filename << endl;
        }

} // namespace ceng391
