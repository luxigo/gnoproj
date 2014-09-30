/*
 * distortion - optical distortion removal
 *
 * Copyright (c) 2013-2014 FOXEL SA - http://foxel.ch
 * Please read <http://foxel.ch/license> for more information.
 *
 *
 * Author(s):
 *
 *      Stéphane Flotron <s.flotron@foxel.ch>
 *
 *
 * This file is part of the FOXEL project <http://foxel.ch>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Additional Terms:
 *
 *      You are required to preserve legal notices and author attributions in
 *      that material or in the Appropriate Legal Notices displayed by works
 *      containing it.
 *
 *      You are required to attribute the work as explained in the "Usage and
 *      Attribution" section of <http://foxel.ch/license>.
 */


#include "gnoproj.hpp"

using namespace std;
using namespace cv;
using namespace elphelphg;

int main(int argc, char** argv) {

    /* Usage branch */
    if ( argc<3 || argc>4 || !strcmp( argv[1], "help" ) || !strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")  ) {
        /* Display help */
        printf( "Usage : %s <imagej_prefs_xml> <input_image> [ <output_image> ]\n\n",argv[0]);
        return 1;
    }

    char *imagej_prefs_xml=argv[1];
    char *input_image_filename=argv[2]; 
    std::string output_image_filename((argc==4)?argv[3]:"");

    // set some output parameters
     cout.setf(ios::left);
     cout.setf(ios::scientific);

    try {

      // get channel number from file name
      struct utils::imagefile_info *info=utils::imagefile_parsename(input_image_filename);
      int channel_index=atoi(info->channel);

      // accessing calibration data
      CameraArray e4pi(CameraArray::EYESIS4PI_CAMERA,imagej_prefs_xml);
      Channel *channel=e4pi.channel(channel_index);
      EqrData *eqr=channel->eqr;
      SensorData *sensor=channel->sensor;

      // load image
      printf("load image\n");
      IplImage* eqr_img = cvLoadImage(input_image_filename, CV_LOAD_IMAGE_COLOR );
      printf("done\n");

       /* Initialize output image structure */
       IplImage* out_img = cvCreateImage( cvSize( channel->sensor->pixelCorrectionWidth, channel->sensor->pixelCorrectionHeight ), IPL_DEPTH_8U , eqr_img->nChannels );

      /* Gnomonic projection of the equirectangular tile */
      lg_ttg_uc(

          ( inter_C8_t *) eqr_img->imageData,
          eqr_img->width, 
          eqr_img->height, 
          eqr_img->nChannels, 
          ( inter_C8_t *) out_img->imageData,
          out_img->width, 
          out_img->height,
          out_img->nChannels,
          sensor->px0,
          sensor->py0,
          eqr->imageFullWidth,
          eqr->imageFullLength-1, // there's an extra pixel for wrapping
          eqr->xPosition,
          eqr->yPosition,
          eqr->xPosition+eqr->x0,
          eqr->yPosition+eqr->y0,
          rad(sensor->roll),
          rad(sensor->azimuth),
          rad(sensor->elevation),
          rad(sensor->heading),
          0.001*sensor->pixelSize,
          sensor->focalLength,
          li_bilinearf
      );
     
      // generate output file name
      if (!output_image_filename.length()) {
        output_image_filename+=std::string(info->dir)+"/"+info->timestamp+"-"+info->channel+"-"+info->attributes+"_GNO."+info->extension;
      }

      /* Gnomonic image exportation */
      cvSaveImage(output_image_filename.c_str() , out_img, NULL );

    } catch(std::exception &e) {
      std::cerr << e.what() << std::endl;
      return 1;
    } catch(std::string &msg) {
      std::cerr << msg << std::endl;
      return 1;
    } catch(...) {
      std::cerr << "unhandled exception\n";
      return 1;
    }

    return 0;
}
