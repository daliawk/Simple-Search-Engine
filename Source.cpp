#include <iostream>
#include <fstream>
#include <sstream>
#include "WebGraph.h"
using namespace std;

void NewSearch(WebGraph& g);
int FindWebsite(string& name, vector<Website>& arr);
void SearchMenu(WebGraph& g);
bool OpenWebPage(vector<int>& list, WebGraph& g);
bool isNumber(string x);

int main() {

	ifstream graphFile;
	ifstream keywordFile;
	ifstream impressionFile;
	vector<Website> listOfWebsites;

	//Openning graph file
	graphFile.open("Web Graph File.txt");
	if (graphFile.fail()) {
		cout << "Error! Could not open the web graph file!\n";
		exit(1);
	}

	//Openning Keywords file
	keywordFile.open("Keyword File.txt");
	if (keywordFile.fail()) {
		cout << "Error! Could not open the keyword file!\n";
		exit(1);
	}

	//Openning Impressions File
	impressionFile.open("Impression File.txt");
	if (impressionFile.fail()) {
		cout << "Error openning impressions file!\n";
		exit(1);
	}

	//Reading the keyword file
	string line, text;
	int webNum = 0;
	while (!keywordFile.eof()) { //A loop to read each sentence in the file
		getline(keywordFile, line); //Takes a sentence from the file

		stringstream sentence(line); 

		bool webName = true; //To detect if we are reading the name of the website which has the keywords
		listOfWebsites.resize(webNum + 1);

		while (getline(sentence, text, ',')) {
			if (webName) {
				listOfWebsites[webNum].Initialise(text, webNum); //Adds website int the list of websites and initializes it
				webName = false; //A flag that the next words are keywords
			}
			else {
				listOfWebsites[webNum].PushKeyword(text); //Adds keywords
			}
		}

		++webNum;
	}



	//Reading the Graph file
	vector<Edge> hyperlinks; 
	int numEdges = 0;
	while (!graphFile.eof()) {
		getline(graphFile, line);

		stringstream sentence(line);

		hyperlinks.resize(numEdges + 1);

		//Reading and adding the source of the edge
		getline(sentence, text, ',');
		hyperlinks[numEdges].src = FindWebsite(text, listOfWebsites);

		//Reading and adding the destination of the edge
		getline(sentence, text);
		hyperlinks[numEdges].dest = FindWebsite(text, listOfWebsites);

		++numEdges;
	}

	//Creating the graph
	WebGraph graph(hyperlinks, listOfWebsites);

	//Reading the Impression File
	bool firstRun = true;
	while (!impressionFile.eof()) {
		getline(impressionFile, line);

		stringstream sentence(line);

		int i = 0; //counter for number of words read from each sentence
		int position; //The order of the website written in the sentence
		while (!sentence.eof()) {
			getline(sentence, text, ',');

			if (i == 0) {
				position = FindWebsite(text, listOfWebsites); //Getting which website written in the sentence
			}
			else if (i == 1) {
				graph.sources[position].WriteImpression(stoi(text)); //Adding the number of impressions of the website
			}
			else if (i == 2) { //if there is still text in the sentence after reading the web name and impressions, then this is not the first run and we need to read more values
				graph.sources[position].SetVisits(stoi(text)); //Adding the number of visits on the website
				firstRun = false;

			}
			else if (i == 3) {
				graph.sources[position].SetPR(stof(text)); //Adding the pagerank of the website
			}

			i++;
		}

		
		++webNum;
	}

	if (firstRun) { //If this is the first run, then we should calculate the pagerank
		graph.CalculatePR();
	}


	cout << "Welcome!\n";
	SearchMenu(graph); //This function is the search engine and we don't return to main until the user chooses exit

	//Updating the websites
	ofstream writeImpression;
	writeImpression.open("Impression File.txt");
	if (writeImpression.fail()) {
		cout << "Error loading file";
		exit(1);
	}

	for (int i = 0; i < graph.sources.size(); i++) {
		writeImpression << graph.sources[i].getName() << "," << graph.sources[i].getImpression() << "," << graph.sources[i].getVisits()<< "," << graph.sources[i].getPR() << endl;
	}

	return 0;
}


int FindWebsite(string& name, vector<Website>& arr) {//Finds website order in the vector using its name
	for (int i = 0; i < arr.size(); i++) {
		if (arr[i].getName() == name)
			return i;
	}
}

void SearchMenu(WebGraph& g) {
	string choice; //the value is string in case the user enters a string by mistake 

	cout << "What would you like to do?\n" << "1. New Search\n" << "2. Exit\n\n" << "Type your choice: ";
		
	cin >> choice;


	while (!isNumber(choice) ||(stoi(choice) != 1 && stoi(choice) != 2)) { //Checks if the choice is string or out of range in order to re-enter
		cout << "Invalid choice!!\n\n";
		cout << "What would you like to do?\n" << "1. New Search\n" << "2. Exit\n\n" << "Type your choice: ";
		cin >> choice;
	}

	int correctChoice = stoi(choice); //Gets the integar value after ensuring that it's not a string or out of range

	switch (correctChoice)
	{
	case 1: //New Search
		NewSearch(g);
		break;
	case 2: //Exit
		cout << "Thank you for using our program!\n";
	}
}

