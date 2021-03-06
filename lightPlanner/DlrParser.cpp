/*
 * Copyright (c) 2001-2014 LAAS/CNRS
 * All rights reserved.
 *
 * Redistribution  and  use  in  source  and binary  forms,  with  or  without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice and this list of conditions.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice and  this list of  conditions in the  documentation and/or
 *      other materials provided with the distribution.
 *
 * THE SOFTWARE  IS PROVIDED "AS IS"  AND THE AUTHOR  DISCLAIMS ALL WARRANTIES
 * WITH  REGARD   TO  THIS  SOFTWARE  INCLUDING  ALL   IMPLIED  WARRANTIES  OF
 * MERCHANTABILITY AND  FITNESS.  IN NO EVENT  SHALL THE AUTHOR  BE LIABLE FOR
 * ANY  SPECIAL, DIRECT,  INDIRECT, OR  CONSEQUENTIAL DAMAGES  OR  ANY DAMAGES
 * WHATSOEVER  RESULTING FROM  LOSS OF  USE, DATA  OR PROFITS,  WHETHER  IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR  OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.                                  
 *
 * Siméon, T., Laumond, J. P., & Lamiraux, F. (2001). 
 * Move3d: A generic platform for path planning. In in 4th Int. Symp.
 * on Assembly and Task Planning.
 */
#include "../lightPlanner/proto/DlrParser.h"
#include <stdlib.h>
#include <unistd.h>

DlrParser::DlrParser(char* fileName){
	_fileName.assign(fileName);
	_planner = NULL;
}

DlrParser::DlrParser(char* fileName, DlrPlanner* planner){
  _fileName.assign(fileName);
	_planner = planner;
	_planner->setParseFile(_fileName);
}

DlrParser::~DlrParser(){
}


int DlrParser::parse(void){
  return parse(_fileName);
}

int DlrParser::parse(std::string fileName){
	if(_planner == NULL){
		return false;
	}
  std::ifstream in(fileName.c_str(), std::ifstream::in);
  if (in.is_open()) {
		sleep(0.5);
    std::string line, tmp, nextKeyword = "", keyword, lineToProcess;
    std::vector<std::string> stringVector;
    int lineNum = 0;
    while (std::getline(in, line, '=')) {
      std::string::size_type i;
      lineNum++;
      keyword.clear();
      keyword.append(nextKeyword);
      if (!nextKeyword.compare("")) {//firstKeword save it and loop
        stripSpacesAndComments(line, nextKeyword);
        continue;
      } else {
        i = line.find_last_of("\n");
        tmp = line.substr(i + 1, line.size() - i);
        stripSpacesAndComments(tmp, nextKeyword);
      }
      //Process current keyword
      std::string::size_type brace = line.find_first_of("[");
      if (brace == std::string::npos) {//we have only quotes
        tmp = line.substr(line.find_first_of("\"") + 1, line.find_last_of("\"") - line.find_first_of("\"") - 1);
        stripSpacesAndComments(tmp, lineToProcess);
      } else {
        tmp = line.substr(line.find_first_of("[") + 1, line.find_last_of("]") - line.find_first_of("[") - 1);
        stripSpacesAndComments(tmp, lineToProcess);
      }
      std::vector<std::string> stringVector;
      tokenize(lineToProcess, stringVector, ",");//convert to array to check the order of frame
      std::vector<double> doubleVector = parseFrame(lineToProcess);

      if (!keyword.compare("start_configuration")) {
//        std::cout << "start_configuration" << std::endl;
        _planner->setStartConfig(doubleVector);
      } else if (!keyword.compare("object")) {
//        std::cout << "object" << std::endl;
        std::string objectName = "";
        if (brace == std::string::npos) {//it's a non manipulated object declaration
          objectName.append(lineToProcess);
          _planner->addObject(objectName);
        } else {
          std::vector<double> leftFrame, rightFrame;
          objectName.append(stringVector[0]);
          for (int j = 2; j < 14; j++) {
            if (!stringVector[1].compare("left")) {
              leftFrame.push_back(doubleVector[j]);
            } else {
              rightFrame.push_back(doubleVector[j]);
            }
          }
					if(stringVector.size() > 15){
						for (int j = 15; j < 27; j++) {
							if (!stringVector[14].compare("left")) {
								leftFrame.push_back(doubleVector[j]);
							} else {
								rightFrame.push_back(doubleVector[j]);
							}
						}
					}
          _planner->addObject(objectName, rightFrame, leftFrame);
        }
        //set the object active
      } else if (!keyword.compare("object_pose")) {
//        std::cout << "object_pose" << std::endl;
        std::vector<double> objectPos;
        for (int j = 2; j < 14; j++) {
          objectPos.push_back(doubleVector[j]);
        }
        int objectPosId = (int) doubleVector[1];
        std::string objectName = "";
        objectName.append(stringVector[0]);
        _planner->addPositionToObject(objectName, objectPosId, objectPos);
      } else if (!keyword.compare("plan_type")) {
//        std::cout << "plan_type" << std::endl;
				DlrPlan::planType type = DlrPlan::APPROACH;
				if(!lineToProcess.compare("approach_object")){
					type = DlrPlan::APPROACH;
				}else if(!lineToProcess.compare("grasp_object")){
					type = DlrPlan::GRASP;
				}else if(!lineToProcess.compare("carry_object")){
					type = DlrPlan::CARRY;
				}else if(!lineToProcess.compare("pre_grasp_object")){
          type = DlrPlan::PRECOMPGRASP;
        }else if(!lineToProcess.compare("pre_carry_object")){
          type = DlrPlan::PRECOMPCARRY;
        }else if(!lineToProcess.compare("reach_configuration")){
          type = DlrPlan::REACH;
        }else if(!lineToProcess.compare("touch_object")){
          type = DlrPlan::TOUCH;
        }else{
					std::cout << "Unknown keyword at line : " << lineNum << std::endl;
					return false;
				}
				_planner->addPlan(type);
      } else if (!keyword.compare("plan_object")) {
//        std::cout << "plan_object" << std::endl;
				std::vector<std::string> stringVector;
				tokenize(lineToProcess, stringVector, ",");//convert to array to get the object name
				std::vector<double> doubleVector = parseFrame(lineToProcess);
				DlrPlan* plan = _planner->getCurrrentPlan();
				DlrObject* object = _planner->getObject(stringVector[0]);
				plan->setObject(object);
				plan->setStartPos(object,  (int) doubleVector[1]);
      } else if (!keyword.compare("plan_object_target")) {
//        std::cout << "plan_object_target" << std::endl;
				std::vector<std::string> stringVector;
				tokenize(lineToProcess, stringVector, ",");//convert to array to get the object name
				std::vector<double> doubleVector = parseFrame(lineToProcess);
				DlrPlan* plan = _planner->getCurrrentPlan();
				DlrObject* object = _planner->getObject(stringVector[0]);
				plan->setObject(object);
				plan->setTargetPos(object,  (int) doubleVector[1]);
      } else if (!keyword.compare("plan_obstacles")) {
//        std::cout << "plan_obstacles" << std::endl;
				std::vector<std::string> stringVector;
				tokenize(lineToProcess, stringVector, ",");//convert to array to get the object name
				std::vector<double> doubleVector = parseFrame(lineToProcess);
				DlrPlan* plan = _planner->getCurrrentPlan();
				for(unsigned int j = 0; j < stringVector.size(); j+=2){
					DlrObject* object = _planner->getObject(stringVector[j]);
					plan->addObstacle(object, (int) doubleVector[j + 1]);
				}
      }else if (!keyword.compare("plan_target_configuration")) {
        _planner->setFinalConfig(doubleVector);
      }else if (!keyword.compare("plan_execute")) {
//        std::cout << "plan_execute" << std::endl;
				DlrPlan* plan = _planner->getCurrrentPlan();
				if(!lineToProcess.compare("true")){
					plan->setExecute(true);
				}else if(!lineToProcess.compare("false")){
					plan->setExecute(false);
				}else{
					std::cout << "Unknown keyword at line : " << lineNum << std::endl;
					return false;	
				}
      } else {
        std::cout << "Unknown keyword at line : " << lineNum << std::endl;
				return false;
      }
    }
  }else{
    std::cout << "Cannot open the file" << std::endl;
		return false;
	}
  return true;
}

