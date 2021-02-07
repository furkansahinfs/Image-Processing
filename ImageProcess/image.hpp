
#include <string>
#include <vector>

using std::vector;
namespace ceng391 {

typedef unsigned char uchar;

class Image {
public:
        Image(int width, int height, int n_channels, int step = 0);
        ~Image();

        static Image *new_gray(int width, int height);
        static Image *from_pnm(const std::string &filename);
        static Image *from_png(const std::string &filename, bool load_as_grayscale);

        int w() const { return m_width; }
        int h() const { return m_height; }
        int n_ch() const { return m_n_channels; }

        uchar *data() { return m_data; }
        const uchar *data() const { return m_data; }
        uchar *data(int y) { return m_data + y * m_step; }
        const uchar *data(int y) const { return m_data + y * m_step; }

        void set_rect(int tlx, int tly, int width, int height, uchar value);
        void set(uchar value) { set_rect(0, 0, m_width, m_height, value); }
        void set_zero() { set(0); }

        Image read_pnm(const std::string &filename);
        void save_as_pnm(const std::string &filename);
        bool save_as_png(const std::string &filename);
        bool load_png(const std::string &filename, bool load_as_grayscale);
        
        Image to_grayscale();
        Image to_rgba();
        Image *filter_2d(int n,  vector<vector<float>> kernel);
        short *deriv_x();
        short *deriv_y();
        void warp_affine(vector<float> matrixA,vector<float> matrixT, Image *out, bool bilinear);
        
       
       
private:
        int m_width;
        int m_height;
        int m_n_channels;
        int m_step;
        uchar *m_data;
};

}
