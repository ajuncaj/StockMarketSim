// Project identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882
#pragma once

#include <algorithm>
#include <cassert>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <math.h>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "P2random.h"

using namespace std;

class Deployment;
class Planet;

struct command {
	bool isVerbose = false;
	bool isMedian = false;
	bool isGeneral = false;
	bool isWatcher = false;

};

struct general { // For -g mode
	int gen_ID = -1;
	uint32_t numJedi = 0;
	uint32_t numSith = 0;
	int numSurvived = 0;
};
vector<general> generals; // For -g mode
struct ForceUsers {
	uint32_t general_ID;
	uint32_t force;
	uint32_t quantity;
	uint32_t ID;
	bool sOrJ;
};
struct sithComp {
	bool operator()(const ForceUsers& sith1, const ForceUsers& sith2) {
		if (sith1.force == sith2.force) {
			return sith1.ID > sith2.ID;
		}
		return sith1.force < sith2.force;
	}
};
struct jediComp {
	bool operator()(const ForceUsers& jedi1, const ForceUsers& jedi2) {
		if (jedi1.force == jedi2.force) {
			return jedi1.ID > jedi2.ID;
		}
		return  jedi1.force > jedi2.force;
	}
};
enum class State {Initial, SeenOne, SeenBoth, MaybeBetter};
class MovieWatcher {
private:
	State seen = State::Initial;
	uint32_t sithTime;
	uint32_t jediTime;
	uint32_t maybeTime;
	uint32_t sithForce;
	uint32_t jediForce;
	uint32_t maybeForce;
	

	void update_sith(const uint32_t& force, const uint32_t& currentTime) {
		sithTime = currentTime;
		sithForce = force;
	}

	void update_jedi(const uint32_t& force, const uint32_t& currentTime) {
		jediTime = currentTime;
		jediForce = force;
	}

	void update_maybe(const uint32_t& force, const uint32_t& currentTime) {
		maybeTime = currentTime;
		maybeForce = force;
	}
public:

	// Default constructor
	MovieWatcher() : seen(), sithTime(), jediTime(), maybeTime(), sithForce(), jediForce(), maybeForce() {}

	uint32_t get_sith() {
		return sithTime;
	}

	uint32_t get_jedi() {
		return jediTime;
	}

	uint32_t get_forceDiff() {
		return sithForce - jediForce;
	}

	State get_state() {
		return seen;
	}

	// Checks if Jedi that just arrived on the planet
	// can be ambushed, and if it's interesting
	void check_ambush(ForceUsers& justDeployed, const uint32_t& currentTime) {
		// Initial state. Must see a sith to start.
		if (seen == State::Initial && !justDeployed.sOrJ) {
			seen = State::SeenOne;
			update_sith(justDeployed.force, currentTime);
		}
		// SeenOne state. Either grabs stronger sith, or first jedi
		else if (seen == State::SeenOne) {
			if (!justDeployed.sOrJ && justDeployed.force > sithForce) {
				update_sith(justDeployed.force, currentTime);
			}
			else {
				if (justDeployed.sOrJ && justDeployed.force <= sithForce) {
					seen = State::SeenBoth;
					update_jedi(justDeployed.force, currentTime);
				}
			}
		}
		// SeenBoth state. Grabs weaker jedi, and looks for maybe better sith
		else if (seen == State::SeenBoth) {
			if (justDeployed.sOrJ && justDeployed.force < jediForce) {
				update_jedi(justDeployed.force, currentTime);
			}
			else {
				if (!justDeployed.sOrJ && justDeployed.force > sithForce) {
					seen = State::MaybeBetter;
					update_maybe(justDeployed.force, currentTime);
				}
			}
		}
		// MaybeBetter state. Looks for jedi that gives bigger force difference than the old pair
		// or sith that is stronger than maybe sith.
		else if (seen == State::MaybeBetter) {
			if (justDeployed.sOrJ && justDeployed.force <= maybeForce &&
				(maybeForce - justDeployed.force) > get_forceDiff()) {
				seen = State::SeenBoth;
				update_jedi(justDeployed.force, currentTime);
				update_sith(maybeForce, maybeTime);
			}
			else {
				if (!justDeployed.sOrJ && justDeployed.force > maybeForce) {
					update_maybe(justDeployed.force, currentTime);
				}
			}
		}
	}
	
