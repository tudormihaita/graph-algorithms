#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <stack>

using namespace std;

#define INF 999999
#define NIL -1

struct edge {
	int src, dest, weight;
};

class Graph {
private:
	string input;
	int V, E;
	vector<pair<int, int>>* adj;
	vector<pair<int, int>>* adjT;
	int** mat;
	vector<edge> edges;
	vector<int> viz;
	stack<int> stack;
public:
	Graph(string input);
	~Graph() {
		delete[] this->adj;
	}
	void DFS(int vertex, int val, vector<pair<int, int>>* &adj);
	void BFS(int src);
	void Moore(int src, int dest);
	void Kosaraju();
	vector<int> BellmanFord(int V, vector<edge> edges, int src);
	vector<int> Dijkstra(int V, vector<pair<int, int>>* adj, int src);
	bool Johnson();
};


Graph::Graph(string input) {
	this->input = input;

	ifstream fin(input);

	int vertices, edges;
	fin >> vertices >> edges;

	this->V = vertices; this->E = edges;

	this->adj = new vector<pair<int, int>>[this->V+1];
	this->adjT = new vector<pair<int, int>>[this->V+1];
	mat = new int*[this->V];
	for (int i = 0; i < this->V; i++)
		mat[i] = new int[this->V];

	int u, v, w;
	while (fin >> u >> v >> w) {
		adj[u].push_back({ v,w });
		adjT[v].push_back({ u,w });
		mat[u][v] = 1;
		this->edges.push_back({ u,v,w });
	}

	fin.close();
}


void Graph::BFS(int src) {
	this->viz = vector<int>(V + 1, 0);
	queue<int> coada;
	vector<int> dist(V + 1, INF);

	viz[src] = 1;
	dist[src] = 0;
	coada.push(src);

	while (!coada.empty()) {
		int curent = coada.front();
		cout << curent << " | " << dist[curent] << "\n";
		coada.pop();
		for (auto& i : this->adj[curent]) {
			if (viz[i.first] == 0) {
				viz[i.first] = 1;
				dist[i.first] = dist[curent] + 1;
				coada.push(i.first);
			}
		}
	}

	cout << "\n";

}

void Graph::DFS(int vertex, int val, vector<pair<int, int>>* &adj) {
	viz[vertex] = val;
	for (auto i : adj[vertex]) {
		int node = i.first;
		if (viz[node] == 0)
			DFS(node, val, adj);
	}
	stack.push(vertex);
}


void Graph::Moore(int src, int dest) {
	vector<int> dist(V + 1, INF);
	vector<int> parent(V + 1, NIL);

	dist[src] = 0;
	parent[src] = -1;

	queue<int> coada;
	coada.push(src);

	while (!coada.empty()) {
		int curent = coada.front();
		coada.pop();
		for (auto& i : this->adj[curent]) {
			if (dist[i.first] == INF) {
				dist[i.first] = dist[curent] + 1;
				parent[i.first] = curent;
				coada.push(i.first);
			}
		}
	}
	if (dist[dest] == INF) {
		cout << "Nu exista drum intre cele 2 noduri!\n";
		return;
	}

	vector<int> drum;
	int curent = dest;
	while (parent[curent] != -1) {
		drum.insert(drum.begin(), curent);
		curent = parent[curent];
	}

	cout << "Drumul cel mai scurt de la nodul " << src << " la nodul " << dest << " este:\n";
	for (int i = 0; i < drum.size(); i++)
		cout << drum[i] << " ";
	cout << "\n";
}

void Graph::Kosaraju() {
	this->viz = vector<int>(V + 1, 0);
	int val = 0;
	for (int i = 0; i < V; i++)
		if (viz[i] == 0) {
			val++;
			DFS(i, val, this->adj);
		}

	int nr = 0;
	val = 0;
	this->viz = vector<int>(V + 1, 0);
	for (int i = 0; i < V; i++) {
		int node = stack.top();
		stack.pop();
		if (viz[i] == 0) {
			nr++;
			val++;
			DFS(i, val, this->adjT);
		}
	}

	cout << "Numarul de componente conexe este: " << nr << "\n";

}

vector<int> Graph::BellmanFord(int V, vector<edge> edges, int src) {
	vector<int> dist(V + 1, INF);
	vector<int> parent(V + 1, NIL);

	dist[src] = 0;
	parent[src] = -1;

	for (int i = 0; i < V - 1; i++) {
		for (auto& i : edges) {
			int u = i.src;
			int v = i.dest;
			int w = i.weight;
			if (dist[u] != INF && dist[v] > dist[u] + w) {
				dist[v] = dist[u] + w;
				parent[v] = u;
			}
		}
	}

	for (auto& i : edges) {
		int u = i.src;
		int v = i.dest;
		int w = i.weight;
		if (dist[u] != INF && dist[v] > dist[u] + w) {
			return {};
		}
	}

	return dist;

	/*cout << "Costuri minime drumuri plecand de la nodul sursa " << src << ":\n";
	for (int i = 0; i < V; i++) {
		if (dist[i] == INF) {
			cout << "INF" << " ";
		}
		else
		cout << dist[i] << " ";
	}
	cout << "\n";*/

}

vector<int> Graph::Dijkstra(int V, vector<pair<int, int>>* adj, int src) {
	vector<int> dist(V + 1, INF);
	vector<int> parent(V + 1, NIL);

	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> coada;
	
	dist[src] = 0;
	parent[src] = -1;
	coada.push({ dist[src], src});

	while (!coada.empty()) {
		int u = coada.top().second;
		coada.pop();
		for (auto& i : this->adj[u]) {
			int v = i.first;
			int w = i.second;
			if (dist[u] != INF && dist[v] > dist[u] + w) {
				dist[v] = dist[u] + w;
				parent[v] = u;
				coada.push({ dist[v] , v });
			}
		}
	}

	/*cout << "Costuri minime drumuri plecand de la nodul sursa " << src << ":\n";
	for (int i = 0; i < V; i++) {
		if (dist[i] == INF) {
			cout << "INF" << " ";
		}
		else
			cout << dist[i] << " ";
	}
	cout << "\n";*/

	return dist;
}


bool Graph::Johnson() {
	vector<edge> new_edges = this->edges;
	for (int i = 0; i < V; i++) {
		edge new_edge;
		new_edge.src = V;
		new_edge.dest = i;
		new_edge.weight = 0;
		new_edges.push_back(new_edge);
	}

	vector<int> new_weights = this->BellmanFord(V + 1, new_edges, V);

	if (new_weights.size() == 0) {
		cout << "-1\n";
		return false;
	}

	for (int i = 0; i < V; i++) {
		for (auto& j : this->adj[i]) {
			j.second = j.second + new_weights[i] - new_weights[j.first];
			cout << i << " " << j.first << " " << j.second << "\n";
		}
	}

	for (int i = 0; i < V; i++) {
		vector<int> dist = this->Dijkstra(V, this->adj, i);
		for (int j = 0; j < V; j++) {
			if (dist[j] == INF)
				cout << "INF" << " ";
			else {
				dist[j] = dist[j] - new_weights[i] + new_weights[j];
				cout << dist[j] << " ";
			}
		}
		cout << "\n";
	}

}

int main() {
	Graph G("in.txt");
	G.Kosaraju();
	G.BFS(7);
	G.Moore(0, 7);
	//G.Johnson();
	return 0;
}