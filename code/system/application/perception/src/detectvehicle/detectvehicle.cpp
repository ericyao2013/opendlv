/**
 * Copyright (C) 2016 Chalmers REVERE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */


//---
#include <ctype.h>
#include <cstring>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"
#include "opendavinci/odcore/wrapper/SharedMemoryFactory.h"
#include "opendavinci/odcore/wrapper/SharedMemory.h"

#include "odvdopendlvdata/GeneratedHeaders_ODVDOpenDLVData.h"

#include "detectvehicle/detectvehicle.hpp"

namespace opendlv {
namespace perception {
namespace detectvehicle {


/**
  * Constructor.
  *
  * @param a_argc Number of command line arguments.
  * @param a_argv Command line arguments.
  */
DetectVehicle::DetectVehicle(int32_t const &a_argc, char **a_argv)
    : DataTriggeredConferenceClientModule(
      a_argc, a_argv, "perception-detectvehicle")
    , m_initialised(false)
    , m_debug()
{
}

DetectVehicle::~DetectVehicle()
{
}

void DetectVehicle::setUp()
{
  odcore::base::KeyValueConfiguration kv = getKeyValueConfiguration();
  m_debug = (kv.getValue<int32_t> ("perception-detectvehicle.debug") == 1);
  std::cout << "Setup complete." << std::endl;
  m_initialised = true;
}
void DetectVehicle::tearDown()
{
}
/**
 * Receives SharedImage from camera.
 * Sends .
 */
void DetectVehicle::nextContainer(odcore::data::Container &c)
{
  if (c.getDataType() != odcore::data::image::SharedImage::ID() || !m_initialised) {
    return;
  }


  odcore::data::image::SharedImage mySharedImg = c.getData<odcore::data::image::SharedImage>();


  std::string cameraName = mySharedImg.getName();
  // std::cout << "Received image from camera " << cameraName  << "!" << std::endl;

  //TODO compare the source name to keep track different camera sources

  std::shared_ptr<odcore::wrapper::SharedMemory> sharedMem(
      odcore::wrapper::SharedMemoryFactory::attachToSharedMemory(
          mySharedImg.getName()));
  
  const uint32_t nrChannels = mySharedImg.getBytesPerPixel();
  const uint32_t imgWidth = mySharedImg.getWidth();
  const uint32_t imgHeight = mySharedImg.getHeight();

  IplImage* myIplImage;
  myIplImage = cvCreateImage(cvSize(
      imgWidth, imgHeight), IPL_DEPTH_8U, nrChannels);
  cv::Mat myImage(myIplImage);

  if (!sharedMem->isValid()) {
    return;
  }
  
  {
    sharedMem->lock();
    memcpy(myImage.data, sharedMem->getSharedMemory(), 
        imgWidth*imgHeight*nrChannels);
    sharedMem->unlock();
  }

  cvReleaseImage(&myIplImage);
}



void DetectVehicle::sendObjectInformation()
{
  //TODO: refactoring


  /*
  for (uint32_t i=0; i<detections->size(); i++) {
    
    cv::Rect currentBoundingBox = detections->at(i);

    odcore::data::TimeStamp lastSeen = timeStampOfImage;
    
    std::string type = "vehicle";

    //Hardcoded confidence
    float typeConfidence = 0.5f;

    Eigen::Vector3d pointBottomLeft(
        currentBoundingBox.x, 
        currentBoundingBox.y + currentBoundingBox.height, 
        1);
    Eigen::Vector3d pointBottomRight(
        currentBoundingBox.x + currentBoundingBox.width, 
        currentBoundingBox.y + currentBoundingBox.height, 
        1);
    Eigen::Vector3d pointBottomMid(
        currentBoundingBox.x + currentBoundingBox.width / 2.0f, 
        currentBoundingBox.y + currentBoundingBox.height, 
        1);
    TransformPointToGlobalFrame(pointBottomLeft);
    TransformPointToGlobalFrame(pointBottomRight);
    TransformPointToGlobalFrame(pointBottomMid);

    float headingOfBottomLeft = std::atan2(pointBottomLeft(1), pointBottomLeft(0));
    float headingOfBottomRight = std::atan2(pointBottomRight(1), pointBottomRight(0));
    float angularSize = headingOfBottomLeft - headingOfBottomRight;
    //Hardcoded confidence
    float angularSizeConfidence = -1.0f;


    float azimuth = headingOfBottomRight + angularSize / 2.0f; //midpoint of box
    float zenith = 0;
    opendlv::model::Direction direction(azimuth, zenith);
    //Hardcoded confidence
    float directionConfidence = 0.1f;

    float azimuthRate = 0;
    float zenithRate = 0;
    opendlv::model::Direction directionRate(azimuthRate, zenithRate);
    float directionRateConfidence = -1.0f;


    float distance = static_cast<float>(sqrt(pow(pointBottomMid(0),2) + pow(pointBottomMid(1),2)));
    //Hardcoded confidence
    float distanceConfidence = 0.5f;


    float angularSizeRate = 0;
    float angularSizeRateConfidence = -1.0f;


    float confidence = 0.2f;
    std::vector<std::string> sources;
    sources.push_back(m_sourceName);

    std::vector<std::string> properties;

    uint16_t objectId = -1;

    float detectionWidth = static_cast<float>(
        sqrt(pow(pointBottomRight(0) - pointBottomLeft(0),2) + 
        pow(pointBottomRight(1) - pointBottomLeft(1),2)));

    //std::cout << "Object width: " << detectionWidth << " m" << std::endl;
    //Should detection sort be in the this module?
    if (angularSize < 0 
        || distance > 150 
        || detectionWidth < 0.5f 
        || detectionWidth > 10) {
      // Something fishy with this detection
      continue;
    }

    opendlv::perception::Object detectedObject(lastSeen, type, typeConfidence,
        direction, directionConfidence, directionRate, directionRateConfidence,
        distance, distanceConfidence, angularSize, angularSizeConfidence,
        angularSizeRate, angularSizeRateConfidence, confidence, sources,
        properties, objectId);
    odcore::data::Container objectContainer(detectedObject);
    getConference().send(objectContainer);

    std::cout << "Sending DetectedObject:" << std::endl;
    std::cout << "    type:          " << type << std::endl;
    std::cout << "    azimuth:       " << azimuth << std::endl;
    std::cout << "    azimuth (deg): " << (azimuth*static_cast<float>(opendlv::Constants::RAD2DEG)) << std::endl;
    std::cout << "    angularSize:   " << angularSize << std::endl;
    std::cout << "    angSize (deg): " << (angularSize*static_cast<float>(opendlv::Constants::RAD2DEG)) << std::endl;
    std::cout << "    distance (m):  " << distance << std::endl;
    std::cout << "    width (m):     " << detectionWidth << std::endl;

  }
  */
}

} // detectvehicle
} // perception
} // opendlv
