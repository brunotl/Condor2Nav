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
 * @file targetXCSoar.cpp
 *
 * @brief Implements the condor2nav::CTargetXCSoar class. 
**/

#include "targetXCSoar.h"
#include "imports/xcsoarTypes.h"
#include <fstream>
#include <iostream>


const char *condor2nav::CTargetXCSoar::XCSOAR_PROFILE_NAME = "xcsoar-registry.prf";

const char *condor2nav::CTargetXCSoar::OUTPUT_PROFILE_NAME = "Condor.prf";
const char *condor2nav::CTargetXCSoar::WP_FILE_NAME = "WP_CondorTask.dat";
const char *condor2nav::CTargetXCSoar::POLAR_FILE_NAME = "Polar_Condor.plr";
const char *condor2nav::CTargetXCSoar::AIRSPACES_FILE_NAME = "A_Condor.txt";


/**
 * @brief Class constructor.
 *
 * condor2nav::CTargetXCSoar class constructor.
 *
 * @param translator Configuration INI file parser.
**/
condor2nav::CTargetXCSoar::CTargetXCSoar(const CTranslator &translator):
CTranslator::CTarget(translator),
_profileParser(CTranslator::DATA_PATH + std::string("\\") + XCSOAR_PROFILE_NAME),
_outputXCSoarDataPath(OutputPath() + "\\XCSoarData")
{
  std::string subDir = ConfigParser().Value("XCSoar", "Condor2NavDataSubDir");
  if(subDir != "")
    subDir = "\\" + subDir;
  _outputCondor2NavDataPath = _outputXCSoarDataPath + subDir;
  _condor2navDataPath = ConfigParser().Value("XCSoar", "XCSoarDataPath") + subDir;
  
  DirectoryCreate(_outputCondor2NavDataPath);
}


/**
 * @brief Class destructor.
 *
 * condor2nav::CTargetXCSoar class destructor.
**/
condor2nav::CTargetXCSoar::~CTargetXCSoar()
{
  _profileParser.Dump(_outputCondor2NavDataPath + std::string("\\") + OUTPUT_PROFILE_NAME);
}


/**
* @brief Sets scenery map. 
*
* Method sets scenery map XCM data file according to the Condor landscape name. 
*
* @param sceneryData Information describing the scenery. 
**/
void condor2nav::CTargetXCSoar::SceneryMap(const CFileParserCSV::CStringArray &sceneryData)
{
  _profileParser.Value("", "MapFile", "\"" + _condor2navDataPath + "\\" + sceneryData.at(SCENERY_XCSOAR_FILE) + "\"");
}


/**
* @brief Sets time for scenery time zone. 
*
* Method sets UTC time offset for selected scenery and forces time synchronization to the GPS source.
*
* @param sceneryData Information describing the scenery. 
**/
void condor2nav::CTargetXCSoar::SceneryTime(const CFileParserCSV::CStringArray &sceneryData)
{
  _profileParser.Value("", "UTCOffset", sceneryData.at(SCENERY_UTC_OFFSET));
  _profileParser.Value("", "SetSystemTimeFromGPS", "1");
}


