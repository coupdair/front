/*

*/

#include <string>
#include "CImg.h"
using namespace cimg_library;

int main(int argc,char **argv)
{
  //option{
  ///usage
  cimg_usage(std::string("front position detection program of the Laboratoire de PhysicoChimie des Processus de Combustion et de l'Atmosphere (PC2A), \
it uses different GNU libraries (see --info option)\n\n \
usage: ./front -h -I\n \
       ./front -t 0.3 -i image_xt.cimg -o graph_xpVSt.cimg\n \
version: "+std::string(VERSION)+"\n compilation date: " \
            ).c_str());//cimg_usage
  std::string input_file_name=cimg_option("-i","data.cimg","Input data.");
  ///standard GNU/CImg program options
  const bool show_h   =cimg_option("-h",    false,NULL);//-h hidden option
        bool show_help=cimg_option("--help",show_h,"help (or -h option)");show_help=show_h|show_help;//same --help or -h option
  bool show_info=cimg_option("-I",false,NULL);//-I hidden option
  if( cimg_option("--info",show_info,"show compilation options (or -I option)") ) {show_info=true;cimg_library::cimg::info();}//same --info or -I option
#if cimg_display!=0
  const bool showX=cimg_option("-X",false,NULL);//-X hidden option
  bool show=cimg_option("--show",show_h,"show GUI (or -X option)");show=showX|show;//same --show or -X option
#endif
  ///stop help request
  if(show_help) {/*print_help(std::cerr);*/return 0;}
  //}option

  CImg<float> img_src(input_file_name.c_str());
  
#if cimg_display!=0
  if(show) img_src.display(input_file_name.c_str());
  else img_src.print(input_file_name.c_str());
#else
  img_src.print(input_file_name.c_str());
#endif

  std::exit(0);
  return 0;
}//main

