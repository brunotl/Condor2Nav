//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009-2012 Mateusz Pusz
//
// Condor2Nav is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Condor2Nav is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Condor2Nav. If not, see <http://www.gnu.org/licenses/>.
//
// Visit the project webpage (http://sf.net/projects/condor2nav) for more info.
//

/**
 * @file istream.h
 *
 * @brief Declares the IStream wrapper class. 
 */

#ifndef __ISTREAM_H__
#define __ISTREAM_H__

#include "nonCopyable.h"
#include "boostfwd.h"
#include <sstream>

namespace condor2nav {

  /**
   * @brief Input stream wrapper
   *
   * condor2nav::CIStream class is a wrapper for different stream types.
   */
  class CIStream : CNonCopyable {
    std::stringstream _buffer;            ///< @brief Buffer with file data. 
  public:
    explicit CIStream(const bfs::path &fileName);
    CIStream(const std::string &server, const bfs::path &url, unsigned timeout = 30);
    explicit operator bool() const           { return static_cast<bool>(_buffer); }
    std::istream &GetLine(std::string &line) { return getline(_buffer, line); }

    template<class Stream>
    friend Stream &operator<<(Stream &out, CIStream &in)
    {
      out << in._buffer.rdbuf();
      return out;
    }
  };

}

#endif /* __ISTREAM_H__ */
