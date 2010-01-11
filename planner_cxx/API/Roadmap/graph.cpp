//
// C++ Implementation: graph
//
// Description:
//
//
// Author: Florian Pilardeau,B90,6349 <fpilarde@jolimont>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "../planningAPI.hpp"

using namespace std;
using namespace tr1;

// Constructors
//----------------------------------------------
Graph::Graph(Robot* R, p3d_graph* G)
{
    if (G)
    {
        _Graph = MY_ALLOC(p3d_graph, 1);
        *_Graph = *G;
    }
    else
    {
        _Graph = p3d_create_graph();
    }
    _Robot = R;
    _Graph->rob->GRAPH = _Graph;
    _Traj = NULL;
    this->init();
}

Graph::Graph(Robot* R)
{
    _Robot = R;
    _Graph = p3d_allocinit_graph();

    _Graph->env = (p3d_env *) p3d_get_desc_curid(P3D_ENV);
    _Graph->rob = _Robot->getRobotStruct();
    if (_Robot->getRobotStruct() != NULL)
    {
        _Robot->getRobotStruct()->GRAPH = _Graph;
        XYZ_GRAPH = _Graph;
    }
    if (STAT)
    {
        _Graph->stat = createStat();
    }
    else
    {
        _Graph->stat = NULL;
    }
    _Traj = NULL;
    this->init();
}

Graph::Graph(p3d_graph* G)
{
    if (G)
    {
        /*_Graph = MY_ALLOC(p3d_graph, 1);
                *_Graph = *G;*/
        _Graph = G;
    }
    else
    {
        _Graph = p3d_create_graph();
    }
    _Robot = new Robot(_Graph->rob);
    _Traj = NULL;
    this->init();
}

/**
  * Fonction creant un Objet Graph a partir d'une structure de p3d_Graph
  */
void Graph::init()
{
    if (_Graph->nodes)
    {
        p3d_list_node* l = _Graph->nodes;
        while (l)
        {
            Node* node = new Node(this, l->N);
            _NodesTable.insert(pair<p3d_node*, Node*> (l->N, node));
            _Nodes.push_back(node);
            l = l->next;
        }
    }
    if (_Graph->edges)
    {
        p3d_list_edge* l = _Graph->edges;
        while (l)
        {
            Edge* edge = new Edge(this, l->E);
            _Edges.push_back(edge);
            l = l->next;
        }
    }
    this->setName();
}

// Destructors
//----------------------------------------------

Graph::~Graph()
{
    p3d_del_graph(_Graph);
    this->freeResources();
}

/**
  * Frees the Nodes and Edges
  */
void Graph::freeResources()
{

    for(int i=0;i<_Nodes.size();i++)
    {
        delete _Nodes[i];
    }

    for(int i=0;i<_Edges.size();i++)
    {
        delete _Edges[i];
    }

}

// Accessors
//----------------------------------------------
p3d_graph* Graph::getGraphStruct()
{
    return _Graph;
}

void Graph::setGraph(p3d_graph* G)
{
    *_Graph = *G;
}

Robot* Graph::getRobot()
{
    return _Robot;
}

void Graph::setTraj(p3d_traj* Traj)
{
    _Traj = Traj;
}

p3d_traj* Graph::getTrajStruct()
{
    return _Traj;
}

vector<Node*> Graph::getNodes()
{
    return _Nodes;
}

vector<Edge*> Graph::getEdges()
{
    return _Edges;
}

map<p3d_node*, Node*> Graph::getNodesTable()
{
    return _NodesTable;
}

int Graph::getNbNode()
{
    return _Nodes.size();
}

Node* Graph::getNode(p3d_node* N)
{
    map<p3d_node*, Node*>::iterator it = _NodesTable.find(N);
    if (it != _NodesTable.end())
        return (it->second);
    else
    {
        cout << "node " << N << " not found: num: " << N->num << endl;
        return (NULL);
    }
}

Node* Graph::getLastnode()
{
    return (_Nodes.back());
}

