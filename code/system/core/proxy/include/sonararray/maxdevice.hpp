/**
 * Copyright (C) 2015 Chalmers REVERE
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

#ifndef SONARARRAY_GPIODEVICE_HPP_
#define SONARARRAY_GPIODEVICE_HPP_

#include <cstdint>
#include <iostream>
#include <string>

#include "sonararray/device.hpp"

namespace opendlv {
namespace proxy {
namespace sonararray {

class MaxDevice : public Device {
 public:
  MaxDevice(std::vector<opendlv::model::Cartesian3>, 
      std::vector<opendlv::model::Direction>, std::vector<uint16_t>, 
      float);
  MaxDevice(MaxDevice const &) = delete;
  MaxDevice &operator=(MaxDevice const &) = delete;
  virtual ~MaxDevice();
  std::vector<float> GetSonarReadings();

 private:
  std::vector<uint16_t> m_pins;
  float m_scaleValue;
};

} // sonararray
} // proxy
} // opendlv

#endif
