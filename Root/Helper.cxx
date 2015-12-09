#include <stdlib.h>
#include <TVector2.h>
#include <TMath.h>

#include "MonoJet/Helper.h"

using namespace MonoJet;
string Helper::parse_sys_name(const char* name){
   string line(name);
   size_t delim_pos = line.find("__");
   if (delim_pos != string::npos) {
       string sys_name = line.substr(0, delim_pos);
       string var_name = line.substr(delim_pos+3, line.size());
       return sys_name+"-"+var_name;
   } else {
       if(line.find("MET_SoftTrk") != string::npos) {
            if(line.find("Para") != string::npos ||
               line.find("Down") != string::npos){
                return line+"-down";
            } else {
                return line+"-up";
            }
       } else {
           return "none";
       }
   }
}

double Helper::get_mt(xAOD::IParticle::FourMom_t p4, 
        double MET_etx, double MET_ety)
{
    TVector2 met_vec(MET_etx, MET_ety);
    return sqrt(2*met_vec.Mod()*p4.Pt()*(1-TMath::Cos(p4.Vect().XYvector().DeltaPhi(met_vec))));
}


bool Helper::IsSysNeeded(const ST::SystInfo& sysInfo){
    return (sysInfo.affectsType == ST::Jet ||
            sysInfo.affectsType == ST::Electron ||
            sysInfo.affectsType == ST::Egamma ||
            sysInfo.affectsType == ST::Muon ||
            sysInfo.affectsType == ST::MET_TST || 
            sysInfo.affectsType == ST::MET_Track
            );

}
