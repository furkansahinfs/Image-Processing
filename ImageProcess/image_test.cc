// Furkan Şahin - 250201042 - CENG391 HW1

#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include "image.hpp"

using std::cerr;
using std::clog;
using std::endl;
using std::ofstream;
using std::string;
using std::vector;

using ceng391::Image;
using ceng391::uchar;

// Ideally these test should return test/fail status. Better yet use a proper
// unit test framework such as gtest.

void createImage(int channel);
void filter_2d();
void derivative();
void warp_affine();

void createImage(int channel)
{
     Image *img = new Image(200, 200, channel);

     clog << "Created new image of size " << img->w()
          << "x" << img->h() << " with " << img->n_ch() << endl;

     img->set_zero();
     img->set_rect(75, 75, 50, 50, 255);

     img->save_as_pnm("test_channel_" + std::to_string(channel));

     /*
        To_rgba function
        img->to_rgba() returns a new Image object which is ppm RGBA 4 channel image.
        to save it just code like that :
        img->to_rgba().save_as_pnm("rgba");
        */
     clog << "\n"
          << endl;
     if (channel == 1)
     {
          img->to_rgba().save_as_pnm("test_channel_" + std::to_string(channel) + "_to_rgba");
     }

     /*
        To_grayscale function
        img->to_grayscale() returns a new Image object which is pgm grayscale 1 channel image.
        to save it just code like that :
        img->to_grayscael().save_as_pnm("grayscale");
        */
     else
     {
          img->to_grayscale().save_as_pnm("test_channel_" + std::to_string(channel) + "_to_grayscale");
     }

     delete img;
}

Image readImage(const std::string &filename)
{
     /*
        To reach read_pnm function, I create an empty image and I invoke the read_pnm function of the empty image with given filename
        read_pnm find and read file in the function and return a new Image object
        */
     Image *img = new Image(0, 0, 0);
     return img->read_pnm(filename);
}

void filter_2d()
{

     clog << "filter_2d() function"
          << endl;

     string filename = "../data/small_city.png"; //Image that is read.

     Image *img = Image::from_png(filename, false);

     clog << "Loaded " << img->w() << "x" << img->h()
          << " image from " << filename << endl;

     //matrix n, you can change the n according to matrix
     int n = 3;

     //filter matrix, you can change the matrix
     vector<vector<float>> vect = {
         {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}}};

     img = img->filter_2d(n, vect);

     string output_filename = "/tmp/output_filtered.png"; //output image name.
     img->save_as_png(output_filename);

     clog << "filter_2d() function output :" << output_filename << endl;

     delete img;
}

void derivative()
{
     clog << "\nderivative functions"
          << endl;

     string filename = "../data/small_city.png"; //Image that is read.

     Image *img = Image::from_png(filename, false);

     clog << "Loaded " << img->w() << "x" << img->h()
          << " image from " << filename << endl;

     short *deriv_x = img->deriv_x();

     //writing short array to the file
     ofstream myfileX("/tmp/deriv_x_output.txt");
     if (myfileX.is_open())
     {
          for (int count = 0; count < img->w() * img->h(); count++)
          {
               myfileX << deriv_x[count] << " ";
          }
          myfileX.close();
     }
     clog << "Short Array of Derivative X Output File : /tmp/deriv_x_output.txt" << endl;

     short *deriv_y = img->deriv_y();

     //writing short array to the file
     ofstream myfileY("/tmp/deriv_y_output.txt");
     if (myfileY.is_open())
     {

          for (int count = 0; count < img->w() * img->h(); count++)
          {
               myfileY << deriv_y[count] << " ";
          }
          myfileY.close();
     }
     clog << "Short Array of Derivative Y Output File : /tmp/deriv_y_output.txt" << endl;

     delete img;
}

void warp_affine()
{

     clog << "\nwarp affine function"
          << endl;

     string filename = "../data/small_city.png"; //Image that is read.

     Image *img = Image::from_png(filename, false);

     clog << "Loaded " << img->w() << "x" << img->h()
          << " image from " << filename << endl;

     //out image creation, according to transform matrix, you can change the size of the image to fit the image to the viewer.
     // for example w()*3, h()*3
     Image *out = new Image(img->w() * 2, img->h() * 2, 1);

     //Matrix A, you can change the matrix according to this definition : {a11,a12,a21,a22}
     vector<float> matrixA =
         {1, 1, 0, 1};

     //Matrix T, you can change the matrix according to this definition  : {t1,t2}
     vector<float> matrixT =
         {0, 0};

     // To do bilinear sampling, make the boolean true.
     bool bilinear = true;
     img->warp_affine(matrixA, matrixT, out, bilinear);

     delete img;
     delete out;
}

int main(int argc, char **argv)
{    
     int channel = 1; // 1 or 3 or 4
     createImage(channel);
     filter_2d();
     derivative();
     warp_affine();

     return EXIT_SUCCESS;
}
