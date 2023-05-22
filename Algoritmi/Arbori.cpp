#include "Utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>

using namespace std;

class Graph {
private:
	int V=0, E=0;
	vector<Edge> edges;

	ifstream fin;
	ofstream fout;

public:
	Graph(string input, string output) : fin{ input }, fout{ output } {};

	~Graph() {
		fin.close();
		fout.close();
	}

	//ALGORITMI DE CODARE SI DECODARE
	void PruferEncoding();
	void PruferDecoding();
	void HuffmanEncoding();
	void HuffmanDecoding();

	//ALGORITMUL LUI KRUSKAL PENTRU DETERMINAREA ARBORELUI MINIM DE ACOPERIRE
	void KruskalMST();

};


void Graph::PruferEncoding(){
	Tree T;

	fin >> T.N;

	T.deg = new int[T.N];
	for (int i = 0; i < T.N; i++) {
		T.deg[i] = 0;
	}

	T.parent = new int[T.N];
	for (int i = 0; i < T.N; i++) {
		fin >> T.parent[i];
		T.deg[i]++;
		if (T.parent[i] != -1)
			T.deg[T.parent[i]]++;
	}

	for (int i = 0; i < T.N; i++) {
		if (T.deg[i] == 1 && T.parent[i] != -1) {
			T.leafs.insert(i);
		}
	}

	int* code = new int[T.N - 1];

	for (int i = 0; i < T.N - 1; i++) {
		int minLeaf = *(T.leafs.begin());
		T.leafs.erase(T.leafs.begin());

		int minLeafP = T.parent[minLeaf];
		code[i] = minLeafP;
		T.deg[minLeaf]--;
		T.deg[minLeafP]--;


		if (T.parent[minLeafP] != -1 && T.deg[minLeafP] == 1) {
			T.leafs.insert(minLeafP);
		}
	}

	fout << T.N - 1 << "\n";
	for (int i = 0; i < T.N - 1; i++) {
		fout << code[i] << " ";
	}

	delete[] T.deg;
	delete[] T.parent;
	delete[] code;
}


void Graph::PruferDecoding() {
	Tree T;

	fin >> T.N;
	T.deg = new int[T.N + 1];
	T.parent = new int[T.N + 1];

	for (int i = 0; i < T.N+1; i++) {
		T.leafs.insert(i);
	}

	list<int> code;
	for (int i = 0; i < T.N; i++) {
		int x;
		fin >> x;
		code.push_back(x);
		T.deg[x]++;
		T.leafs.erase(x);
	}

	for (int i = 0; i < T.N; i++) {
		int minLeaf = *(T.leafs.begin());
		T.leafs.erase(T.leafs.begin());
		int minLeafP = code.front();

		T.parent[minLeaf] = minLeafP;
		T.deg[minLeafP]--;
		if (T.deg[minLeafP] == 0) {
			T.leafs.insert(minLeafP);
		}

		code.pop_front();
	}

	T.parent[(*T.leafs.begin())] = -1;

	fout << T.N + 1 << "\n";
	for (int i = 0; i < T.N + 1; i++) {
		fout << T.parent[i];
	}

	delete[] T.deg;
	delete[] T.parent;
}


void Graph::HuffmanEncoding() {
	string text;
	getline(fin, text);

	if (text == "") {
		return;
	}

	int freq[256]{};
	int letters = 0;

	for (char chr : text) {
		freq[int(chr)]++;
		if (freq[int(chr)] == 1)
			letters++;
	}

	multimap<FreqPair, string> Q;

	fout << letters << "\n";
	for (int i = 0; i < 256; i++) {
		if (freq[i]) {
			fout << char(i) << " " << freq[i] << "\n";
			Q.insert({ {freq[i], char(i)}, string(1,i) });
		}
	}

	string code[256];
	while (--letters) {
		auto x = *Q.begin();
		for (char chr : x.second) {
			code[int(chr)] = "0" + code[int(chr)];
		}

		Q.erase(Q.begin());

		auto y = *Q.begin();
		for (char chr : y.second) {
			code[int(chr)] = "1" + code[int(chr)];
		}

		Q.erase(Q.begin());

		Q.insert({ {x.first.freq + y.first.freq, min(x.first.chr, y.first.chr)}, x.second + y.second });
	}

	for (char chr : text) {
		fout << code[int(chr)];
	}

}

void Graph::HuffmanDecoding() {
	multimap<FreqPair, string> Q;

	int letters, freq[256]{};
	char ch;
	string line;

	getline(fin, line);
	letters = stoi(line);

	for (int i = 0; i < letters; i++) {
		getline(fin, line);
		ch = line[0];
		freq[int(ch)] = stoi(line.substr(2));
		//cout << ch << " " << freq[int(ch)] << "\n";
		Q.insert({ { freq[int(ch)], ch }, string(1, ch) });
	}

	string encoded;
	getline(fin, encoded);

	string text;
	map<string, char> codes;

	string code[256]{};

	while (--letters) {
		auto x = *Q.begin();
		for (auto chr : x.second) {
			code[int(chr)] = "0" + code[int(chr)];
		}

		Q.erase(Q.begin());

		auto y = *Q.begin();
		for (auto chr : y.second) {
			code[int(chr)] = "1" + code[int(chr)];
		}

		Q.erase(Q.begin());

		Q.insert({ {x.first.freq + y.first.freq, min(x.first.chr, y.first.chr)}, x.second + y.second });
	}

	for (int i = 0; i < 256; i++) {
		if (code[i] != "\0") {
			codes.insert({ code[i], i });
			cout << code[i] << " " << char(i) << "\n";
		}
	}

	string currentCode;
	int ind = 0;
	bool found = false;

	while (!encoded.empty()) {
		found = false;
		currentCode += encoded[ind];

		for (auto& code : codes) {
			if (currentCode == code.first) {
				text.append(string(1, code.second));
				//cout << text << "\n";
				encoded.erase(encoded.find(currentCode), currentCode.length());
				currentCode.clear();
				ind = 0;
				found = true;
				break;
			}
		}

		if (found == false)
			ind++;
	}

	fout << text;
}


void Graph::KruskalMST() {
	fin >> V >> E;
	for (int i = 0; i < E; i++) {
		int u, v, w;
		fin >> u >> v >> w;
		edges.push_back({ u,v,w });
	}

	//SORTARE MUCHII IN FUNCTIE DE PONDERI
	sort(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) {
		return e1.weight < e2.weight;
		});

	vector<Edge> result;
	int minCost = 0;

	DSU S(this->V);

	for (auto edge : edges) {
		int x = edge.src;
		int y = edge.dest;
		int w = edge.weight;

		if (S.findSet(x) != S.findSet(y)) {
			S.unionSet(x, y);
			minCost += w;
			result.push_back(edge);
		}
	}

	fout << minCost << "\n";
	fout << result.size() << "\n";

	sort(result.begin(), result.end(), [](const Edge& e1, const Edge e2) {
		if (e1.src == e2.src)
			return e1.dest < e2.dest;
		else
			return e1.src < e2.src;
		});

	for (auto edge : result) {
		fout << edge.src << " " << edge.dest << "\n";
	}

}

int main(int argc, char** argv) {
	Graph G{ "in-Arbori.txt", "out-Arbori.txt" };

	//G.PruferEncoding();
	//G.PruferDecoding();
	//G.HuffmanEncoding();
	//G.HuffmanDecoding();

	G.KruskalMST();

	return 0;
}