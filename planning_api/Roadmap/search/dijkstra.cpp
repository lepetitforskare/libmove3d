/* Copyright (c) 2009 the authors listed at the following URL, and/or
 the authors of referenced articles or incorporated external code:
 http://en.literateprograms.org/Dijkstra's_algorithm_(C_Plus_Plus)?action=history&offset=20080520195640

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 Retrieved from: http://en.literateprograms.org/Dijkstra's_algorithm_(C_Plus_Plus)?oldid=13422
 */

#include "dijkstra.hpp"

using namespace std;
using namespace tr1;

Dijkstra::Dijkstra() : graph(NULL)
{
}

Dijkstra::Dijkstra(Graph* ptrGraph) : graph(ptrGraph)
{

}

Dijkstra::~Dijkstra()
{

}

void Dijkstra::creatStructures()
{
	if(graph==NULL)
	{
		return;
	}

	graph_node_map.clear();
	graph_adjacency_map.clear();

	vector<Node*> nodes = graph->getNodes();

	for (int i = 0; i < nodes.size(); i++)
	{
		int id_node = nodes[i]->getNodeStruct()->num;
		graph_node_map[id_node] = nodes[i];
//		cout << "Node num = " << id_node << endl;
//		nodes[i]->print();
	}

	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		Node* ptrNode = nodes[i];

		vector<p3d_node*> neighbors = ptrNode->getNeighbors();
		vector<p3d_edge*> edges = ptrNode->getEdges();

		vector<vertex_t> neighID;
		vector<weight_t> neighWeight;

		for (int j = 0; j < ptrNode->getNumberOfNeighbors(); j++)
		{
			vertex_t id = neighbors[j]->num;
			for (unsigned int k = 0; k < ptrNode->getNumberOfEdges(); k++)
			{
				p3d_edge* ptrEdge = edges[k];
				if ((ptrEdge->Nf->num == id) || (ptrEdge->Ni->num == id))
				{
					neighID.push_back(id);
					neighWeight.push_back(ptrEdge->cost);
					break;
				}
			}
		}

		for (int j = 0; j < ptrNode->getNumberOfNeighbors(); j++)
		{
			graph_adjacency_map[nodes[i]->getNodeStruct()->num].push_back(edge_dijkstra(neighID[j], neighWeight[j]));
//			cout << "graph_adjacency_map["<<i<<"] = "<<"edge_dijkstra("<<neighID[j]<<","<<neighWeight[j]<<")"<< endl;
		}
	}
}

void Dijkstra::computePaths(vertex_t source, adjacency_map_t& adjacency_map,
		map<vertex_t, weight_t>& min_distance,
		map<vertex_t, vertex_t>& previous)
{
	for (adjacency_map_t::iterator vertex_iter = adjacency_map.begin(); vertex_iter
			!= adjacency_map.end(); vertex_iter++)
	{
		vertex_t v = vertex_iter->first;
		min_distance[v] = numeric_limits<double>::infinity();
	}

	min_distance[source] = 0;
	set<pair<weight_t, vertex_t> , pair_first_less<weight_t, vertex_t> >
			vertex_queue;

	for (adjacency_map_t::iterator vertex_iter = adjacency_map.begin(); vertex_iter
			!= adjacency_map.end(); vertex_iter++)
	{
		vertex_t v = vertex_iter->first;
		vertex_queue.insert(pair<weight_t, vertex_t> (min_distance[v], v));
	}

	while (!vertex_queue.empty())
	{
		vertex_t u = vertex_queue.begin()->second;
		vertex_queue.erase(vertex_queue.begin());

		// Visit each edge exiting u
		for (list<edge_dijkstra>::iterator edge_iter = adjacency_map[u].begin(); edge_iter
				!= adjacency_map[u].end(); edge_iter++)
		{
			vertex_t v = edge_iter->target;
			weight_t weight = edge_iter->weight;
			weight_t distance_through_u = min_distance[u] + weight;

			if (distance_through_u < min_distance[v])
			{
				vertex_queue.erase(
						pair<weight_t, vertex_t> (min_distance[v], v));

				min_distance[v] = distance_through_u;
				previous[v] = u;
				vertex_queue.insert(pair<weight_t, vertex_t> (min_distance[v],
						v));
			}
		}
	}
}

