#include <stdlib.h>
#include "MonoJet/CutFlow.h"
#include <iostream>

using namespace std;
int main(int argc, char** argv){

    if((argc > 1 && string(argv[1]).find("help")!= string::npos) ||
            argc < 4){
        cout<<argv[0]<<" filelist/file outname channel is_mc"<<endl;
        return 0;
    }
    bool is_mc = true;
    double lumi = 5000.0;
    string inputFileName(argv[1]);
    string outputFileName(argv[2]);
    int channel = atoi(argv[3]);
    if (argc > 4){
        is_mc = (bool) atoi(argv[4]);
    }
    auto* cutflow = new CutFlow(inputFileName.c_str(), is_mc, outputFileName.c_str(), lumi);
    cutflow ->getCutFlows(channel);
    delete cutflow;
    return 0;
}
