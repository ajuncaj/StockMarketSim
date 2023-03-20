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

using namespace std;

struct ForceUsers {
	uint32_t general_ID;
	uint32_t force;
	uint32_t quantity;
};
struct sithComp {
	bool operator()(const ForceUsers& sith1, const ForceUsers& sith2) {
		return  sith1.force < sith2.force;
	}
};
struct jediComp {
	bool operator()(const ForceUsers& jedi1, const ForceUsers& jedi2) {
		return  jedi1.force > jedi2.force;
	}
};


class Median {
	// who knows whats going on down here lol
	// Make 2 PQs to store high values + low values
	// https://stackoverflow.com/questions/28846065/calculate-median-using-priority-queues
	// https://www.youtube.com/watch?v=_T63Cajeyhw

	priority_queue<int> higher;
	priority_queue<int> lower;
};

class Planet {
private:
	priority_queue<ForceUsers, vector<ForceUsers>, jediComp> jedi;
	priority_queue<ForceUsers, vector<ForceUsers>, sithComp> sith;
	uint32_t planet_ID;
	Median median; // for -m mode

public:
	friend class Deployment;

	Planet()
		: jedi(), sith(), planet_ID(-1), median() {}

	Planet(uint32_t planetNum)
		: jedi(), sith(), planet_ID(planetNum), median() {}

	int roundToZero(int x) {
		if (x < 0) {
			return 0;
		}
		else {
			return x;
		}
	}

	// General <SITH_GENERAL_NUM>’s battalion attacked General <JEDI_GENERAL_NUM>’s
	// battalion on planet <PLANET_NUM>. <NUM_TROOPS_LOST> troops were lost.
	// Checks if fights can occure. If so, fight!
	void fight() {
		while (!sith.empty() && !jedi.empty() && sith.top().force >= jedi.top().force) {
			int sithTroops = sith.top().quantity;
			int oldSithTroops = sithTroops;
			int jediTroops = jedi.top().quantity;
			int oldJediTroops = jediTroops;

			sithTroops -= oldJediTroops;
			jediTroops -= oldSithTroops;

			cout << "General " << sith.top().general_ID << "'s battalion attacked General "
				<< jedi.top().general_ID << "'s battalion on planet " << this->planet_ID << ". "
				<< (oldSithTroops - roundToZero(sithTroops)) + (oldJediTroops - roundToZero(jediTroops)) << " troops were lost\n";

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
		}
	}
};