string Graph::getName()
{
    return _Name;
}

void Graph::setName()
{
    _Name = "graph";
}

void Graph::setName(string Name)
{
    _Name = Name;
}

bool Graph::equal(Graph* G)
{
    return (_Graph->nodes == G->getGraphStruct()->nodes);
}

bool Graph::equalName(Graph* G)
{
    return (_Name == G->getName());
}

// Graph operations
//----------------------------------------------

/**
  * Search configuration in graph
  */
Node* Graph::searchConf(shared_ptr<Configuration> q)
{
    p3d_node* node(p3d_TestConfInGraph(_Graph, q->getConfigStruct()));
    return (node ? _NodesTable[node] : NULL);
}

/**
  * Insert node in graph
  */
Node* Graph::insertNode(Node* node)
{
    _NodesTable.insert(pair<p3d_node*, Node*> (node->getNodeStruct(), node));
    _Nodes.push_back(node);

    p3d_insert_node(_Graph, node->getNodeStruct());

    this->getGraphStruct()->dist_nodes = p3d_add_node_to_list(
            node->getNodeStruct(),
            this->getGraphStruct()->dist_nodes);

    return (node);
}

/**
  * Insert Extremal Node in Graph
  */
Node* Graph::insertExtremalNode(Node* node)
{
    this->insertNode(node);
    node->getNodeStruct()->type = ISOLATED;
    return (node);
}

/**
  * Compare Length Edges
  */
bool Graph::compareEdges(Edge* E1, Edge* E2)
{
    return (E1->getEdgeStruct()->longueur < E2->getEdgeStruct()->longueur);
}

/**
  * Compare Nodes
  */
bool Graph::compareNodes(Node* N1, Node* N2)
{
    return (N1->getNodeStruct()->dist_Nnew < N2->getNodeStruct()->dist_Nnew);
}

/**
  * Sort Edges
  */
void Graph::sortEdges()
{
    if (_Edges.size() > 0)
    {
        sort(_Edges.begin(), _Edges.end(), &compareEdges);
    }
}

/**
  * Sort Node by distance
  */
void Graph::sortNodesByDist(Node* N)
{
    if (_Nodes.size() > 1)
    {
        for (int i = 0; i < _Nodes.size(); i = i + 1)
        {
            _Nodes[i]->dist(N);
        }
        sort(_Nodes.begin(), _Nodes.end(), &compareNodes);
    }
}

/**
  * Is Edge in graph
  */
bool Graph::isEdgeInGraph(Node* N1, Node* N2)
{
    bool flag = false;

    for (uint i = 0; i < _Edges.size(); i = i + 1)
    {
        flag =
                ( _Edges[i]->getEdgeStruct()->Ni == N1->getNodeStruct() ) &&
                ( _Edges[i]->getEdgeStruct()->Nf == N2->getNodeStruct() );
        if (flag)
        {
            return flag;
        }
    }
    return flag;
}

/**
  * Add Edge to Graph
  */
void Graph::addEdge(Node* N1, Node* N2, double Long)
{
    if (!this->isEdgeInGraph(N1, N2))
    {
        Edge* E = new Edge(this, N1, N2, Long);
        _Edges.push_back(E);
    }
}

/**
  * Add Edges
  */
void Graph::addEdges(Node* N1, Node* N2, double Long)
{
    this->addEdge(N1, N2, Long);
    this->addEdge(N2, N1, Long);
}

/**
  * Add Edge to all node inferior to max Dist form N
  */
void Graph::addNode(Node* N, double maxDist)
{
    double d;
    for (uint i = 0; i < _Nodes.size(); i = i + 1)
    {
        if ((d = _Nodes[i]->getConfiguration()->dist(*N->getConfiguration()))
            < maxDist)
            {
            LocalPath path(_Nodes[i]->getConfiguration(), N->getConfiguration());

            if ( path.getValid() )
            {
                this->addEdges(_Nodes[i], N, d);
            }
        }
    }
    _Nodes.push_back(N);
}

