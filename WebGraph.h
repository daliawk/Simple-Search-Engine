#ifndef GRAPH
#define GRAPH
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// data structure to store graph edges
struct Edge {
	int src, dest;
};



//Data structure to carry value of CTR
struct CTR {
	float impressions;
	float visits;
};



//A class for websites
class Website {
private:
	int inputOrder; //order of the website in the list
	string webName; //website's name
	vector<string> keywords; //The keywords of the website
	CTR ctr; //The websites CTR
	float score; //The score if the website

public:
	float pagerank; //The page rank of the website
	int pointers; //The number of outgoing edges from the website

	Website() { inputOrder = -1; webName = " "; ctr = { 0,0 }; pagerank = -1; score = -1; pointers = 0; }
	string getName() { return webName; }
	void PushKeyword(string key) { keywords.push_back(key); } //Adds keywords
	void Initialise(string name, int order) { webName = name; inputOrder = order; } //Initializes the name and order
	void WriteImpression(int impression) { ctr.impressions = impression; } //Sets the number of impressions
	void SetPR(float pr) { pagerank = pr; CalculateScore(); } //Sets the PR and calculates the score if it's not the first run
	void SetVisits(int v) { ctr.visits = v; } //Sets the number of visits if it's not the first run
	void PrintKeywords() { cout << webName << ": "; for (int i = 0; i < keywords.size(); i++) cout << keywords[i] << " "; cout << endl; }
	bool KeywordExist(string key) { for (int i = 0; i < keywords.size(); i++) if (keywords[i] == key) return true; return false; } //If the website has this keyword
	void IncrementImpressions() { ctr.impressions++; CalculateScore(); } //Increments the number of impressions and updates the score
	void IncrementVisits() { ctr.visits++; CalculateScore(); } //Increments the number of visits and updates the score
	void CalculateScore() { score = pagerank + 0.06 * (ctr.visits - ctr.impressions*pagerank) / (1 + 0.1 * ctr.impressions); } //Calculating the score
	float getScore() { return score; }
	int getImpression() { return ctr.impressions; }
	int getVisits() { return ctr.visits; }
	float getPR() { return pagerank; }
};



//class to represent a graph object
class WebGraph
{
public:
	vector<vector<bool>> adjMatrix; //The adjacency matrix, where "true" means there is an edge from this row value to this column value
	vector<Website> sources; //The list of websites
	vector<Edge> edges; //The list of edges

	// Graph Constructor
	WebGraph(vector<Edge> const& e, vector<Website>& webArray)
	{
		//Setting the size of adjacency matrix
		adjMatrix.resize(webArray.size());
		for (int i = 0; i < webArray.size(); i++) {
			adjMatrix[i].resize(webArray.size());
		}

		sources.resize(webArray.size());
		edges = e;
		sources = webArray;

		// add edges to the directed graph
		for (auto& edge : e)
		{
			adjMatrix[edge.src][edge.dest]=true;
			sources[edge.src].pointers++;
		}
	}

	// print adjacency list representation of graph
	void printGraph()
	{
		for (int i = 0; i < sources.size(); i++)
		{
			cout << sources[i].getName() << " --> ";

			for (int v=0; v < sources.size(); v++) {
				if(adjMatrix[i][v])
				cout << sources[v].getName() << " ";
			}
			cout << endl;
		}
	}
	

	void CalculatePR() { //Calculating the pagerank of each website
		int n = sources.size();

		for(int i=0; i<sources.size(); i++){ //initializing the pagerank
			sources[i].pagerank = 1.0 / n;
		}

		float d = 0.85; //Damping Factor

		for (int i = 0; i < n; i++) {
			vector <float> sum(sources.size(), 0);

			for (int j = 0; j < sources.size(); j++) {
				for (int k = 0; k < sources.size(); k++) {
					if (adjMatrix[k][j]) {
						sum[j] += sources[k].pagerank / sources[k].pointers;
					}
				}

			}

			for (int m = 0; m < sources.size(); m++) {
				sources[m].pagerank = (1-d)/n + d*sum[m];
			}
			
		}

		for (int j = 0; j < sources.size(); j++) {
			sources[j].CalculateScore();
		}
		
	}

	vector<int> PrintWebsiteIncludingAll(vector<string> key) { //Gets websites which have ALL the keywords
		vector<int> position;

		if (!key.empty()) {
			for (int i = 0; i < sources.size(); i++) {
				bool include = false;


				int m = 0;
				do {
					include = sources[i].KeywordExist(key[m]);
					++m;
				} while (include && m < key.size()); //Exit if one keyword is missing or finished list

				if (include) { //If it inludes all keywords
					position.push_back(i);
				}
			}

			if (position.empty()) {
				cout << "There are no websites that include all these keywords!\n";
			}
			else {
				this->heap_sort(position, position.size()); //Sorts the search results according to score using heap sort
			}
		}
		return position;
	}

	vector<int> PrintWebsiteIncludingEither(vector<string> key) { //Gets all websites including ATLEAST ONE keyword
		vector<int> position;

		if (!key.empty()) {
			for (int i = 0; i < sources.size(); i++) {
				bool include = false;

				int m = 0;
				do {
					include = sources[i].KeywordExist(key[m]);
					++m;
				} while (!include && m < key.size()); //Exit if found one keyword or finished list

				if (include) { //If includes at least one keyword
					position.push_back(i);
				}
			}

			if (position.empty()) {
				cout << "There are no websites that include any these keywords!\n";
			}
			else {
				this->heap_sort(position, position.size()); //Sorts the search results according to score using heap sort
			}
		}
		return position;
	}
	
	void heapify(vector<int>& arr, int n, int i)
	{
		int smallest = i;
		int l = 2 * i + 1;
		int r = 2 * i + 2;

		if (l < n && sources[arr[l]].getScore() < sources[arr[smallest]].getScore())
			smallest = l;

		if (r < n && sources[arr[r]].getScore() < sources[arr[smallest]].getScore())
			smallest = r;

		if (smallest != i) {
			swap(arr[i], arr[smallest]);
			heapify(arr, n, smallest);
		}
	}

	void build_heap(vector<int>& arr, int n)
	{
		for (int i = n / 2 - 1; i >= 0; i--)
			heapify(arr, n, i);
	}

	void heap_sort(vector<int>& arr, int n)
	{
		build_heap(arr, n);

		for (int i = n - 1; i >= 0; i--) {
			swap(arr[0], arr[i]);
			heapify(arr, i, 0);
		}
	}


};

#endif
