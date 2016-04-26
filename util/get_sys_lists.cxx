// dump supported systematics in SUSYTools
#include <stdlib.h>
#include <iostream>
#include "SUSYTools/SUSYObjDef_xAOD.h"
#include "MonoJet/Helper.h"

using namespace std;
int main( int argc, char* argv[]){
    ST::SUSYObjDef_xAOD objTool("SUSYObjDef_xAOD");
    std::cout << " ABOUT TO INITIALIZE SUSYTOOLS " << std::endl;
    objTool.msg().setLevel(MSG::ERROR);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Configure the SUSYObjDef instance
    bool isData = false;
    bool isAtlfast = false;
    ST::ISUSYObjDef_xAODTool::DataSource data_source = isData ? ST::ISUSYObjDef_xAODTool::Data : (isAtlfast ? ST::ISUSYObjDef_xAODTool::AtlfastII : ST::ISUSYObjDef_xAODTool::FullSim);
    objTool.setProperty("DataSource", data_source);
    // use the first set of JES NPs
    objTool.setProperty("JESNuisanceParameterSet", 1);

    if( objTool.initialize() != StatusCode::SUCCESS ){
        exit(-1);
    }else{
        cout << "SUSYObjDef_xAOD initialized... " << endl;
    }
    std::cout << " INITIALIZED SUSYTOOLS " << std::endl;
    cout << "==================================================" << endl;
    cout << "==== supported systematics list in SUSYTools =====" << endl;
    // dump the supported systematics
    vector<ST::SystInfo> sysInfoList = objTool.getSystInfoList();
    for (const auto& sysInfo : sysInfoList){
        const CP::SystematicSet& sys = sysInfo.systset;
        // only list Jet, Electron, Muon, MET
        if (MonoJet::Helper::IsSysNeeded(sysInfo))
        {
            cout << sys.name() << " " << sysInfo.affectsKinematics
                << " " << sysInfo.affectsWeights
                << " " << sysInfo.affectsType << endl;
        }
    }
    cout << "==================================================" << endl;
}