/**
* @brief Set glider data. 
*
* Method created and sets glider polar file, handicap, safety speed, the time to empty the water
* ballast and glider name for the logger.
*
* @param gliderData Information describing the glider. 
**/
void condor2nav::CTargetXCSoar::Glider(const CFileParserCSV::CStringArray &gliderData)
{
  // set WinPilot Polar
  _profileParser.Value("", "Polar", "6");
  _profileParser.Value("", "PolarFile", "\"" + _condor2navDataPath + std::string("\\") + POLAR_FILE_NAME + std::string("\""));

  _profileParser.Value("", "AircraftType", "\"" + gliderData.at(GLIDER_NAME) + "\"");
  _profileParser.Value("", "SafteySpeed", Convert(KmH2MS(Convert<unsigned>(gliderData.at(GLIDER_SPEED_MAX)))));
  _profileParser.Value("", "Handicap", gliderData.at(GLIDER_DAEC_INDEX));
  _profileParser.Value("", "BallastSecsToEmpty", gliderData.at(GLIDER_WATER_BALLAST_EMPTY_TIME));

  // create polar file
  std::string polarFileName = _outputCondor2NavDataPath + std::string("\\") + POLAR_FILE_NAME;
  std::ofstream polarFile(polarFileName.c_str());
  if(!polarFile)
    throw std::runtime_error("ERROR: Couldn't open Polar file '" + polarFileName + "' for writing!!!");

  polarFile << "***************************************************************************************************" << std::endl;
  polarFile << "* " << gliderData.at(GLIDER_NAME) << " WinPilot POLAR file generated with Condor2Nav" << std::endl;
  polarFile << "*" << std::endl;
  polarFile << "* MassDryGross[kg], MaxWaterBallast[liters], Speed1[km/h], Sink1[m/s], Speed2, Sink2, Speed3, Sink3" << std::endl;
  polarFile << "***************************************************************************************************" << std::endl;
  for(unsigned i=GLIDER_MASS_DRY_GROSS; i<=GLIDER_SINK_3; i++) {
    if(i > GLIDER_MASS_DRY_GROSS)
      polarFile << ",";
    polarFile << gliderData.at(i);
  }
  polarFile << std::endl;
}


