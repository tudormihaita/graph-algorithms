#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_set>

#define MAX 101001

using namespace std;

struct Edge {
	int flow, capacity;
	int u, v;
};

struct Vertex {
	int h, excess;
};


class Graph {
private:
	ifstream fin;
	ofstream fout;
	int V, E;

	vector<Vertex> vertices;
	vector<Edge> edges;
	int* current;


	int** G;
	int** rezG;
	int* parent;

	unordered_set<int>* adj;
	vector<int> circuit;

	//FUNCTIE UTILITARA METODA FORD-FULKERSON UTILIZAND ALGORITMUL EDMONDS-KARP
	bool BFS(int** rezG, int S, int T, int* parent);

	//FUNCTII UTILITARE METODA POMPARE-PREFLUX / PUSH-RELABEL
	void pushPreflow(int S);
	bool push(int u);
	void relabel(int u);
	void updateReverseEdgeFlow(int i, int flow);
	int overflowVertex(vector<Vertex>& ver);

	//FUNCTII UTILITARE ADITIONALE METODA POMPARE TOPOLOGICA / RELABEL-TO-FRONT
	void pushFlow(int u, int v);
	void discharge(int u);

	//FUNCTIE UTILITARA ALGORIMTUL LUI FLEURY DE DETERMINARE A UNUI CICLU EULERIAN
	void findEluerianCircuit(vector<int>& L, vector<vector<int>>& G, int u);


public:
	Graph(string input, string output);

	//AlGORITMI DE FLUX MAXIM
	int FordFulkerson();
	int PushRelabel();
	int RelabelToFront();

	//ALGORITMI DE DETERMINARE CICLU EULERIAN
	void Fleury();
	void Hierholzer(int s);


	~Graph() {
		for (int i = 0; i < V; i++) {
			delete[] G[i];
			delete[] rezG[i];
		}

		delete[] rezG;
		delete[] parent;
		delete[] G;
		delete[] adj;


		fin.close();
		fout.close();
	}

};


Graph::Graph(string in, string out) : fin{ in }, fout{ out } {
	fin >> V >> E;

	parent = new int[V];

	G = new int* [V];
	for (int i = 0; i < V; i++) {
		parent[i] = -1;
		G[i] = new int[V];
		for (int j = 0; j < V; j++) {
			G[i][j] = 0;
		}
	}

	this->adj = new unordered_set<int>[V];

	for (int i = 0; i < V; i++) {
		vertices.push_back({ 0,0 });
	}

	for (int i = 0; i < E; i++) {
		int u, v, w;
		fin >> u >> v >> w;

		adj[u].insert(v);
		adj[v].insert(u);

		edges.push_back({ 0,w,u,v });

		G[u][v] = w;
	}

	rezG = new int* [V];
	for (int i = 0; i < V; i++) {
		rezG[i] = new int[V];
		for (int j = 0; j < V; j++)
			rezG[i][j] = G[i][j];
	}

	current = new int[V];

	fin.close();
}


/* --- FORD FULKERSON METHOD USING EDMONDS-KARP ALGORITHM --- */
bool Graph::BFS(int** rezG, int S, int T, int* parent) {
	bool* visited = new bool[V];
	memset(visited, false, sizeof(bool) * V);

	queue<int> Q;
	Q.push(S);

	visited[S] = true;
	parent[S] = -1;

	while (!Q.empty()) {
		int u = Q.front();
		Q.pop();

		for (int v = 0; v < V; v++) {
			if (visited[v] == false && rezG[u][v] > 0) {
				if (v == T) {
					parent[v] = u;
					return true;
				}

				Q.push(v);
				parent[v] = u;
				visited[v] = true;
			}
		}
	}

	return false;
}


int Graph::FordFulkerson() {
	int S = 0;
	int T = V - 1;

	int maxFlow = 0;

	while (BFS(rezG, S, T, parent)) {
		int pathFlow = INT_MAX;
		for (int v = T; v != S; v = parent[v]) {
			int u = parent[v];
			if (rezG[u][v] < pathFlow) {
				pathFlow = rezG[u][v];
			}
		}

		for (int v = T; v != S; v = parent[v]) {
			int u = parent[v];
			rezG[u][v] -= pathFlow;
			rezG[v][u] += pathFlow;
		}

		maxFlow += pathFlow;
	}

	fout << maxFlow;

	fin.close();

	return maxFlow;

}
/* ------------------------------------------------------- */


/* --- PUSH-RELABEL AND RELABEL-TO-FRONT ALGORITHMS --- */
void Graph::pushPreflow(int S) {
	vertices[S].h = vertices.size();

	for (int i = 0; i < edges.size(); i++) {
		if (edges[i].u == S) {
			edges[i].flow = edges[i].capacity;
			vertices[edges[i].v].excess = edges[i].capacity;
			vertices[edges[i].u].excess -= edges[i].capacity;
			edges.push_back({ -edges[i].flow, 0, edges[i].v, S });
		}
	}
}

int Graph::overflowVertex(vector<Vertex>& ver) {
	for (int i = 1; i < ver.size() - 1; i++) {
		if (ver[i].excess > 0)
			return i;

	}

	return -1;
}

