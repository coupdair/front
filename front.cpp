/*

*/

#include <iostream>
#include <string>
#include "CImg.h"
using namespace cimg_library;

//! detection of position from binary image
/**
 *  position is right tail of line (i.e. last none zero value position in row)
 *  \param img_bin: binary image (x,y,t) or (x,t)
 *  \return position image (y,t) or (t) as interger position
 *  \note can be used either for (x,y,t) or (x,t) binary images.
**/
CImg<int> binary_position(CImg<int> &img_bin)
{
  CImg<int> xposition(img_bin.height(),img_bin.depth());
  cimg_forXY(xposition,y,t)
  {
    //get single line
    const CImg<int> row=img_bin.get_shared_row(y,t);
    //search for last maximum position
    int xpos=-1;
    cimg_forX(row,x)
    {
      if(row(x)>0) xpos=x;
    }//x loop
    xposition(y,t)=xpos;
  }//(y and) time loop
  return xposition;
}//binary_position

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
       old: ./front -t 0.3 -i image_xt.cimg -o graph_xpVSt.cimg -X false #front.v0.0.1\n \
       ./front -at 12.3 -rt 12.0 \
-i volume_xyt.cimg -o graph_xpVSt.cimg \
-ts 0.123 -tu 0.001 -ta time_axis.cimg \
-d volume_xyt.posx.cimg -X true -t0 395 -t1 410 #analysis with display and result position on image \n \
       ./front -X false #quiet analysis \n \
version: "+std::string(VERSION)+"\n compilation date: " \
            ).c_str());//cimg_usage
  std::string  input_file_name=cimg_option("-i","volume.cimg","[in]  volume.");
  std::string output_file_name=cimg_option("-o","graph.cimg" ,"[out] graph.");
  std::string vutput_file_name=cimg_option("-d","volumeWithPos.cimg","[out] volume with detected position.");
  float  ts=cimg_option("-ts",0.1328,"time start (s).");
  float fps=cimg_option("-tu",5000,  "time unit in image/second (fps).");
  std::string tutput_file_name=cimg_option("-ta","time.cimg" ,"[out] time axis.");
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
  float binary_threshold=cimg_option("-at",8.0,"average binary threshold for input image (e.g. 0.3)");
  float raw_binary_threshold=cimg_option("-rt",5.0,"raw binary threshold for input image (e.g. 5.0)");
  ///time for show (extract in future ?)
  int t0=cimg_option("-t0",395,"time range: first time index for showing position detection (i.e. z in CImg volume)");
  int t1=cimg_option("-t1",410,"time range: last  time index");
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

//image statistics//
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

//x position of average along y//
  ///averaging along y
  CImg<float> img_avg(img_src.width(),img_src.depth());
  img_avg.fill(0.0);
  cimg_forXYZ(img_src,x,y,t)
  {
    img_avg(x,t)+=img_src(x,y,t);
  }
  img_avg/=img_src.height();
  display_print(img_avg,show,output_file_name);

  ///binarisation with fixed threshold
  CImg<int> img_bin=img_avg.get_threshold(binary_threshold);
  display_print(img_bin,show,output_file_name);
  ///position detection
  CImg<int> xpositionAF=binary_position(img_bin);//Fixed Threshold

  ///binarisation with dynamic threshold
//! \todo _ setup threshold for each row using (max-min)*percent_threshold then get_threshold(threshold)
  int threshold=binary_threshold;
  img_bin=img_avg.get_threshold(threshold);
  display_print(img_bin,show,output_file_name);
  ///position detection
  CImg<int> xpositionAD=binary_position(img_bin);//Dynamic Threshold

  if(show) xpositionAF.display_graph("xpositionAF");
  /*else*/ xpositionAF.print("Fixed Threshold position vs time");
  if(show) xpositionAD.display_graph("xpositionAD");
  /*else*/ xpositionAD.print("Dynamic Threshold position vs time");

