/*

*/

#include <string>
#include "CImg.h"
using namespace cimg_library;

int main(int argc,char **argv)
{
  //option{
  ///usage
  cimg_usage("front detection program of the Laboratoire de PhysicoChimie des Processus de Combustion et de l'Atmosphere (PC2A), \
it uses different GNU libraries (see --info option)\n\n \
usage: ./front -h -I\n \
       ./front -t 0.3 -i image.nc -o graph.cimg\n \
\ncompilation date: "
            );//cimg_usage
  std::string input_file_name=cimg_option("-i","data.cimg","Input data.");
  const bool show_h   =cimg_option("-h",    false,NULL);//-h hidden option
        bool show_help=cimg_option("--help",show_h,"help (or -h option)");show_help=show_h|show_help;//same --help or -h option
  bool show_info=cimg_option("-I",false,NULL);//-I hidden option
  if( cimg_option("--info",show_info,"show compilation options (or -I option)") ) {show_info=true;cimg_library::cimg::info();}//same --info or -I option
  //}option
  if(show_help) {/*print_help(std::cerr);*/return 0;}

  CImg<float> img_src(input_file_name.c_str());
  img_src.display(input_file_name.c_str());
  std::exit(0);
  return 0;
}