void DlrParser::tokenize(const std::string& str,
                         std::vector<std::string>& tokens,
                         const std::string& delimiters = " "){
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos) {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

std::vector<double> DlrParser::parseFrame(std::string& line){
  std::vector<double> frame;
  std::vector<std::string> stringVector;
  tokenize(line, stringVector, ",");
  for (unsigned int i = 0; i < stringVector.size(); i++) {
    frame.push_back(atof(stringVector[i].c_str()));
  }
  return frame;
}

void DlrParser::removeCharFromString(const std::string& src, std::string& dest, const std::string& delimiter = " "){
  dest.clear();
  //Spaces
  // Skip delimiters at beginning.
  std::string::size_type lastPos = src.find_first_not_of(delimiter, 0);
  // Find first "non-delimiter".
  std::string::size_type pos = src.find_first_of(delimiter, lastPos);
  while (std::string::npos != pos || std::string::npos != lastPos) {
    // Found a token, add it to the vector.
    dest += src.substr(lastPos, pos - lastPos);
    // Skip delimiters.  Note the "not_of"
    lastPos = src.find_first_not_of(delimiter, pos);
    // Find next "non-delimiter"
    pos = src.find_first_of(delimiter, lastPos);
  }
}

void DlrParser::stripSpacesAndComments(std::string& src, std::string& dest){
  std::string tmp;
  //Remove spaces
  removeCharFromString(src, tmp, " ");

  //Comments
  std::string::size_type lastPos = 0;
  std::string::size_type pos = tmp.find_first_of("#", lastPos);
  lastPos = tmp.find_first_of("\n", pos);
  while (pos != std::string::npos || lastPos != std::string::npos) {
    tmp.erase(pos, lastPos - pos);
    pos = tmp.find_first_of("#", pos);
    lastPos = tmp.find_first_of("\n", pos);
  }

  //Retruns
  removeCharFromString(tmp, dest, "\n");
  //remove quotes and brakets
  tmp.clear();
  tmp.append(dest);
  dest.clear();
  removeCharFromString(tmp, dest, "\"");
	tmp.clear();
  tmp.append(dest);
  dest.clear();
	removeCharFromString(tmp, dest, "[");
	tmp.clear();
  tmp.append(dest);
  dest.clear();
	removeCharFromString(tmp, dest, "]");

}
