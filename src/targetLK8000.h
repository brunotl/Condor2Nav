//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009 Mateusz Pusz
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
 * @file targetLK8000.h
 *
 * @brief Declares the condor2nav::CTargetLK8000 class. 
**/

#ifndef __TARGET_LK8000_H__
#define __TARGET_LK8000_H__

#include "targetXCSoarCommon.h"


namespace condor2nav {

  /**
   * @brief Translator to LK8000 data format.
   *
   * condor2nav::CTranslatorLK8000 class is responsible for Condor data translation
   * to LK8000 (http://www.bware.it/xcsoar) format.
  **/
  class CTargetLK8000 : public CTargetXCSoarCommon {
    // dirs
    static const char *AIRSPACES_SUBDIR;         ///< @brief LK8000 airspaces subdirectory. 
    static const char *CONFIG_SUBDIR;            ///< @brief LK8000 configuration subdirectory. 
    static const char *MAPS_SUBDIR;              ///< @brief LK8000 maps subdirectory. 
    static const char *POLARS_SUBDIR;            ///< @brief LK8000 polars subdirectory. 
    static const char *TASKS_SUBDIR;             ///< @brief LK8000 tasks subdirectory. 

    // inputs
    static const char *LK8000_PROFILE_NAME;	     ///< @brief LK8000 profile file name to use for input. 

    std::auto_ptr<CFileParserINI> _profileParser;///< @brief LK8000 profile file parser. 
    const std::string _outputLK8000DataPath;     ///< @brief The path to the output LK8000 directory.
    std::string _condor2navDataPath;             ///< @brief The Condor2Nav destination data directory path (in LK8000 format) on the target device that runs LK8000.

    std::string _outputAirspacesSubDir;          ///< @brief The subdirectory where output LK8000 airspaces file should be located
    std::string _outputConfigSubDir;             ///< @brief The subdirectory where Condor maps files should be located
    std::string _outputMapsSubDir;               ///< @brief The subdirectory where output LK8000 airspaces file should be located
    std::string _outputPolarsSubDir;             ///< @brief The subdirectory where output LK8000 polars file should be located
    std::string _outputTaskFilePath;             ///< @brief The path where output LK8000 task file should be located
    
  public:
    explicit CTargetLK8000(const CTranslator &translator);
    virtual ~CTargetLK8000();

    virtual void SceneryMap(const CFileParserCSV::CStringArray &sceneryData);
    virtual void SceneryTime();
    virtual void Glider(const CFileParserCSV::CStringArray &gliderData);
    virtual void Task(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv);
    virtual void PenaltyZones(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv);
    virtual void Weather(const CFileParserINI &taskParser);
  };

}

#endif /* __TARGET_LK8000_H__ */
