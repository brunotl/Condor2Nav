#include "fileParserCSV.h"
#include "tools.h"
#include <fstream>
#include <string>


/**
 * @brief Class constructor.
 *
 * condor2nav::CFileParserCSV class constructor.
 *
 * @param filePath The path of the CSV file to parse.
**/
condor2nav::CFileParserCSV::CFileParserCSV(const std::string &filePath):
CFileParser(filePath)
{
  // open CSV file
  std::ifstream inputStream(filePath.c_str());
  if(!inputStream)
    throw std::invalid_argument("ERROR: Couldn't open CSV file '" + filePath + "'!!!");

  // parse all lines
  std::string line;
  while(getline(inputStream, line)) {
    if(line.empty())
      continue;

    std::auto_ptr<CStringArray> row(new CStringArray);
    LineParseCSV(line, *row.get());
    _rowsList.push_back(row.release());
  }
}


/**
 * @brief Class destructor. 
 *
 * condor2nav::CFileParserCSV class destructor.
**/
condor2nav::CFileParserCSV::~CFileParserCSV()
{
  Purge(_rowsList);
}


/**
 * @brief Returns requested row.
 *
 * Method returns the values from one row specified by the value in specified column.
 *
 * @param value  The value to use for searching.
 * @param column The column index to be used for value comparison.
 *
 * @exception std Thrown when requested row is not found.
 *
 * @return Requested row.
**/
const condor2nav::CFileParser::CStringArray &condor2nav::CFileParserCSV::Row(const std::string &value, unsigned column /* = 0 */) const
{
  for(CRowsList::const_iterator it=_rowsList.begin(); it!=_rowsList.end(); ++it)
    if((**it).at(column) == value)
      return **it;

  throw std::runtime_error("ERROR: Couldn't find value '" + value + "' in column '" + Convert(column) + "' of CSV file '" + Path() + "'!!!");
}


//void condor2nav::CFileParserCSV::Row(CStringArray &row)
//{
//}


/**
 * @brief Dumps class data to the file.
 *
 * Method dumps class data to the file in the same format as input file has.
 * 
 * @param filePath Path of the file to create (empty means overwrite input file).
**/
void condor2nav::CFileParserCSV::Dump(const std::string &filePath /* = "" */) const
{
  std::string path = filePath != "" ? filePath.c_str() : Path().c_str();
  std::ofstream outputStream(path.c_str());
  if(!outputStream)
    throw std::invalid_argument("ERROR: Couldn't open CSV file '" + path + "' for writing!!!");

  for(CRowsList::const_iterator it=_rowsList.begin(); it!=_rowsList.end(); ++it) {
    for(unsigned i=0; i<(*it)->size(); i++) {
      if(i)
        outputStream << ",";
      outputStream << (**it)[i];
    }
    outputStream << std::endl;
  }
}
