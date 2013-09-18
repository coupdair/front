/*

*/

#include <iostream>
#include <string>
#include "CImg.h"
using namespace cimg_library;

//! either display or/and print based on display at compilation or running time.
template<typename T>
void display_print(CImg<T> &img,bool show,std::string &title)
{
#if cimg_display!=0
  if(show) img.display(title.c_str());
  else img.print(title.c_str());
#else
  img.print(title.c_str());
#endif
}//display_print

int main(int argc,char **argv)
{
  //option{
  ///usage
  cimg_usage(std::string("front position detection program of the Laboratoire de PhysicoChimie des Processus de Combustion et de l'Atmosphere (PC2A), \
it uses different GNU libraries (see --info option)\n\n \
usage: ./front -h -I\n \
       ./front -t 0.3 -i image_xt.cimg -o graph_xpVSt.cimg -X false #quiet analysis \n \
       ./front -t 0.3 -i volume_xyt.cimg  -o graph_xpVSt.cimg -X true  #analysis with display and result position on image \n \
version: "+std::string(VERSION)+"\n compilation date: " \
            ).c_str());//cimg_usage
  std::string  input_file_name=cimg_option("-i","volume.cimg","[in]  volume.");
  std::string output_file_name=cimg_option("-o","graph.cimg" ,"[out] graph.");
  std::string vutput_file_name=cimg_option("-d","volumeWithPos.cimg","[out] volume with detected position.");
  ///standard GNU/CImg program options
  const bool show_h   =cimg_option("-h",    false,NULL);//-h hidden option
        bool show_help=cimg_option("--help",show_h,"help (or -h option)");show_help=show_h|show_help;//same --help or -h option
  bool show_info=cimg_option("-I",false,NULL);//-I hidden option
  if( cimg_option("--info",show_info,"show compilation options (or -I option)") ) {show_info=true;cimg_library::cimg::info();}//same --info or -I option
#if cimg_display!=0
  const bool showX=cimg_option("-X",false,NULL);//-X hidden option
  bool show=cimg_option("--show",show_h,"show GUI (or -X option)");show=showX|show;//same --show or -X option
#endif
  ///threshold
  float binary_threshold=cimg_option("-t",0.3,"binary threshold for input image (e.g. 0.3)");
  ///time for show (extractin future ?)
  int t0=cimg_option("-t0",165,"time range: first time index for showing position detection (i.e. z in CImg volume)");
  int t1=cimg_option("-t1",180,"time range: last  time index");
  ///stop help request
  if(show_help) {/*print_help(std::cerr);*/return 0;}
  //}option

  CImg<float> img_src(input_file_name.c_str());
  //copy volume for detection position show
  CImg<int> img_vol;
  if(show) img_vol=img_src;
  display_print(img_src,show,input_file_name);

  //check time indexes
  ///lowest limit
  if(t0<0)  t0=0;
  if(t1<t0) t1=t0;
  ///upper limit
  if(t1>img_src.depth()-1)  t1=img_src.depth()-1;
  if(t1<t0) t0=t1;

  ///profile
  CImg<float> profile(img_src.depth(),1,1,5);
  cimg_forZ(img_src,t)
  {
    //get single image
    const CImg<float> img=img_src.get_shared_slice(t);
    //search for last maximum position
    profile(t,0,0,0)=img.min();//min
    profile(t,0,0,1)=img.max();//max
    profile(t,0,0,2)=img.mean();//mean
  }//time
  //display
  profile.display_graph("min,max,mean v.s. time");

  ///grad
  cimg_for_inZ(img_src,0,profile.width()-2,t)
  {
    profile(t,0,0,3)=(profile(t,0,0,1)==0)?0:profile(t+1,0,0,1)/profile(t,0,0,1);
    profile(t,0,0,4)=(profile(t,0,0,2)==0)?0:profile(t+1,0,0,2)/profile(t,0,0,2);
  }
  //display
  (profile.get_shared_channels(3,4)).display_graph("max,mean gradients v.s. time e.g. g=f(t)/f(t+1)");

  ///averaging along y
  CImg<float> img_avg(img_src.width(),img_src.depth());
  img_avg.fill(0.0);
  cimg_forXYZ(img_src,x,y,t)
  {
    img_avg(x,t)+=img_src(x,y,t);
  }
  img_avg/=img_src.height();
  display_print(img_avg,show,output_file_name);

  ///binarisation
  CImg<int> img_bin=img_avg.get_threshold(binary_threshold);
  display_print(img_bin,show,output_file_name);

  ///position detection
  CImg<int> position(img_bin.height());
  cimg_forX(position,t)
  {
    //get single line
    const CImg<int> row=img_bin.get_shared_row(t);
    //search for last maximum position
    int xpos=-1;
    cimg_forX(row,x)
    {
      if(row(x)>0) xpos=x;
    }//x loop
    position(t)=xpos;
  }//time loop

  if(show) position.display_graph("position");
  else position.print("position vs time");

  position.save(output_file_name.c_str());

  //show position
  if(show)
  {
    //load volume
    if(position.width()!=img_vol.depth())
    {
      std::cerr<<"error: img/vol dimension.\n";
      std::exit(1);
      return 1;
    }
    //modify volume by drawing a line for each time (i.e. z) at x position (i.e. position(t))
    const int max[1]={img_vol.max()};
    int y0=0;
    int y1=img_vol.height()-1;
    cimg_forZ(img_vol,t)
    {
      //draw single line
      img_vol.draw_line(position(t),y0,t,position(t),y1,t,max);
      //show part of volume only
      if((t>t0)&&(t<t1)) (img_vol.get_shared_slice(t)).display("plane");
    }
    //save
    img_vol.save(vutput_file_name.c_str());
  }//volume show

  std::exit(0);
  return 0;
}//main

