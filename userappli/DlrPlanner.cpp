#include "../userappli/proto/DlrPlanner.h"
#include "../userappli/proto/userappli_proto.h"
#include <stdlib.h>
#include "Graphic-pkg.h"

DlrPlanner::DlrPlanner(char* fileName){
	_trajFile.append(fileName);
  _startConfig = NULL;
  _approachConfig = NULL;
  _graspConfig = NULL;
  _finalConfig = NULL;
  _robot = XYZ_ROBOT;
}

DlrPlanner::~DlrPlanner(){
	if(_startConfig != NULL){
		p3d_destroy_config(_robot, _approachConfig);
		_startConfig = NULL;
	}
	if(_approachConfig != NULL){
    p3d_destroy_config(_robot, _approachConfig);
		_approachConfig = NULL;
  }
	if(_graspConfig != NULL){
    p3d_destroy_config(_robot, _graspConfig);
		_graspConfig = NULL;
  }
	if(_finalConfig != NULL){
    p3d_destroy_config(_robot, _finalConfig);
		_finalConfig = NULL;
  }
  for(std::map<std::string, DlrObject*>::iterator i = _objects.begin(); i != _objects.end(); i++){
    delete i->second; 
  }
	_objects.clear();
	for(unsigned int i = 0; i < _execStack.size(); i++){
		delete _execStack[i];
	}
	_execStack.clear();
	_robot = NULL;
}

void DlrPlanner::setStartConfig(std::vector<double> config){
  if(_startConfig != NULL){
    p3d_destroy_config(_robot, _startConfig);
  }
  _startConfig = vectorToConfigPt(config);
}
void DlrPlanner::setApproachConfig(std::vector<double> config){
  if(_approachConfig != NULL){
    p3d_destroy_config(_robot, _approachConfig);
  }
  _approachConfig = vectorToConfigPt(config);
}
void DlrPlanner::setGraspConfig(std::vector<double> config){
  if(_graspConfig != NULL){
    p3d_destroy_config(_robot, _graspConfig);
  }
  _graspConfig = vectorToConfigPt(config);
}
void DlrPlanner::setFinalConfig(std::vector<double> config){
  if(_finalConfig != NULL){
    p3d_destroy_config(_robot, _finalConfig);
  }
  _finalConfig = vectorToConfigPt(config);
}
void DlrPlanner::addObject(std::string name){
  DlrObject* object = new DlrObject(name);
  _objects.insert(make_pair(name, object));
}
void DlrPlanner::addObject(std::string name, std::vector<double> rightFrame, std::vector<double> leftFrame){
  DlrObject* object = new DlrObject(name, rightFrame, leftFrame);
  _objects.insert(make_pair(name, object));
}
void DlrPlanner::addPositionToObject(std::string name, int id, std::vector<double> position){
  std::map<std::string, DlrObject*>::iterator iter = _objects.find(name);
  iter->second->addPosition(position, id);
}
void DlrPlanner::addPlan(DlrPlan::planType type){
	DlrPlan* plan = new DlrPlan(type);
	_execStack.push_back(plan);
}
DlrPlan* DlrPlanner::getCurrrentPlan(){
	std::vector<DlrPlan*>::iterator iter = _execStack.end() - 1;
	return *iter;
}
std::string DlrPlanner::getTrajFileName(){
	return _trajFile;
}
configPt DlrPlanner::vectorToConfigPt(std::vector<double> config){
  configPt q = p3d_alloc_config(_robot);
  for(int i = 0; i < 6; i++){
    q[i] = 0;
  }
  for(int i = _robot->nb_dof - 1; i > _robot->nb_dof - 7; i--){
    q[i] = 0;
  }
  for(unsigned int i = 0; i < config.size(); i++){
    q[i + 6] = config[i];
  }
  return q;
}

DlrObject* DlrPlanner::getObject(std::string name){
	std::map<std::string, DlrObject*>::iterator iter = _objects.find(name);
	return iter->second;
}