void NewSearch(WebGraph& g) {

	string searchQuery; //The user's search query
	cout << endl;
	cout << "Search for: ";
	cin.ignore(256, '\n');
	getline(cin, searchQuery); //Got string query

	
	stringstream sentence(searchQuery);
	vector<string> words; //The keywords the user searching for
	int keywordNum = 0;
	bool AND = false; //If the user wrote "AND", since "OR" is the default

	//Analyzing string query
	if (searchQuery[0] == '"') {//If the user used quotation

		searchQuery.erase(searchQuery.begin()); //Erasing first quotation mark
		searchQuery.pop_back(); //Erasing last quotation mark

		//There will be only one keyword which is the whole query
		words.resize(1);
		words[0] = searchQuery;
	}
	else { //If there are no quotations, we must read each word seperately

		while (!sentence.eof()) { 
			words.resize(keywordNum + 1);
			getline(sentence, words[keywordNum], ' '); //Get one word from string query

			if (words[keywordNum] == "AND") { //if there is AND between search words
				AND = true;
				--keywordNum; //Removing "AND" from the keywords we will search for
			}
			else if (words[keywordNum] == "OR") { //if there is OR between the search words
				--keywordNum;
			}
			

			++keywordNum;
			//If there is "AND" or "OR", this element in the vector will be replaced by the next search word
		}
	}

	//Getting search results
	vector<int> results;
	if (AND) {
		 results = g.PrintWebsiteIncludingAll(words);;
	}
	else {
		results = g.PrintWebsiteIncludingEither(words);
	}


	if (results.empty()) { //If there are no websites with these keywords
		cout << endl;
		SearchMenu(g);
	}
	else {

		string choice; //the value is string in case the user enters a string by mistake 
		bool back = false; //If the user visited a website and wants to go back to previous results

		do {
			cout << endl;
			cout << "Search Results: \n";
			for (int j = 0; j < results.size(); j++) {
				cout << j + 1 << ". " << g.sources[results[j]].getName() << " " << endl;
				g.sources[results[j]].IncrementImpressions(); //Incrementing the impressions of each website printed
			}

			cout << endl;
			cout << "Would you like to\n" << "1. Choose a webpage to open\n" << "2. New search\n" << "3. Exit\n" << endl;
			cout << "Type in your choice: ";
			
			cin >> choice;
			cout << endl;


			while (!isNumber(choice) || stoi(choice) <= 0 || stoi(choice) > 4) { //Checks if the choice is string or out of range in order to re-enter
				cout << "Invalid Choice!!\n";
				cout << "Type in your choice: ";
				cin >> choice;
			}

			int correctChoice = stoi(choice); //Gets the integar value after ensuring that it's not a string or out of range

			switch (correctChoice)
			{
			case 1: //Open Web Page
				back = OpenWebPage(results, g); //Openning the webpage and checking if the user wants to go back
				break;
			case 2: //New Search
				NewSearch(g);
				back = false; //No need to showcase old results
				break;
			case 3: //Exit
				cout << "Thank you for using our program!!\n";
				back = false; //No need to showcase old results
			}
		} while (back);

	}
}


bool OpenWebPage(vector<int>& list, WebGraph& g) {
	string choice; //the value is string in case the user enters a string by mistake 
	int correctChoice;

	cout << "Type in which website you want: ";
	cin >> choice;

	while (!isNumber(choice) || stoi(choice) <= 0 || stoi(choice) > list.size()) { //Checks if the choice is string or out of range in order to re-enter
		cout << "Invalid Choice!! Please re-enter the value: ";
		cin >> choice;
	}

	correctChoice = stoi(choice);//Gets the integar value after ensuring that it's not a string or out of range
	cout << endl;

	cout << "Now you are viewing " << g.sources[list[correctChoice - 1]].getName() << endl;
	g.sources[list[correctChoice - 1]].IncrementVisits(); //Incrementing the number of visits of the website being visited

	
	cout << "Would you like to \n" << "1. Go back to search results\n" << "2. Start new search\n" << "3. Exit\n\n" << "Type in your choice: ";
	cin >> choice;

	while (!isNumber(choice) || stoi(choice) <= 0 || stoi(choice) > 3) { //Checks if the choice is string or out of range in order to re-enter
		cout << "Invalid Input!!\n\n";
		cout << "Would you like to \n" << "1. Go back to search results\n" << "2. Start new search\n" << "3. Exit\n\n";
		cin >> choice;
	}

	correctChoice = stoi(choice); //Gets the integar value after ensuring that it's not a string or out of range

	switch (correctChoice) {
	case 1: //Go back to search results
		return true; 
		break;
	case 2: //New search
		NewSearch(g);
		return false;
		break;
	case 3: //Exit
		cout << "Thank you for using our program!!\n";
		return false;
	}

}

bool isNumber(string x) { //Checks if the string is a number
	bool flag = true;
	int i = 0;

	while(flag && i < x.size()) { //Checks each digit if it is a number as long we are within range of string and no character have been found
		if (!isdigit(x[i])) {
			flag = false; //It is not a number
		}
		i++;
	}

	return flag;
}