	// Runs attacks for movie-watcher mode
	void check_attack(const ForceUsers& justDeployed, const uint32_t& currentTime) {
		// Initial state. Must see a jedi to start. 
		if (seen == State::Initial && justDeployed.sOrJ) {
			seen = State::SeenOne;
			update_jedi(justDeployed.force, currentTime);
		}
		// SeenOne state. Must find either weaker jedi or stronger sith
		else if (seen == State::SeenOne) {
			if (justDeployed.sOrJ && justDeployed.force < jediForce) {
				update_jedi(justDeployed.force, currentTime);
			}
			else {
				if (!justDeployed.sOrJ && justDeployed.force >= jediForce) {
					seen = State::SeenBoth;
					update_sith(justDeployed.force, currentTime);
				}
			}
		}
		// SeenBoth state. Grabs stronger siths, and looks for other jedi
		else if (seen == State::SeenBoth) {
			if (!justDeployed.sOrJ && justDeployed.force > sithForce) {
				update_sith(justDeployed.force, currentTime);
			}
			else {
				if (justDeployed.sOrJ && justDeployed.force < jediForce) {
					seen = State::MaybeBetter;
					update_maybe(justDeployed.force, currentTime);
				}
			}
		}
		// MaybeBetter state. Looks for sith that gives bigger force difference than the old pair.
		else if (seen == State::MaybeBetter){
			if (!justDeployed.sOrJ && justDeployed.force >= maybeForce &&
				(justDeployed.force - maybeForce) > get_forceDiff()) {
				seen = State::SeenBoth;
				update_sith(justDeployed.force, currentTime);
				update_jedi(maybeForce, maybeTime);
			}
			else {
				if (justDeployed.sOrJ && justDeployed.force < maybeForce) {
					update_maybe(justDeployed.force, currentTime);
				}
			}
		}
	}

};

class Median { // For -m
private:
	priority_queue<uint32_t, vector<uint32_t>, greater<uint32_t>> higher;
	priority_queue<uint32_t, vector<uint32_t>, less<uint32_t>> lower;
public:

	// Constructor. Initializes PQs
	Median() : higher(), lower() {}

	// Adds element to either higher or lower vector, checks for rebalancing.
	void add(uint32_t x) {
		if (!higher.empty() && x >= higher.top()) {
			higher.push(x);
		}
		else {
			lower.push(x);
		}

		if (higher.size() - lower.size() == 2) {
			lower.push(higher.top());
			higher.pop();
		}
		else if (lower.size() - higher.size() == 2) {
			higher.push(lower.top());
			lower.pop();
		}
	}

	// Return median
	uint32_t get_median() {
		if (higher.size() == lower.size() && !empty()) {
			return ((higher.top() + lower.top()) / 2);
		}
		else if (higher.size() > lower.size() && !higher.empty()) {
			return higher.top();
		}
		else {
			return lower.top();
		}
	}

	bool empty() {
		if (higher.empty() && lower.empty()) {
			return true;
		}
		else {
			return false;
		}
	}

	bool higher_empty() {
		return higher.empty();
	}

	bool lower_empty() {
		return lower.empty();
	}
};

class Planet {
private:
	priority_queue<ForceUsers, vector<ForceUsers>, jediComp> jedi;
	priority_queue<ForceUsers, vector<ForceUsers>, sithComp> sith;
	uint32_t planet_ID;
	Median median; // for -m mode
	MovieWatcher attack;
	MovieWatcher ambush;
	command options;
	bool fightHappened;

public:
	friend class Deployment;

	Planet()
		: jedi(), sith(), planet_ID(-1), median(), attack(), ambush(), options(), fightHappened() {}

