// Project identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882

#include <vector>
#include <iostream>
#include <algorithm> // std::sort
#include <getopt.h>
#include <string>    // needed for VS
#include <cstring>
#include <cassert>
#include "deployment.h"

using namespace std;



void get_options(int argc, char** argv);
command options;

int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false); // comment out if you use valgrind
    get_options(argc, argv);
    
    cout << "Deploying troops...\n";
    Deployment war(options);
    war.warfare();
   // war.gen_random();

    return 0;
}


void get_options(int argc, char** argv) {
    int option_index = 0, option = 0;
    // Don't display getopt error messages about options
    opterr = false;

    // use getopt to find command line options
    struct option longOpts[] = { { "verbose",      no_argument, nullptr, 'v' },
                                 { "median",       no_argument, nullptr, 'm' },
                                 { "general-eval", no_argument, nullptr, 'g' },
                                 { "watcher",      no_argument, nullptr, 'w' },
                                 { nullptr, 0, nullptr, '\0' } };


    while ((option = getopt_long(argc, argv, "vmgw", longOpts, &option_index)) != -1) {
        switch (option) {

        case 'v': // work on throughout
            options.isVerbose = true;
            break;

        case 'm':
            options.isMedian = true;
            break;

        case 'g':
            options.isGeneral = true;
            break;

        case 'w':
            options.isWatcher = true;
            break;
        }
    }
}

    /*
    ----NOTES----
    -2 queues, sith + jedi
        - Jedi lowest force == highest priority, sith highest force == highest priority
        -One deployment funciton, different functors
        - DO NOT use strings. Not efficient
    - Another queue, planet
        - Put 2 queues in each planet queue, put planets in a vector or something?
        class Planet {
            public:
            PQ Jedi
            PQ Sith
            vector median
        }
    -for general mode, keep generals in vector
        - can play both sides, be careful for that. Need to track jedi + sith deployed, how many died OR lived
    -need to make functors to use as comparators for sith + jedi
    */