/**
  * Add  vector of node
  */
void Graph::addNodes(vector<Node*> N, double maxDist)
{
    for (vector<Node*>::iterator it = N.begin(), end = N.end(); it != end; it++)
    {
        this->addNode(*it, maxDist);
    }
}

/**
  * Test if node is in graph
  */
bool Graph::isInGraph(Node* N)
{
    for (uint i = 0; i < _Nodes.size(); i = i + 1)
    {
        if (N->equal(_Nodes[i]))
        {
            return true;
        }
    }
    return false;
}

/**
  * Link node to graph
  */
bool Graph::linkNode(Node* N)
{
    if (ENV.getBool((Env::useDist)))
    {
        return this->linkNodeAtDist(N);
    }
    else
    {
        return this->linkNodeWithoutDist(N);
    }
}

/**
  * Links node to graph Without distance
  */
bool Graph::linkNodeWithoutDist(Node* N)
{
    bool b = false;
    for (int j = 1; j <= _Graph->ncomp; j = j + 1)
    {
        if (N->getNodeStruct()->numcomp != j)
        {
            for (uint i = 0; i < _Nodes.size(); i = i + 1)
            {
                if (_Nodes[i]->getNodeStruct()->numcomp == j)
                {
                    if (_Nodes[i]->connectNodeToCompco(N, 0))
                    {
                        N->getNodeStruct()->search_to
                                = N->getNodeStruct()->last_neighb->N;
                        N->getNodeStruct()->last_neighb->N->search_from
                                = N->getNodeStruct();
                        N->getNodeStruct()->last_neighb->N->edge_from
                                = N->getNodeStruct()->last_neighb->N->last_edge->E;
                        b = true;
                    }
                    break;
                }
            }
        }
    }
    return b;
}

/**
  * Links node at distance
  */
bool Graph::linkNodeAtDist(Node* N)
{
    p3d_link_node_graph_multisol(N->getNodeStruct(), _Graph);
    this->MergeCheck();
}

/**
  * Links Node to all nodes
  */
bool Graph::linkToAllNodes(Node* N)
{
    return p3d_all_link_node(N->getNodeStruct(), _Graph);
}

/**
  * Returns vector of nodes that are orphan
  */
vector<Node**> Graph::isOrphanLinking(Node* N, int* link)
{
    vector<Node**> vect;
    double dist = 0;
    for (int j = 1; j <= _Graph->ncomp; j = j + 1)
    {
        for (uint i = 0; i < _Nodes.size(); i = i + 1)
        {
            if (_Nodes[i]->getNodeStruct()->numcomp == j)
            {
                if (_Nodes[i]->isLinkable(N, &dist) && (dist < ENV.getDouble(
                        Env::dist) || !ENV.getBool(Env::useDist)))
                {
                    *link = *link + 1;
                    vect.push_back(&_Nodes[i]);
                    break;
                }
            }
        }
    }
    return vect;
}

/**
  * Is Linking Orphan node
  */
bool Graph::linkOrphanLinking(Node* N, p3d_graph* Graph_Pt, void(*fct_draw)(
        void), int type, int* ADDED, int* nb_fail)
{
    int link = 0;
    vector<Node**> vect = this->isOrphanLinking(N, &link);
    if ((type == 1 || type == 2) && link > 1)
    {
        for (uint k = 0; k < vect.size(); k = k + 1)
        {
            (*(vect[k]))->connectNodeToCompco(N, 0);
        }
        this->insertNode(N);
        *ADDED = *ADDED + 1;
        *nb_fail = 0;
        if (ENV.getBool(Env::drawGraph))
        {
            *Graph_Pt = *(this->getGraphStruct());
            (*fct_draw)();
        }
        vect.clear();
        return false;
    }
    else if ((type == 0 || type == 2) && link == 0)
    {
        this->insertNode(N);
        *ADDED = *ADDED + 1;
        *nb_fail = 0;
        if (ENV.getBool(Env::drawGraph))
        {
            *Graph_Pt = *(this->getGraphStruct());
            (*fct_draw)();
        }
        vect.clear();
        return true;
    }
    else
    {
        N->deleteCompco();
        MY_FREE(N->getNodeStruct(),p3d_node,1);
        N->~Node();
        vect.clear();
        return false;
    }
}