list<vertex_t> Dijkstra::getShortestPathTo(vertex_t target, map<vertex_t,
		vertex_t>& previous)
{
	list<vertex_t> path;
	map<vertex_t, vertex_t>::iterator prev;
	vertex_t vertex = target;
	path.push_front(vertex);
	while ((prev = previous.find(vertex)) != previous.end())
	{
		vertex = prev->second;
		path.push_front(vertex);
	}
	return path;
}

int Dijkstra::example()
{
	adjacency_map_t adjacency_map;
	vector<string> vertex_names;

	vertex_names.push_back("Harrisburg"); // 0
	vertex_names.push_back("Baltimore"); // 1
	vertex_names.push_back("Washington"); // 2
	vertex_names.push_back("Philadelphia"); // 3
	vertex_names.push_back("Binghamton"); // 4
	vertex_names.push_back("Allentown"); // 5
	vertex_names.push_back("New York"); // 6
	adjacency_map[0].push_back(edge_dijkstra(1, 79.83));
	adjacency_map[0].push_back(edge_dijkstra(5, 81.15));
	adjacency_map[1].push_back(edge_dijkstra(0, 79.75));
	adjacency_map[1].push_back(edge_dijkstra(2, 39.42));
	adjacency_map[1].push_back(edge_dijkstra(3, 103.00));
	adjacency_map[2].push_back(edge_dijkstra(1, 38.65));
	adjacency_map[3].push_back(edge_dijkstra(1, 102.53));
	adjacency_map[3].push_back(edge_dijkstra(5, 61.44));
	adjacency_map[3].push_back(edge_dijkstra(6, 96.79));
	adjacency_map[4].push_back(edge_dijkstra(5, 133.04));
	adjacency_map[5].push_back(edge_dijkstra(0, 81.77));
	adjacency_map[5].push_back(edge_dijkstra(3, 62.05));
	adjacency_map[5].push_back(edge_dijkstra(4, 134.47));
	adjacency_map[5].push_back(edge_dijkstra(6, 91.63));
	adjacency_map[6].push_back(edge_dijkstra(3, 97.24));
	adjacency_map[6].push_back(edge_dijkstra(5, 87.94));

	map<vertex_t, weight_t> min_distance;
	map<vertex_t, vertex_t> previous;

	computePaths(0, adjacency_map, min_distance, previous);

	for (adjacency_map_t::iterator vertex_iter = adjacency_map.begin(); vertex_iter
			!= adjacency_map.end(); vertex_iter++)
	{
		vertex_t v = vertex_iter->first;
		cout << "Distance to " << vertex_names[v] << ": " << min_distance[v]
				<< endl;
		list<vertex_t> path = getShortestPathTo(v, previous);
		list<vertex_t>::iterator path_iter = path.begin();
		cout << "Path: ";
		for (; path_iter != path.end(); path_iter++)
		{
			cout << vertex_names[*path_iter] << " ";
		}
		cout << endl;
	}

	return 0;
}

Trajectory* Dijkstra::extractTrajectory(shared_ptr<Configuration> init,shared_ptr<Configuration> goal)
{
	if(graph==NULL)
	{
		return NULL;
	}

	Node* N;

	N = new Node(graph,init);
	graph->insertNode(N);
	graph->linkNode(N);

	vertex_t source = N->getNodeStruct()->num;

	N = new Node(graph,goal);
	graph->insertNode(N);
	graph->linkNode(N);

	vertex_t target = N->getNodeStruct()->num;

	if (ENV.getBool(Env::drawGraph))
	{
		g3d_draw_allwin_active();
	}

	return extractTrajectory(source,target);
}

Trajectory* Dijkstra::extractTrajectory(vertex_t source,vertex_t target)
{
	creatStructures();

	map<vertex_t, weight_t> min_distance;
	map<vertex_t, vertex_t> previous;

	computePaths(source, graph_adjacency_map, min_distance, previous);
	list<vertex_t> path = getShortestPathTo(target, previous);

	Trajectory* traj = new Trajectory(graph->getRobot());

	for (list<vertex_t>::iterator path_iter = path.begin();
			path_iter != path.end();
			path_iter++)
	{
		Node* ptrNode = graph_node_map[*path_iter];
		shared_ptr<Configuration> q = ptrNode->getConfiguration();
		traj->push_back(q);
	}

//	traj->print();

	return traj;

}