/**
* @brief Sets task information. 
*
* Method sets task information.
*
* @param taskParser Condor task parser. 
* @param coordConv  Condor coordinates converter.
**/
void condor2nav::CTargetXCSoar::Task(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv)
{
  using namespace xcsoar;

  // set task settings
  SETTINGS_TASK settingsTask;
  settingsTask.AutoAdvance           = static_cast<AutoAdvanceMode_t>(Convert<unsigned>(_profileParser.Value("", "AutoAdvance")));
  settingsTask.AATTaskLength         = 0;
  settingsTask.AATEnabled            = false;
  settingsTask.EnableMultipleStartPoints = false;
  settingsTask.EnableFAIFinishHeight = false;
  settingsTask.StartMaxHeightMargin  = 0;
  settingsTask.StartMaxSpeed         = 0;
  settingsTask.StartMaxSpeedMargin   = 0;
  settingsTask.StartHeightRef        = 0;

  TASK_POINT taskPointArray[MAXTASKPOINTS];
//  START_POINT startPointArray[MAXSTARTPOINTS];

  // reset all tasks
  for(unsigned i=0; i<MAXTASKPOINTS; i++)
    taskPointArray[i].Index = -1;


  _profileParser.Value("", "AdditionalWPFile", "\"" + _condor2navDataPath + std::string("\\") + WP_FILE_NAME + std::string("\""));
  std::string wpFileName = _outputCondor2NavDataPath + std::string("\\") + WP_FILE_NAME;
  std::ofstream wpFile(wpFileName.c_str());
  if(!wpFile)
    throw std::runtime_error("ERROR: Couldn't open file '" + wpFileName + "' for writing!!!");

  unsigned tpNum = condor2nav::Convert<unsigned>(taskParser.Value("Task", "Count"));
  // skip takeoff waypoint
  for(unsigned i=1; i<tpNum; i++){
    // dump WP file line
    std::string tpIdxStr(Convert(i));
    std::string x(taskParser.Value("Task", "TPPosX" + tpIdxStr));
    std::string y(taskParser.Value("Task", "TPPosY" + tpIdxStr));
    std::string name;
    if(i == 1)
      name = "START";
    else if(i == tpNum - 1)
      name = "FINISH";
    else
      name = "TP" + Convert(i - 1);
    wpFile << i << "," << coordConv.Latitude(x, y, CCondor::CCoordConverter::FORMAT_DDMMFF) << "," 
      << coordConv.Longitude(x, y, CCondor::CCoordConverter::FORMAT_DDMMFF) << ","
      << taskParser.Value("Task", "TPPosZ" + tpIdxStr) << "M,T," << name << ","
      << taskParser.Value("Task", "TPName" + tpIdxStr) << std::endl;

    // dump Task File data
    std::string sectorTypeStr(taskParser.Value("Task", "TPSectorType" + tpIdxStr));
    unsigned sectorType(Convert<unsigned>(sectorTypeStr));
    if(sectorType == CCondor::SECTOR_CLASSIC) {
      unsigned angle(condor2nav::Convert<unsigned>(taskParser.Value("Task", "TPAngle" + tpIdxStr)));
      switch(angle) {
      case 90:
        if(i == 1)
          settingsTask.StartType = xcsoar::START_SECTOR;
        else if(i == tpNum - 1)
          settingsTask.FinishType = xcsoar::FINISH_SECTOR;
        else
          settingsTask.SectorType = xcsoar::AST_FAI;
        break;

      case 180:
        if(i == 1)
          settingsTask.StartType = xcsoar::START_LINE;
        else if(i == tpNum - 1)
          settingsTask.FinishType = xcsoar::FINISH_LINE;
        else {
          std::cerr << "WARNING: " << name << ": XCSoar does not support line TP type. FAI Sector will be used instead. You may need to manualy advance a waypoint after reaching it in Condor." << std::endl;
          settingsTask.SectorType = xcsoar::AST_FAI;
        }
        break;

      case 270:
        std::cerr << "WARNING: " << name << ": XCSoar does not support TP with angle '270'. Circle sector will be used instead. Be carefull to advance a waypoint in Condor after it has been advanced by the XCSoar." << std::endl;
      case 360:
        if(i == 1)
          settingsTask.StartType = xcsoar::START_CIRCLE;
        else if(i == tpNum - 1)
          settingsTask.FinishType = xcsoar::FINISH_CIRCLE;
        else
          settingsTask.SectorType = xcsoar::AST_CIRCLE;
        break;

      default:
        ;
      }
      
      unsigned radius(condor2nav::Convert<unsigned>(taskParser.Value("Task", "TPRadius" + tpIdxStr)));
      if(i == 1) {
        settingsTask.StartRadius = radius;
        settingsTask.StartMaxHeight = condor2nav::Convert<unsigned>(taskParser.Value("Task", "TPHeight" + tpIdxStr));
      }
      else if(i == tpNum - 1) {
        settingsTask.FinishRadius = radius;
        settingsTask.FinishMinHeight = condor2nav::Convert<unsigned>(taskParser.Value("Task", "TPWidth" + tpIdxStr));
      }
      else
        settingsTask.SectorRadius = radius;
    }
    else if(sectorType == CCondor::SECTOR_WINDOW) {
      std::cerr << "WARNING: " << name << ": XCSoar does not support window TP type. Circle TP will be used and you are responsible for reaching it on correct height and with correct heading." << std::endl;
    }
    else
      throw std::range_error("ERROR: Unsupported sector type '" + sectorTypeStr + "' specified for TP '" + name + "'!!!");
  }

  // set profile parameters
  // HomeWaypoint

  _profileParser.Value("", "StartLine", Convert(settingsTask.StartType));
  _profileParser.Value("", "StartMaxHeight", Convert(settingsTask.StartMaxHeight));
  _profileParser.Value("", "StartRadius", Convert(settingsTask.StartRadius));

  _profileParser.Value("", "FAISector", Convert(settingsTask.SectorType));
  _profileParser.Value("", "Radius", Convert(settingsTask.SectorRadius));

  _profileParser.Value("", "FinishLine", Convert(settingsTask.FinishType));
  _profileParser.Value("", "FinishMinHeight", Convert(settingsTask.FinishMinHeight));
  _profileParser.Value("", "FinishRadius", Convert(settingsTask.FinishRadius));
}
  // set units
  // Speed