/**
  * Create One Orphan Linking
  */
void Graph::createOneOrphanLinking(p3d_graph* Graph_Pt, void(*fct_draw)(void),
                                   int type, int* ADDED, int* nb_fail)
{
    shared_ptr<Configuration> C = _Robot->shoot();
    *nb_fail = *nb_fail + 1;
    if (!C->IsInCollision())
    {
        Node* N = new Node(this, C);
        this->linkOrphanLinking(N, Graph_Pt, fct_draw, type, ADDED, nb_fail);
    }
    //   else
    //   {
    //     C->Clear();
    //     C->~Configuration();
    //   }
}

/**
  * Create Orphan Linkin node
  */
int Graph::createOrphansLinking(int nb_node, int(*fct_stop)(void),
                                void(*fct_draw)(void), int type)
{
    int ADDED = 0;
    int nb_try = 0;
    while ((*fct_stop)() && _Nodes.size() < nb_node && nb_try < ENV.getInt(
            Env::NbTry) && (_Graph->ncomp > 1 || !type))
    {
        createOneOrphanLinking(_Graph, fct_draw, type, &ADDED, &nb_try);
    }
    return ADDED;
}

/**
  * Create Random Configurations
  */
void Graph::createRandConfs(int NMAX, int(*fct_stop)(void), void(*fct_draw)(
        void))
{
    int inode;
    double tu, ts;

    ChronoOn();

    inode = 0;

    shared_ptr<Configuration> Cs = shared_ptr<Configuration> (
            new Configuration(_Robot, _Robot->getRobotStruct()->ROBOT_POS));
    Node* Ns = new Node(this, Cs);
    this->insertNode(Ns);

    while (inode < NMAX)
    {
        shared_ptr<Configuration> C = _Robot->shoot();
        if (!C->IsInCollision())
        {
            this->insertNode(new Node(this, C));
            inode = inode + 1;
            if (fct_draw)
                (*fct_draw)();
            else
            {
                PrintInfo(("Random conf. generation in not possible\n"));
                break;
            }

            if (fct_stop)
            {
                if (!(*fct_stop)())
                {
                    PrintInfo(("Random confs. generation canceled\n"));
                    break;
                }
            }
        }
    }

    PrintInfo(("For the generation of %d configurations : ", inode));
    ChronoTimes(&tu, &ts);
    _Graph->time = _Graph->time + tu;
    ChronoPrint("");
    ChronoOff();
    p3d_print_info_graph(_Graph);
}

/**
  * Random Nodes From Component
  */
Node* Graph::randomNodeFromComp(Node* comp)
{
    return (this->getNode(p3d_RandomNodeFromComp(comp->getCompcoStruct())));
}

/**
  * Nearest Weighted Neighbout in graph
  */