	Planet(uint32_t planetNum, command& optionsIn)
		: jedi(), sith(), planet_ID(planetNum), median(), attack(), ambush(), options(optionsIn), fightHappened() {}

	int roundToZero(int x) {
		if (x < 0) {
			return 0;
		}
		else {
			return x;
		}
	}

	// Checks if fights can occure. If so, fight!
	uint32_t fight() {
		uint32_t numBattles = 0;
		while (!sith.empty() && !jedi.empty() && (sith.top().force >= jedi.top().force)) {
			fightHappened = true;
			
			int sithTroops = sith.top().quantity;
			int oldSithTroops = sithTroops;
			int jediTroops = jedi.top().quantity;
			int oldJediTroops = jediTroops;

			sithTroops -= oldJediTroops;
			jediTroops -= oldSithTroops;

			// ***Command line options***
			if (options.isVerbose) {
				cout << "General " << sith.top().general_ID << "'s battalion attacked General "
					<< jedi.top().general_ID << "'s battalion on planet " << this->planet_ID << ". "
					<< (oldSithTroops - roundToZero(sithTroops)) + (oldJediTroops - roundToZero(jediTroops)) 
					<< " troops were lost.\n";
			}
			if (options.isGeneral) {
				generals[jedi.top().general_ID].numSurvived -= (oldJediTroops - roundToZero(jediTroops));
				generals[sith.top().general_ID].numSurvived -= (oldSithTroops - roundToZero(sithTroops));
			}
			if (options.isMedian) {
				median.add((oldSithTroops - roundToZero(sithTroops)) + (oldJediTroops - roundToZero(jediTroops)));
			}
			// ***Command line options***

			if (jediTroops > 0) {
				ForceUsers jediAlive = jedi.top();
				jediAlive.quantity = jediTroops;
				jedi.pop();
				jedi.push(jediAlive);
			}
			else {
				jedi.pop();
			}

			if (sithTroops > 0) {
				ForceUsers sithAlive = sith.top();
				sithAlive.quantity = sithTroops;
				sith.pop();
				sith.push(sithAlive);
			}
			else {
				sith.pop();
			}
			numBattles++;
		}
		return numBattles;
	}
};

class Deployment { // Stores all planets, runs the large algorithm. Reads inputs, etc.
private:
	vector<Planet> planets;
	uint32_t currentTime;
public:
	command options;
	
	friend class Planet;

	// Custom constructor
	Deployment(command &commands) 
		: planets(), currentTime(0), options(commands) {}


	void print_generals() {
		cout << "---General Evaluation---\n";
		
		for (uint32_t i = 0; i < generals.size(); i++) {
			cout << "General " << i << " deployed " << generals[i].numJedi
				 << " Jedi troops and " << generals[i].numSith << " Sith troops, and "
				 << generals[i].numSurvived << "/" << generals[i].numJedi + generals[i].numSith
				 << " troops survived.\n";
		}
	}

	// Read in first four lines of file and resize vectors
	// Returns true if DL, false if PR
	bool initialize(istream& input) { 
		string mode;
		uint32_t num_generals = 0;
		uint32_t num_planets;
		input.ignore(256, '\n'); // Ignore comment
		input.ignore(256, ':'); 
		input >> mode;
		
		input.ignore(256, ':');
		input >> num_generals;

		input.ignore(256, ':');
		input >> num_planets;

		generals.resize(num_generals);
		planets.resize(num_planets);
		
		if (mode == "PR") {
			return true;
		}
		else {
			return false;
		}
	}

	// Handels the rest of a PR file.
	// Basically fills the planets + generals vector with randos.
	void pr_input(istream& input, stringstream& ss) { // Needs to call P2random.h functions
		uint32_t random_seed = 0;
		uint32_t num_deployments = 0;
		uint32_t arrival_rate = 0;

		input.ignore(256, ':');
		input >> random_seed;

		input.ignore(256, ':');
		input >> num_deployments;

		input.ignore(256, ':');
		input >> arrival_rate;

		P2random::PR_init(ss, random_seed, static_cast<int>(generals.size()), static_cast<int>(planets.size()), num_deployments, arrival_rate);
		return;
	}

