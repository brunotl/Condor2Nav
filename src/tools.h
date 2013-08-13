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
 * @file src/tools.h
 *
 * @brief Declares common tools. 
 */

#ifndef __TOOLS_H__
#define __TOOLS_H__

#include "exception.h"
#include <sstream>
#include <vector>
#include <Windows.h>

namespace boost {
  namespace filesystem {
    class path;
  }
}

namespace condor2nav {

  /**
   * @brief Pointer version of std::less
   *
   * Pointer version of std::less used for STL containers sorting.
   */
  struct CPtrCmp {
    template<class T>
    bool operator()(T v1, T v2) const
    {
      return *v1 < *v2;
    }
  };

  /**
   * @brief Deleter for HModule
   */
  struct CHModuleDeleter {
    typedef HMODULE pointer;
    void operator()(HMODULE hmod) const { ::FreeLibrary(hmod); }
  };


  // conversions
  template<class T> T Convert(const std::string &str);
  template<class T> std::string Convert(const T &val);

  void Trim(std::string &str);
  std::pair<std::string, std::string> LineParseKeyValue(const std::string &line);
  std::vector<std::string> LineParseCSV(const std::string &line);

  struct TLongitude {
    static const unsigned degStrLength = 3;
    double value;
    explicit TLongitude(double v) : value{v} {}
    char Sign() const { return value > 0 ? 'E' : 'W'; }
  };

  struct TLatitude {
    static const unsigned degStrLength = 2;
    double value;
    explicit TLatitude(double v) : value{v} {}
    char Sign() const { return value > 0 ? 'N' : 'S'; }
  };

  std::string Coord2DDMMFF(TLongitude coord);
  std::string Coord2DDMMFF(TLatitude coord);
  std::string Coord2DDMMSS(TLongitude coord);
  std::string Coord2DDMMSS(TLatitude coord);

  bool InsideArea(TLongitude outerLonMin, TLongitude outerLonMax, TLatitude outerLatMin, TLatitude outerLatMax,
                  TLongitude innerLonMin, TLongitude innerLonMax, TLatitude innerLatMin, TLatitude innerLatMax);

  unsigned KmH2MS(unsigned value);

  double Deg2Rad(double angle);
  double Rad2Deg(double angle);

  // disk operations
  void DirectoryCreate(const boost::filesystem::path &dirName);
  bool FileExists(const boost::filesystem::path &fileName);
  void Download(const std::string &server, const boost::filesystem::path &url, const boost::filesystem::path &fileName, unsigned timeout = 30);

}


/**
 * @brief Converts string to specific type.
 *
 * Function converts provided string to specified type. The convertion
 * is being done using STL streams so the output type have to provide
 * the means to initialize itself from the stream.
 *
 * @param str The string to convert.
 *
 * @exception std Thrown when operation failed.
 *
 * @return The data of specified type.
 */
template<class T>
T condor2nav::Convert(const std::string &str)
{
  T value;
  std::stringstream stream(str);
  stream >> value;
  if(stream.fail() && !stream.eof())
    throw EOperationFailed{"Cannot convert '" + str + "' to requested type!!!"};
  return value;
}


/**
 * @brief Converts any type to a string.
 *
 * Function converts provided data to a string. The convertion
 * is being done using STL streams so the input type have to provide
 * the means to convert itself into the stream.
 *
 * @param val The value to convert.
 *
 * @exception std Thrown when operation failed.
 *
 * @return The string describing provided data.
 */
template<class T>
std::string condor2nav::Convert(const T &val)
{
  std::stringstream stream;
  stream << val;
  return stream.str();
}


#endif /* __TOOLS_H__ */