Node* Graph::nearestWeightNeighbour(Node* compco, shared_ptr<Configuration> config,
                                    bool weighted, int distConfigChoice)
{
    p3d_node* BestNodePt = NULL;
    double BestScore = P3D_HUGE;
    double CurrentDist = -1.;
    double CurrentScore = -1.;
    double DistOfBestNode = -1.;
    p3d_matrix4 *RefFramePt = NULL, *MobFramePt = NULL;
    p3d_matrix4 MobFrameRef, invT;

    // When retrieving statistics
    if (getStatStatus())
    {
        _Graph->stat->planNeigCall++;
    }

    //computation of the mobFrameRef of the Config
    if (distConfigChoice == MOBILE_FRAME_DIST && p3d_GetRefAndMobFrames(
            _Graph->rob, &RefFramePt, &MobFramePt))
    {
        p3d_set_robot_config(_Graph->rob, config->getConfigStruct());
        p3d_update_this_robot_pos_without_cntrt_and_obj(_Graph->rob);
        p3d_GetRefAndMobFrames(_Graph->rob, &RefFramePt, &MobFramePt);
        if (RefFramePt == NULL)
        {
            p3d_mat4Copy(*MobFramePt, MobFrameRef);
        }
        else
        {
            p3d_matInvertXform(*RefFramePt, invT);
            p3d_matMultXform(invT, *MobFramePt, MobFrameRef);
        }
    }

//    cout << "distConfigChoice = " << distConfigChoice << endl;

    p3d_list_node* nodes(compco->getCompcoStruct()->dist_nodes);
    while (nodes)
    {
        /* We take into account only the nodes undiscarded */
        if (!nodes->N->IsDiscarded)
        {
            if (distConfigChoice == MOBILE_FRAME_DIST)
            {
                CurrentDist = p3d_GetSe3DistanceFrames(_Graph->rob,
                                                       MobFrameRef, nodes->N->RelMobFrame);
            }
            else
            {
                CurrentDist = config->dist(
                        *_NodesTable[nodes->N]->getConfiguration(),
                        distConfigChoice);
            }

            CurrentScore = CurrentDist
                           * (weighted ? p3d_GetNodeWeight(nodes->N) : 1.0);

            if (CurrentScore < BestScore)
            {
                BestScore = CurrentScore;
                BestNodePt = nodes->N;
                DistOfBestNode = CurrentDist;
            }
        }
        nodes = nodes->next;
    }

    if ((p3d_GetIsMaxDistNeighbor() == TRUE) && (BestNodePt->boundary == TRUE)
        && (BestNodePt->radius < DistOfBestNode))
        {
        /* There is a maximal distance allowed to get a node as neighbor */
        return NULL;
        }

    return _NodesTable[BestNodePt];
}

/**
  * Merge Component
  */
int Graph::MergeComp(Node* CompCo1, Node* CompCo2, double DistNodes)
{

    if ((CompCo1 == NULL) || (CompCo2 == NULL))
    {
        PrintInfo (("Warning: Try to link two nodes with NULL structures \n"));
        return FALSE;
    }

    if (CompCo1->getCompcoStruct()->num < CompCo2->getCompcoStruct()->num)
    {
        CompCo1->merge(CompCo2);
    }
    else if (CompCo1->getCompcoStruct()->num > CompCo2->getCompcoStruct()->num)
    {
        CompCo2->merge(CompCo1);
    }

    this->addEdges(CompCo1, CompCo2, DistNodes);

    p3d_create_edges(_Graph,
                     CompCo1->getNodeStruct(),
                     CompCo2->getNodeStruct(),
                     DistNodes);

    return true;
}


/**
  * Get Nodes in the same compco
  */
std::vector<Node*> Graph::getNodesInTheCompCo(Node* node)
{
    p3d_list_node* ListNode = node->getCompcoStruct()->dist_nodes;
    std::vector<Node*> Nodes;

    while (ListNode!=NULL)
    {
        Nodes.push_back(this->_NodesTable[ListNode->N]);
        ListNode = ListNode->next;
    }
    return Nodes;
}


/**
  * Detect the need of merging comp
  */
void Graph::MergeCheck()
{
    p3d_merge_check(_Graph);
}

/**
  * Insert node for RRT
  */
