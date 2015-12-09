#include <stdlib.h>
#include <string>
#include <vector>

#include "MonoJet/SysHandle.h"
using namespace std;
int main(int argc, char* argv[]){
    string base_path = "/afs/cern.ch/work/x/xju/monoJet/process/get_minitree/testarea/";
    string norm_dir = base_path+"reduced_ntup_norm.root";
    string sys_dir = base_path+"reduced_ntup_sys.root";

    SysHandle* sys_handle = new SysHandle("Wenu");
    // sys_handle->SetDebug();
    sys_handle->SetCuts(250e3, 250e3, 4);
    sys_handle->GetYields(norm_dir, sys_dir);
    // sys_handle->PrintYields();
    vector<string> samples;
    samples.push_back("Znunu");
    samples.push_back("Wenu");
    samples.push_back("Wmunu");
    samples.push_back("Wtaunu");
    samples.push_back("Zee");
    samples.push_back("Zmumu");
    samples.push_back("Ztautau");
    samples.push_back("D5_signal");
    for(auto& sample : samples){
        sys_handle->SetSampleName(sample);
        string output_name = "input_"+sample+".root";
        sys_handle->GetHist(output_name.c_str());
    }


    delete sys_handle;

    return 0;
}
