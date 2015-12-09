#ifndef _MONOJET_HELPER_H_
#define _MONOJET_HELPER_H_

#include <string>

#include <TH1F.h>
#include "xAODBase/IParticle.h"
#include "MonoJet/OutputTreeMan.h"
#include "SUSYTools/SUSYObjDef_xAOD.h"

using namespace std;
namespace MonoJet{
    namespace Helper{
        string parse_sys_name(const char* name);
        double get_mt(xAOD::IParticle::FourMom_t p4, 
                double MET_etx, double MET_ety);
        bool IsSysNeeded(const ST::SystInfo& sysinfo);
    }
}
#endif
