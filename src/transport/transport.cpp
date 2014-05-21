/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013-2014 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * A copy of the GNU General Public License is in the file COPYING.
 */

#include <stdexcept>
#include <ndn-cpp/transport/transport.hpp>

using namespace std;

namespace ndn {

Transport::ConnectionInfo::~ConnectionInfo()
{  
}

void 
Transport::connect(const Transport::ConnectionInfo& connectionInfo, ElementListener& elementListener) 
{
  throw logic_error("unimplemented");
}
  
void 
Transport::send(const uint8_t *data, size_t dataLength)
{
  throw logic_error("unimplemented");
}

void 
Transport::processEvents()
{
  throw logic_error("unimplemented");
}

bool 
Transport::getIsConnected()
{
  throw logic_error("unimplemented");
}

void 
Transport::close()
{
}

Transport::~Transport()
{
}

}
