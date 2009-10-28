#include "condor2nav.h"
#include "condor.h"
#include "xcsoar.h"


const char *condor2nav::CCondor2Nav::CONFIG_FILE_NAME = "condor2nav.ini";
const char *condor2nav::CCondor2Nav::DATA_PATH = "data";
const char *condor2nav::CCondor2Nav::SCENERIES_DATA_FILE_NAME = "SceneryData.csv";
const char *condor2nav::CCondor2Nav::GLIDERS_DATA_FILE_NAME = "GliderData.csv";



/**
 * @brief Class constructor. 
 *
 * condor2nav::CCondor2Nav class constructor that parses application configuration
 * INI file.
 *
 * @param argc   Number of command-line arguments. 
 * @param argv   Array of command-line argument strings. 
**/
condor2nav::CCondor2Nav::CCondor2Nav(int argc, const char *argv[]):
_configParser(CONFIG_FILE_NAME),
_taskName((argc > 1) ? argv[1] : _configParser.Value("Condor", "DefaultTaskName"))
{
}


/**
 * @brief Creates Condor data translator. 
 *
 * Method creates Condor data translator.
 *
 * @note For now only XCSoar is supported but later on that method will return
 *       data translator required by configuration INI file.
 *
 * @param outputPath The path of the translation output directory.
 *
 * @return Condor data translator.
**/
std::auto_ptr<condor2nav::CTranslator> condor2nav::CCondor2Nav::Translator(const std::string &outputPath) const
{
  return std::auto_ptr<condor2nav::CTranslator>(new CTranslatorXCSoar(outputPath, _configParser.Value("XCSoar", "DataPath")));
}


/**
 * @brief Runs translation.
 *
 * Method is responsible for Condor data translation. Several
 * translate actions are configured through configuration INI file.
 * 
 * @return Application execution result.
**/
int condor2nav::CCondor2Nav::Run()
{
  // create Condor data
  condor2nav::CCondor condor(_configParser.Value("Condor", "Path"), _taskName);

  // create translator
  std::auto_ptr<condor2nav::CTranslator> translator(Translator(_configParser.Value("Condor2Nav", "OutputPath")));

  // translate scenery data
  {
    const CFileParserCSV sceneriesParser(DATA_PATH + std::string("\\") + SCENERIES_DATA_FILE_NAME);
    const CFileParserCSV::CStringArray &sceneryData = sceneriesParser.Row(condor.TaskParser().Value("Task", "Landscape"));

    if(_configParser.Value("Condor2Nav", "SetSceneryMap") == "1")
      translator->SceneryMap(sceneryData);
    if(_configParser.Value("Condor2Nav", "SetSceneryTime") == "1")
      translator->SceneryTime(sceneryData);
  }
  
  // translate glider data
  if(_configParser.Value("Condor2Nav", "SetGlider") == "1")
  {
    const CFileParserCSV glidersParser(DATA_PATH + std::string("\\") + GLIDERS_DATA_FILE_NAME);
    translator->Glider(glidersParser.Row(condor.TaskParser().Value("Plane", "Name")));
  }

  // translate task
  if(_configParser.Value("Condor2Nav", "SetTask") == "1")
    translator->Task(condor.TaskParser(), condor.CoordConverter());

  // translate airspaces
  if(_configParser.Value("Condor2Nav", "SetAirspaces") == "1")
    translator->Airspaces(condor.TaskParser(), condor.CoordConverter());

  // translate weather
  if(_configParser.Value("Condor2Nav", "SetWeather") == "1")
    translator->Weather(condor.TaskParser());
  
  return EXIT_SUCCESS;
}
