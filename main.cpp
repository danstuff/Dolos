#include "main.h"

struct connection{
	uint m1, m2;	//first and second message indices 
	uint strength;	//link strength (on a semi-arbitrary scale)
};

vector<string> messages;
vector<connection> connection_map;

void load(string filename){
	fstream file;
	file.open(filename, ios::in);

	//go through the whole file
	while(!file.eof()){
		string temp = "";
		getline(file, temp);

		//if the line does begins with '@' it's a link, so store it in connection_map
		if(temp.c_str()[0] == '@'){
			uint mesg1, mesg2, strength;

			sscanf_s(temp.c_str(), "@ %i %i %i", &mesg1, &mesg2, &strength);

			connection nc;
			nc.m1 = mesg1;
			nc.m2 = mesg2;
			nc.strength = strength;

			connection_map.push_back(nc);
		}
		//otherwise, it's a message, so store it in messages
		else{
			messages.push_back(temp);
		}
	}

	file.close();
}
void save(string filename){
	fstream file;
	file.open(filename, ios::out);

	file.clear();

	//store all data in the same format it was read
	for(uint i = 0; i < messages.size(); i++){
		file << messages[i] << endl;
	}

	for(uint i = 0; i < connection_map.size(); i++){
		file << "@ " << connection_map[i].m1 << " " << connection_map[i].m2;
		file << " " << connection_map[i].strength << endl;
	}

	file.close();
}

double compare(string str1, string str2){
	uint length = str1.length() > str2.length() ? 
		str1.length() : str2.length();

	uint matches = 0;

	for(uint i = 0; i < length; i++){
		if(str1.c_str()[i] == str2.c_str()[i])
			matches++;
	}

	return (double(matches) / double(length))*100;
}
void boost(uint m1, uint m2){
	assert(m1 < messages.size() && m2 < messages.size());

	//add one to the strength m1 and m2 already have a connection
	for(uint i = 0; i < connection_map.size(); i++){
		if(connection_map[i].m1 == m1 && connection_map[i].m2 == m2){
			connection_map[i].strength++;
			return;
		}
	}

	//if not, make a new connection between them
	connection ncnct;
	ncnct.m1 = m1;
	ncnct.m2 = m2;
	ncnct.strength = 1;

	connection_map.push_back(ncnct);
}

int main(){
	load("data.dat");

	uint old_response = 0;

	while(true){
		//get user input
		string user_entry = "";

		cout << "> ";
		getline(cin, user_entry);

		//if a special code is input, save and quit
		if(user_entry == "KILL") break;

		//search through all the connections, rate each, and pick the best
		connection best_connection;
		double best_rating = 0;

		for(uint i = 0; i < connection_map.size(); i++){
			double likeness = compare(messages[connection_map[i].m1], user_entry);
			double strength = connection_map[i].strength;

			double rating = pow(0.2*likeness, 2) + strength;

			if(rating > best_rating){
				best_rating = rating;
				best_connection = connection_map[i];
			}
		}

		//if the user entry was not easily matched
		if(best_rating < 100){
			cout << "Um... ";

			//Add it to messages and connect to last response
			messages.push_back(user_entry);
			boost(old_response, messages.size()-1);
		}
		else{
			//boost best_connection
			boost(best_connection.m1, best_connection.m2);
		}

		//print the response
		cout << messages[best_connection.m2] << endl;

		//record the response for later use
		old_response = best_connection.m2;
	}

	save("data.dat");

	return 0;
}