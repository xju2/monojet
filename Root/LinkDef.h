#ifndef MONOJET_LINKDEF_H
#define MONOJET_LINKDEF_H

#include <MonoJet/MyCuts.h>
#include <MonoJet/OutputTreeMan.h>
#include <MonoJet/Helper.h>
#include <MonoJet/SmearedInfo.h>

#ifdef __MAKECINT__
#pragma link C++ class SmearedInfo+;
#pragma link C++ class vector<SmearedInfo>+;
#endif
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ class MyCuts+;
#pragma link C++ class OutputTreeMan+;

#pragma link C++ namespace MonoJet::Helper;
#pragma link C++ function MonoJet::Helper::parse_sys_name+;
#pragma link C++ function MonoJet::Helper::get_mt+;

#endif

#endif