//x position base on PDF//
  ///binarisation
  img_bin=img_src.get_threshold(raw_binary_threshold);
  display_print(img_bin,show,output_file_name);
  ///position detection
  CImg<int> xpositionYT=binary_position(img_bin);

  if(show) xpositionYT.display("xpositionYT");
  else xpositionYT.print("position(y,time)");

  ///PDF
  //! \todo . PDF for each t
  CImg<int> xpositionHA(img_src.depth());//x position based on average of x position (histogram)
  CImg<int> xpositionHisto((img_src.width()-1)/1,img_src.depth());
  CImg<int> xpositionHM(img_src.depth());//x position based on maximum of x position histogram
  cimg_forY(xpositionYT,t)
  {
    //! \todo v max histogram x position
    const CImg<float> row=xpositionYT.get_shared_row(t);
//row.print("x position vs y");
    const CImg<int> histo=row.get_histogram(xpositionHisto.width(),0,img_src.width()-2);
    xpositionHisto.draw_image(0,t,histo);
    if(histo.max()>0)
    {
      //search for last maximum position
      int xpos=-1;
      int max=-1;
      cimg_forX(histo,x)
      {
        if(histo(x)>max) {xpos=x;max=histo(x);}
      }//class loop
      //! \bug in case of class width!=1
      xpositionHM(t)=xpos;
    } else xpositionHM(t)=-1;
//histo.print("x position histogram");
    //! \todo max position
    //! \todo median x position
    //average x position
    xpositionHA(t)=0;int count=0;
    cimg_forX(xpositionYT,y)
    {
      if((xpositionYT(y,t)>-1)&&(xpositionYT(y,t)<img_src.depth()-1))
      {
        xpositionHA(t)+=xpositionYT(y,t);
        ++count;
      }//valid position
    }//y loop
    if(count>0) xpositionHA(t)/=count; else xpositionHA(t)=-1;
    //show histogram for time selection
/**/
    if((t>t0)&&(t<t1))
    {
      CImg<float> histo=xpositionHisto.get_shared_row(t);
histo.print("x position histogram");
      histo.display_graph("PDF");
    }//show selection
/**/
  }//time loop

  if(show) xpositionHM.display_graph("xpositionHM");
  /*else*/ xpositionHM.print("x position vs time");

  if(show) xpositionHA.display_graph("xpositionHA");
  /*else*/ xpositionHA.print("x position vs time");

  //show several position detection results
  CImg<int> xposition(xpositionAF.width(),1,1,4);
  xposition.draw_image(0,0,0,0,xpositionAF);
  xposition.draw_image(0,0,0,1,xpositionAD);
  xposition.draw_image(0,0,0,2,xpositionHA);
  xposition.draw_image(0,0,0,3,xpositionHM);
  xposition.display_graph("x position detections");

  xposition.save(output_file_name.c_str());

  //show position
    //load volume
    if(xposition.width()!=img_vol.depth())
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
      img_vol.draw_line(xpositionAF(t),y0,t,xpositionAF(t),y1,t,max);
      //show part of volume only
  if(show)
  {
      if((t>t0)&&(t<t1))
      {
/*
        CImgList<int> list(2);
        list(0)=img_vol.get_shared_slice(t);
        list(1)=img_vol.get_shared_slice(t-1);//! \todo redraw line with local max
        CImg<float> disp=list.get_append('y');
*/
/*
        CImg<int> disp=img_vol.get_shared_slice(t);
        disp.display("plane");
*/
        CImg<int> disp=img_src.get_slice(t);
        const int lmax[1]={disp.max()};
        const int lmaxL[1]={(int)((float)disp.max()*0.8f)};
        disp.draw_line(xpositionAF(t),y0,xpositionAF(t),y1,lmax);  //draw position of average along y
        disp.draw_line(xpositionHA(t),y0,xpositionHA(t),y1,lmaxL); //draw average position
        cimg_forY(disp,y) disp.draw_point(xpositionYT(y,t),y,lmax);//draw positions
        disp.display("plane");
      }//show selection
  }//volume show
    }//time loop
    //save
    img_vol.save(vutput_file_name.c_str());

  {//time axis
  CImg<float> time(img_src.depth());
  float tim=ts;
  float tstep=1/fps;
  cimg_forX(time,t) time(t)=tim+tstep*t;
  time.save(tutput_file_name.c_str());
  }//time axis

  std::exit(0);
  return 0;
}//main