//  switch(Speed)
//    {
//    case 0 :
//      SPEEDMODIFY = TOMPH;
//      break;
//    case 1 :
//      SPEEDMODIFY = TOKNOTS;
//      break;
//    case 2 :
//      SPEEDMODIFY = TOKPH;
//      break;
//    }
//  // Distance
//  switch(Distance)
//    {
//    case 0 : DISTANCEMODIFY = TOMILES; break;
//    case 1 : DISTANCEMODIFY = TONAUTICALMILES; break;
//    case 2 : DISTANCEMODIFY = TOKILOMETER; break;
//    }
//
//  // Altitude
//  GetFromRegistryD(szRegistryAltitudeUnitsValue,Altitude);
//  switch(Altitude)
//    {
//    case 0 : ALTITUDEMODIFY = TOFEET; break;
//    case 1 : ALTITUDEMODIFY = TOMETER; break;
//    }
//
//
//    
//}
 

/**
* @brief Sets task penalty zones. 
*
* Method sets penalty zones used in the task.
*
* @param taskParser Condor task parser. 
* @param coordConv  Condor coordinates converter.
**/
void condor2nav::CTargetXCSoar::PenaltyZones(const CFileParserINI &taskParser, const CCondor::CCoordConverter &coordConv)
{
  unsigned pzNum = condor2nav::Convert<unsigned>(taskParser.Value("Task", "PZCount"));
  if(pzNum == 0)
    return;

  _profileParser.Value("", "AirspaceFile", "\"" + _condor2navDataPath + std::string("\\") + AIRSPACES_FILE_NAME + std::string("\""));
  std::string airspacesFileName = _outputCondor2NavDataPath + std::string("\\") + AIRSPACES_FILE_NAME;
  std::ofstream airspacesFile(airspacesFileName.c_str());
  if(!airspacesFile)
    throw std::runtime_error("ERROR: Couldn't open file '" + airspacesFileName + "' for writing!!!");

  airspacesFile << "*******************************************************" << std::endl;
  airspacesFile << "* Condor Task Penalty Zones generated with Condor2Nav *" << std::endl;
  airspacesFile << "*******************************************************" << std::endl;
  for(unsigned i=0; i<pzNum; i++) {
    std::string tpIdxStr(Convert(i));
    airspacesFile << std::endl;
    airspacesFile << "AC P" << std::endl;
    airspacesFile << "AN Penalty Zone " << i + 1 << std::endl;
    airspacesFile << "AH " << taskParser.Value("Task", "PZTop" + tpIdxStr) << "m AMSL" << std::endl;
    unsigned base(Convert<unsigned>(taskParser.Value("Task", "PZBase" + tpIdxStr)));
    if(base == 0)
      airspacesFile << "AL 0" << std::endl;
    else
      airspacesFile << "AL " << base << "m AMSL" << std::endl;
    
    for(unsigned j=0; j<4; j++) {
      std::string tpCornerIdxStr(Convert(j));
      std::string x(taskParser.Value("Task", "PZPos" + tpCornerIdxStr + "X" + tpIdxStr));
      std::string y(taskParser.Value("Task", "PZPos" + tpCornerIdxStr + "Y" + tpIdxStr));
      airspacesFile << "DP " << coordConv.Latitude(x, y, CCondor::CCoordConverter::FORMAT_DDMMSS) <<
        " " << coordConv.Longitude(x, y, CCondor::CCoordConverter::FORMAT_DDMMSS) << std::endl;
    }
  }
}


/**
* @brief Sets weather data. 
*
* Method sets the wind data.
*
* @param taskParser Condor task parser. 
**/
void condor2nav::CTargetXCSoar::Weather(const CFileParserINI &taskParser)
{
  unsigned dir = static_cast<unsigned>(Convert<float>(taskParser.Value("Weather", "WindDir")) + 0.5);
  unsigned speed = static_cast<unsigned>(Convert<float>(taskParser.Value("Weather", "WindSpeed")) + 0.5);
  _profileParser.Value("", "WindBearing", Convert(dir));
  _profileParser.Value("", "WindSpeed", Convert(speed));
}