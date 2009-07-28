#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "../planningAPI.hpp"

class Node;

/**
	\brief Classe représentant une Configuration d'un Robot
	@author Florian Pilardeau,B90,6349 <fpilarde@jolimont>
*/
class Configuration{

private:
  bool flagInitQuaternions;/*!< Booleen indiquant que les Quaternions ont été initialisés*/
  Robot* _Robot;/*!< Le Robot pour lequel la Configuration est créée*/
  configPt _Configuration;/*!< une structure de congitPt contenant les données sur la Configuration*/
//  std::vector<Gb_quat*> _VectQuaternions;/*!< Le vecteur des Quaternions représentant cette Configuration*/

public:
  //constructor and destructor
    /**
     * Constructeur de la classe
     * @param R le Robot pour lequel la Configuration est créée
     */
    Configuration(Robot* R);

    /**
     * Constructeur de la classe
     * @param R le Robot pour lequel la Configuration est créée
     * @param C la structure de Configuration qui sera stockée
     */
    Configuration(Robot* R, configPt C);

    /**
     * Destructeur de la classe
     */
    ~Configuration();

    /**
     * détruie la configPt stockée
     */
    void Clear();

  //Accessors
    /**
     * obtient le Robot pour lequel la Configuration est créée
     * @return le Robot pour lequel la Configuration est créée
     */
    Robot* getRobot();

    /**
     * obtient le vecteur des Quaternions
     * @return le vecteur des Quaternions
     */
//    std::vector<Gb_quat*> getQuat();

    /**
     * obtient le pointeur sur la ConfigPt
     * @return la pointeur sur la ConfigPt
     */
    configPt* getConfigPtStruct();
    /**
     * obtient la structure configPt stockée
     * @return la structure configPt stockée
     */
    configPt getConfigurationStruct();
    /**
     * modifie la structure configPt stockée
     * @param C la nouvelle structure configPt
     */
    void setConfiguration(configPt C);
    /**
     * modifie la structure configPt stockée
     * @param C la Configuration contentant la nouvelle structure
     */
    void setConfiguration(Configuration& C);

    /**
     * indique si le vecteur de Quaternions est initialisé
     * @return le vecteur de Quaternions est initialisé
     */
    bool isInint();
    /**
     * initialise le vecteur de Quaternions
     */
    void initQuaternions();

    /**
     * calcule la distance à une Configuration
     * @param Conf la Configuration entrée
     * @return la distance
     */
    double dist(Configuration& Conf);
    /**
     * calcule la distance à une Configuration
     * @param q la Configuration entrée
     * @param distChoice le type de calcul de distance
     * @return la distance
     */
    double dist(Configuration& q, int distChoice);
    /**
     * indique si la Configuration est en collision
     * @return la Configuration est en collision
     */
    bool IsInCollision();
    /**
     * compare à une autre Configuration
     * @param Conf la Configuration entrée
     * @return les deux Configurations sont égales
     */
    bool equal(Configuration& Conf);
    /**
     * copie une Configuration
     * @return une copie de la Configuration
     */
    std::tr1::shared_ptr<Configuration> copy();
    /**
     * copie les joints passifs de la Configuration courante dans la Configuration entrée
     * @param C in/out la Configuration à modifier
     */
    void copyPassive(Configuration& C);
    /**
     * obtient le cout de la Configuration suivant l'espace des fonctions de cout
     * @return le cout de la Configuration
     */
    double cost();

    /**
     * \brief Main cost acceptance function of the T-RRT algorithm.
     *
     * Transition Test function to validate the feasability of the motion
     * from the current config with the current cost in function
     * of the previous config and cost.
     * This test is currently based on the Metropolis Criterion
     * also referred as the Boltzmann probability when applied to
     * statistical physics or molecular modeling.
     * The temperature parameter is adaptively tuned  in function of the
     * failures and successes during the search process.
     * This adaptation can be local to each node or applied globaly to
     * the entire graph.
     * @param previousNode le Node à tester
     * @param Step la longueur maximale de l'arrete dans RRT
     * @return le test reussit
     */
    bool costTestSucceeded(Node* previousNode, double Step);
    /**
     * somme deux Configuration
     * @param C la Configuration à sommer
     * @return .la somme des deux Configuration
     */
    std::tr1::shared_ptr<Configuration> add(Configuration& C);

};

#endif