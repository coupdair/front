/*

*/

#include <string>
#include "CImg.h"
using namespace cimg_library;

int main(int argc,char **argv)
{
  cimg_usage("front detection");
  std::string input_file_name=cimg_option("-i","data.cimg","Input data.");
  CImg<float> img_src(input_file_name.c_str());
  img_src.display(input_file_name.c_str());
  std::exit(0);
  return 0;
}