void Graph::updateReverseEdgeFlow(int i, int flow) {
	int u = edges[i].v;
	int v = edges[i].u;

	for (int j = 0; j < edges.size(); j++) {
		if (edges[j].v == v && edges[j].u == u) {
			edges[j].flow -= flow;
			return;
		}
	}

	Edge e = { 0,flow,u,v };
	edges.push_back(e);
}

bool Graph::push(int u) {
	for (int i = 0; i < edges.size(); i++) {
		if (edges[i].u == u) {
			if (edges[i].flow == edges[i].capacity)
				continue;

			if (vertices[u].h > vertices[edges[i].v].h) {
				int flow = min(edges[i].capacity - edges[i].flow, vertices[u].excess);
				vertices[u].excess -= flow;
				vertices[edges[i].v].excess += flow;
				edges[i].flow += flow;
				updateReverseEdgeFlow(i, flow);

				return true;
			}
		}
	}

	return false;
}

void Graph::relabel(int u) {
	int minHeight = INT_MAX;
	for (int i = 0; i < edges.size(); i++) {
		if (edges[i].u == u) {
			if (edges[i].flow == edges[i].capacity)
				continue;

			if (vertices[edges[i].v].h < minHeight) {
				minHeight = vertices[edges[i].v].h;
				vertices[u].h = minHeight + 1;
			}
		}
	}
}


void Graph::pushFlow(int u, int v) {
	for (int i = 0; i < edges.size(); i++) {
		if (edges[i].u == u && edges[i].v == v) {
			int deltaFlow = min(edges[i].capacity - edges[i].flow, vertices[u].excess);
			vertices[u].excess -= deltaFlow;
			vertices[edges[i].v].excess += deltaFlow;
			edges[i].flow += deltaFlow;
			updateReverseEdgeFlow(i, deltaFlow);
		}
	}
}

void Graph::discharge(int u) {
	while (vertices[u].excess > 0) {
		if (current[u] == V) {
			relabel(u);
			current[u] = 0;

			continue;
		}

		int v = current[u];
		bool pushed = false;
		for (int i = 0; i < edges.size() && pushed == false; i++) {
			if (edges[i].u == u && edges[i].v == v) {
				if (edges[i].capacity - edges[i].flow > 0 && vertices[u].h == vertices[v].h + 1) {
					pushed = true;
					pushFlow(u, v);
				}
			}
		}

		if (pushed == false)
			current[u]++;
	}
}


int Graph::PushRelabel() {
	int S = 0;
	int T = V - 1;

	pushPreflow(S);
	while (overflowVertex(vertices) != -1) {
		int u = overflowVertex(vertices);
		if (!push(u))
			relabel(u);
	}

	int maxFlow = vertices.back().excess;

	fout << maxFlow;

	return maxFlow;
}

int Graph::RelabelToFront() {
	int S = 0;
	int T = V - 1;

	pushPreflow(S);
	list<int> L;
	L.clear();

	for (int i = 0; i < vertices.size(); i++) {
		if (i != S && i != T) {
			L.push_back(i);
			current[i] = 0;
		}
	}

	auto vertex = L.begin();
	while (vertex != L.end()) {
		int u = *vertex;
		int oldHeight = vertices[u].h;

		discharge(u);

		if (vertices[u].h > oldHeight) {
			L.splice(L.begin(), L, vertex);
		}

		vertex++;
	}

	int maxFlow = vertices[T].excess;

	fout << maxFlow;

	return maxFlow;
}
/* ----------------------------------------------------- */


/* --- EULERIAN CIRCUIT ALGORITHMS --- */
void Graph::Hierholzer(int s) {
	stack<int> S;
	S.push(s);

	while (!S.empty()) {
		int u = S.top();
		if (adj[u].size() != 0) {
			int v = *adj[u].begin();
			adj[u].erase(v);
			adj[v].erase(u);
			S.push(v);

			continue;
		}

		S.pop();
		circuit.push_back(u);
	}

	circuit.pop_back();

	for (auto& i : circuit) {
		fout << i << " ";
	}

	fout.close();
}

void Graph::findEluerianCircuit(vector<int>& L, vector<vector<int>>& G, int u) {
	for (int v = 0; v < V; v++) {
		if (G[u][v] != 0) {
			G[u][v] = G[v][u] = 0;
			findEluerianCircuit(L, G, v);

		}
	}

	L.emplace_back(u);
}


void Graph::Fleury() {
	vector<Edge> edges;

	fin >> V >> E;

	vector<vector<int>> G(V);
	for (int i = 0; i < V; i++)
		G[i] = vector<int>(V, 0);

	for (int i = 0; i < E; i++) {
		int u, v;
		fin >> u >> v;
		G[u][v] = G[v][u] = 1;
	}


	vector<int> L;
	findEluerianCircuit(L, G, 0);

	for (auto& u : L)
		fout << u << " ";

	fout.close();

}
/* ------------------------------------ */


int main() {
	Graph G{ "in-Flux.txt", "out-Flux.txt"};

	//G.FordFulkerson();
	//G.PushRelabel();
	G.RelabelToFront();

	//G.Hierholzer(0);
	//G.Fleury();

	return 0;
}