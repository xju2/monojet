#include <stdlib.h>
#include <string>
#include <iostream>

#include "MyXAODTools/EventCounter.h"

using namespace std;
int main(int argc, char** argv){
    if (argc < 2) {
        cout << argv[0] << " input_list output_list" <<endl;
        return 0;
    }
    string output_name("weights.list");
    if(argc > 2) {
        output_name = string(argv[2]);
    }
    string inputFileName(argv[1]);
    EventCounter* myalg = new EventCounter();
    myalg->GetTotalEventsDic(inputFileName.c_str(), "associate");
    myalg->printTotalEvents();
    myalg->SaveTotalEvents(output_name.c_str());
    delete myalg;
    return 1;
}