void DlrPlanner::saveTraj(p3d_traj* traj){
	if(!traj){
		std::cout << "No traj to write" << std::endl;
		return;
	}
	p3d_localpath *lp = traj->courbePt;
	std::vector<p3d_traj*> trajArray;
	double previousParam = 0, currentParm = 0;
	p3d_localpath* prevLp = traj->courbePt;
	bool baseLP = isABaseLocalPath(lp);
	for(;lp; lp = lp->next_lp){
		if(baseLP != isABaseLocalPath(lp)){
			//split the trajectory
			trajArray.push_back(p3d_get_sub_traj(traj, prevLp, lp->prev_lp));
			previousParam = currentParm;
			prevLp = lp;
			baseLP = !baseLP;
		}
		currentParm += lp->length_lp;
	}
	if(previousParam == 0){
		trajArray.push_back(traj);
	}else{
		for(lp = traj->courbePt; lp->next_lp; lp = lp->next_lp); //go to the last lp in the traj
		trajArray.push_back(p3d_get_sub_traj(traj, prevLp, lp->prev_lp));
	}
	for(unsigned int i = 0; i < trajArray.size(); i++){
		lp = trajArray[i]->courbePt;
		if(isABaseLocalPath(lp)){
			saveTrajInFile(_trajFile.c_str(), trajArray[i], 0);
		}else{
			saveTrajInFile(_trajFile.c_str(), trajArray[i], 1);
		}
	}
}
bool DlrPlanner::isABaseLocalPath(p3d_localpath* lp){
//	configPt start = lp->config_at_param(_robot, lp, 0);
//	configPt end = lp->config_at_param(_robot, lp, lp->length_lp);
	//if(ABS(start[_robot->baseJnt->index_dof] - end[_robot->baseJnt->index_dof]) < EPS6){
	if(lp->type_lp == LINEAR){
		return false;
	}else{
		//base localpath
		return true;
	}
}

int DlrPlanner::process(){
	for(std::vector<DlrPlan*>::iterator iter = _execStack.begin(); iter != _execStack.end(); iter++){
		p3d_matrix4 objectPos, attachRight, attachLeft;
		//object Pos
		(*iter)->getStartPos(objectPos);
		//attach Frames
		DlrObject::convertArrayToP3d_matrix4((*iter)->getObject()->getRightAttachFrame(), attachRight);
		DlrObject::convertArrayToP3d_matrix4((*iter)->getObject()->getLeftAttachFrame(), attachLeft);
		p3d_mat4Copy(attachRight, _robot->ccCntrts[0]->Tatt);
		p3d_mat4Copy(attachLeft, _robot->ccCntrts[1]->Tatt);
		//start config
		deactivateCcCntrts(_robot, -1);
		if(iter == _execStack.begin()){ //the first plan to execute
			p3d_set_and_update_this_robot_conf(_robot, _startConfig);
		}else{
			p3d_set_and_update_this_robot_conf(_robot, _robot->ROBOT_GOTO);
		}
		(*iter)->DlrPlan::setBodyJntAtRightPos(_robot, _robot->objectJnt, objectPos);
		_robot->ROBOT_POS = p3d_get_robot_config(_robot);
		activateCcCntrts(_robot, -1);
		switch((*iter)->getType()){
			case DlrPlan::APPROACH :{
				pickObjectByMat(_robot, objectPos, attachRight, attachLeft);
				//saveTraj(platformGotoObjectByMat(_robot, objectPos, attachRight, attachLeft));
				break;
			}
			case DlrPlan::GRASP :{
				saveTraj(gotoObjectByMat(_robot, objectPos, attachRight, attachLeft));
				break;
			}
			case DlrPlan::CARRY :{
				p3d_matrix4 objectTarget;
				(*iter)->getTargetPos(objectTarget);
				saveTraj(carryObject(_robot, objectTarget, attachRight, attachLeft));
				break;
			}
			default:{
				return false;
			}
		}
	}
	return true;
}