Node* Graph::insertNode(
        Node* expansionNode,
        LocalPath& path)

        /*shared_ptr<Configuration> q,
                Node* expansionNode,
                double currentCost, double step)*/
{
    double step = path.length();


    Node* node(this->insertConfigurationAsNode(path.getEnd(), expansionNode, step));

    // Cost updates
    if (ENV.getBool(Env::isCostSpace))
    {
        double currentCost = path.getEnd()->cost();

        p3d_SetNodeCost(_Graph, node->getNodeStruct(), currentCost );

        //for adaptive variant, new temp is refreshed except if it is going down.
        if (currentCost < expansionNode->getNodeStruct()->cost)
        {
            node->getNodeStruct()->temp = expansionNode->getNodeStruct()->temp;
        }
        else
        {
            node->getNodeStruct()->temp = expansionNode->getNodeStruct()->temp
                                          / 2.;
        }
    }

    //weight updates
    if (p3d_GetIsWeightedChoice())
        p3d_SetNodeWeight(_Graph, node->getNodeStruct());

    //check stop conditions
    if (p3d_GetIsWeightStopCondition())
    {
        node->checkStopByWeight();
    }

    // Graph updates for RANDOM_IN_SHELL method
    if (ENV.getInt(Env::ExpansionNodeMethod) == RANDOM_IN_SHELL_METH)
    {
        p3d_SetNGood(p3d_GetNGood() + 1);

        if (node->getNodeStruct()->weight > _Graph->CurPbLevel)
        {
            _Graph->CurPbLevel = node->getNodeStruct()->weight;
            _Graph->n_consec_pb_level = 0;
            _Graph->n_consec_fail_pb_level = 0;
            if (p3d_GetNGood() > 2)
                _Graph->critic_cur_pb_level = _Graph->CurPbLevel;
        }
        else
        {
            _Graph->n_consec_pb_level++;
            _Graph->n_consec_fail_pb_level = 0;
        }
    }

    //Additional cycles through edges addition if the flag is active
    if (ENV.getBool(Env::addCycles))
    {
        this->addCycles(node, step);
    }


    return (node);
}

/**
  * Add Cycle in Graph
  */
void Graph::addCycles(Node* node, double step)
{
    double longStep = 3. * step;
    p3d_list_node* listDistNodePt = p3d_listNodeInDist(
            _Robot->getRobotStruct(), node->getNodeStruct()->comp,
            node->getNodeStruct(), longStep);

    p3d_list_node* savedListDistNodePt = listDistNodePt;

    shared_ptr<LocalPath> LP;

    while (listDistNodePt)
    {
        if (!p3d_IsSmallDistInGraph(_Graph, node->getNodeStruct(),
                                    listDistNodePt->N, 5, step))
        {
            LP = shared_ptr<LocalPath> (new LocalPath(node->getConfiguration(),
                                                      this->getNode(listDistNodePt->N)->getConfiguration()));
            if (LP->getValid()
                /*&& this->getNode(listDistNodePt->N)->getConfiguration()->costTestSucceeded(
                                                        node, step)
                                        && node->getConfiguration()->costTestSucceeded(
                                                        this->getNode(listDistNodePt->N), step)*/)

                {
                p3d_create_edges(_Graph, node->getNodeStruct(),
                                 listDistNodePt->N, LP->length());
                node->getNodeStruct()->edges->E->for_cycle = true;
            }
        }
        listDistNodePt = listDistNodePt->next;
    }
    while (savedListDistNodePt)
    {
        p3d_list_node* destroyListNodePt = savedListDistNodePt;
        savedListDistNodePt = savedListDistNodePt->next;
        MY_FREE(destroyListNodePt, p3d_list_node, 1);
    }
}

/**
  * Insert Lining Node for RRT
  */
Node* Graph::insertConfigurationAsNode(shared_ptr<Configuration> q, Node* from,
                                       double step)
{
    Node* node = new Node(this, q);

    this->insertNode(node);

    if (node->getCompcoStruct()->num < from->getCompcoStruct()->num)
    {
        p3d_merge_comp(_Graph,
                       node->getCompcoStruct(),
                       from->getCompcoStructPt());
    }
    else if (from->getCompcoStruct()->num < node->getCompcoStruct()->num)
    {
        p3d_merge_comp(_Graph,
                       from->getCompcoStruct(),
                       node->getCompcoStructPt());
    }

    p3d_create_edges(_Graph,
                     from->getNodeStruct(),
                     node->getNodeStruct(),
                     step);

    node->getNodeStruct()->rankFromRoot = from->getNodeStruct()->rankFromRoot +1;
    node->getNodeStruct()->type = LINKING;

    return(node);
}