	// Runs everything
	void warfare() {
		bool readMode = initialize(cin);

		stringstream ss;
		// inputMode is the "PR" or "DL" from line 2
		if (readMode) {
			pr_input(cin, ss);
		} // if

		// Create a reference variable that is ALWAYS used for reading input.
		// If PR mode is on, refer to the stringstream.  Otherwise, refer to cin.
		// This is a place where the ternary operator must be used: an equivalent
		// if/else is impossible because reference variables must be initialized
		// when they are created.
		istream& inputStream = readMode ? ss : cin;

		uint32_t numBattles = 0;
		uint32_t newTime = 0;
		uint32_t newPlanetID;
		uint32_t forceID = 0;
		char sOrJ;
		char junk;
		ForceUsers justDeployed;
		// Make sure to read an entire deployment in the while statement
		while (inputStream >> newTime >> 
			   sOrJ >> junk >> junk >> junk >> junk >> 
			   justDeployed.general_ID >> junk >> 
			   newPlanetID >> junk >> 
			   justDeployed.force >> junk >> 
			   justDeployed.quantity) {
			bool sithOrJedi = true; // true == jedi, false == sith

			//ERROR CHECKING
			if (/*justDeployed.general_ID < 0 ||*/ justDeployed.general_ID > generals.size() - 1) {
				cerr << "Invalid general ID\n";
				exit(1);
			}
			else if (/*newPlanetID < 0 ||*/ newPlanetID >= planets.size()) {
				cerr << "Invalid planet ID\n";
				exit(1);
			}
			else if (justDeployed.force <= 0) {
				cerr << "Invalid force sensntivity level\n";
				exit(1);
			}
			else if (justDeployed.quantity <= 0) {
				cerr << "Invalid number of troops\n";
				exit(1);
			}
			if (newTime < currentTime) {
				cerr << "Invalid decreasing timestamp\n";
				exit(1);
			}
			else {
				//***MEDIAN MODE CHECK***
				if (options.isMedian && newTime != currentTime) {
					for (uint32_t i = 0; i < planets.size(); i++) {
						if ((!planets[i].median.higher_empty() || !planets[i].median.lower_empty()) && planets[i].fightHappened) {
							cout << "Median troops lost on planet " << i << " at time " << currentTime
								<< " is " << planets[i].median.get_median() << ".\n";
						}
					}
				}
				//***********************
				currentTime = newTime;
			}

			// Check if 'sith' or 'jedi'
			if (sOrJ == 'S') {
				sithOrJedi = false;
				justDeployed.sOrJ = false;
			}
			else {
				sithOrJedi = true;
				justDeployed.sOrJ = true;
			}

			//Check if general is ok, then make a new one if one doesn't exist and place in vector
			if (options.isGeneral && generals[justDeployed.general_ID].gen_ID == -1) {
				general newGeneral;
				newGeneral.gen_ID = justDeployed.general_ID;
				if (sithOrJedi) {
					newGeneral.numJedi = justDeployed.quantity;
				}
				else {
					newGeneral.numSith = justDeployed.quantity;
				}
				newGeneral.numSurvived = justDeployed.quantity;
				generals[justDeployed.general_ID] = newGeneral;
			}
			else if (options.isGeneral) { // If one exists, increase num jedi or sith
				if (sithOrJedi) {
					generals[justDeployed.general_ID].numJedi += justDeployed.quantity;
					generals[justDeployed.general_ID].numSurvived += justDeployed.quantity;
				}
				else {
					generals[justDeployed.general_ID].numSith += justDeployed.quantity;
					generals[justDeployed.general_ID].numSurvived += justDeployed.quantity;
				}
			}

			// Checks if planet is already in vector. If not, make one + put it in.
			if (static_cast<int>(planets[newPlanetID].planet_ID) == -1) { // If planet is default,
				Planet newPlanet(newPlanetID, options);
				planets[newPlanetID] = newPlanet;
			}

			// Set forceID to break ties w/ functor
			justDeployed.ID = forceID;
			forceID++;

			// Place into sith or jedi vector
			if (sithOrJedi) {
				planets[newPlanetID].jedi.push(justDeployed);
			}
			else {
				planets[newPlanetID].sith.push(justDeployed);
			}

			if (options.isWatcher) {
				planets[newPlanetID].attack.check_attack(justDeployed, currentTime);
				planets[newPlanetID].ambush.check_ambush(justDeployed, currentTime);
			}

			// Checks each planet to see if fights can happen
			for (uint32_t i = 0; i < planets.size(); i++) {
				numBattles += planets[i].fight();
			}
		} // while
		
		// End of day Median
		if (options.isMedian) {
			for (uint32_t i = 0; i < planets.size(); i++) {
				if ((!planets[i].median.higher_empty() || !planets[i].median.lower_empty()) && planets[i].fightHappened) {
					cout << "Median troops lost on planet " << i << " at time " << currentTime
						<< " is " << planets[i].median.get_median() << ".\n";
				}
			}
		}

		cout << "---End of Day---\n" << "Battles: " << numBattles << '\n';

		
		if (options.isGeneral) {
			print_generals();
		}
		
		if (options.isWatcher) {
			cout << "---Movie Watcher---\n";
			for (uint32_t i = 0; i < planets.size(); i++) {
				// Check to see if interesting battles actually exist
				if (planets[i].ambush.get_state() == State::SeenOne || planets[i].ambush.get_state() == State::Initial) {
					cout << "A movie watcher would enjoy an ambush on planet " << i << " with Sith at time "
						<< -1 << " and Jedi at time " << -1
						<< " with a force difference of " << 0 << ".\n";
				}
				else {
					cout << "A movie watcher would enjoy an ambush on planet " << i << " with Sith at time "
						<< planets[i].ambush.get_sith() << " and Jedi at time " << planets[i].ambush.get_jedi()
						<< " with a force difference of " << planets[i].ambush.get_forceDiff() << ".\n";
				}
				if (planets[i].attack.get_state() == State::SeenOne || planets[i].attack.get_state() == State::Initial) {
					cout << "A movie watcher would enjoy an attack on planet " << i << " with Jedi at time "
						<< -1 << " and Sith at time " << -1
						<< " with a force difference of " << 0 << ".\n";
				}
				else {
					cout << "A movie watcher would enjoy an attack on planet " << i << " with Jedi at time "
						<< planets[i].attack.get_jedi() << " and Sith at time " << planets[i].attack.get_sith()
						<< " with a force difference of " << planets[i].attack.get_forceDiff() << ".\n";
				}
			}
		}
		return;
	}
	/*
	void gen_random() {
		bool readMode = initialize(cin);

		stringstream ss;
		// inputMode is the "PR" or "DL" from line 2
		if (readMode) {
			pr_input(cin, ss);
		} // if

		// Create a reference variable that is ALWAYS used for reading input.
		// If PR mode is on, refer to the stringstream.  Otherwise, refer to cin.
		// This is a place where the ternary operator must be used: an equivalent
		// if/else is impossible because reference variables must be initialized
		// when they are created.
		istream& inputStream = readMode ? ss : cin;

		uint32_t numBattles = 0;
		uint32_t newTime = 0;
		uint32_t newPlanetID;
		uint32_t forceID = 0;
		string sOrJ;
		char junk;
		ForceUsers justDeployed;
		cout << ss.rdbuf();
		// Make sure to read an entire deployment in the while statement
		while (ss >> newTime >>
			sOrJ >> junk >> junk >> junk >> junk >>
			justDeployed.general_ID >> junk >>
			newPlanetID >> junk >>
			justDeployed.force >> junk >>
			justDeployed.quantity) {

			cout << newTime << " G" << justDeployed.general_ID << " P" << newPlanetID
				<< " F" << justDeployed.force << " #" << justDeployed.quantity << '\n';
		}
	}
	*/
};
