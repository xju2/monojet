#ifndef __MONOJET_Truth_JetMET_H
#define __MONOJET_Truth_JetMET_H

#include <vector>
#include <TLorentzVector.h>
using namespace std;
const float ZMASS = 91187.6;
float m4l;
float Hpt_;
float Hphi_;
float mZ1;
float mZ2;
float Z1_lepplus_pt;
float Z1_lepminus_pt;
float Z2_lepplus_pt;
float Z2_lepminus_pt;

class Candidate{
    private: 
        int index;
        TLorentzVector P;
        float charge;
    public:
    Candidate(int index,TLorentzVector pp){
        this->index = index;
        this->P = pp;
    }
    void setCharge(float charge){
        this->charge = charge;
    }

    float getCharge(){
        return this->charge;
    }

    TLorentzVector getFourVector(){
    	return this->P;
    }
};

bool close2Z(vector<Candidate*>* lep_4vec, double& m12, int& idL1, int& idL2){
    double tmpZ1 = -10.0;
    for(int i=0; i < (int)lep_4vec->size(); i++){
        for(int j=i+1; j<(int)lep_4vec->size(); ++j){
            float ch1 = lep_4vec->at(i)->getCharge();
            float ch2 = lep_4vec->at(j)->getCharge();
            if(ch1 * ch2 == 1)continue;
            double dimass = (lep_4vec->at(i)->getFourVector()+
                    lep_4vec->at(j)->getFourVector()).M();
            if(fabs(dimass - ZMASS) < fabs(tmpZ1 - ZMASS)){
                    idL1 = i; 
                    idL2 = j;
                    tmpZ1 = dimass;
            }
        }
    }
    if(tmpZ1 > 0) {
        m12 = tmpZ1;
        return true;
    }else{ 
        return false;
    }

}
bool test_4mu4e(vector<Candidate*>* lep_4vec ){
    double tmpZ1 = 0.0;
    int idL1 = -1, idL2 = -1, idL3 = -1, idL4 = -1;
    if(close2Z(lep_4vec, tmpZ1, idL1, idL2)){
        vector<Candidate*>* newlep = new vector<Candidate*>();
        for(int i=0; i < (int) lep_4vec->size(); i++){
            if(i==idL1 || i==idL2) continue;
            newlep->push_back(lep_4vec->at(i));
        }
        double tmpZ2 = 0.0;
        if(close2Z(newlep, tmpZ2, idL3, idL4)){
            mZ1 = (float)tmpZ1; mZ2 = (float)tmpZ2;
            TLorentzVector Higgs_tlv(lep_4vec->at(idL1)->getFourVector());
            Higgs_tlv += lep_4vec->at(idL2)->getFourVector();
            Higgs_tlv += newlep->at(idL3)->getFourVector();
            Higgs_tlv += newlep->at(idL4)->getFourVector();
            m4l = (float)Higgs_tlv.M();
            Hpt_ = (float) Higgs_tlv.Pt();
            Hphi_ = (float) Higgs_tlv.Phi();

            if(lep_4vec->at(idL1)->getCharge() > 0){
                Z1_lepplus_pt  = lep_4vec->at(idL1)->getFourVector().Pt();
                Z1_lepminus_pt = lep_4vec->at(idL2)->getFourVector().Pt();
            }else{
                Z1_lepplus_pt  = lep_4vec->at(idL2)->getFourVector().Pt();
                Z1_lepminus_pt = lep_4vec->at(idL1)->getFourVector().Pt();
            }
            if(newlep ->at(idL3)->getCharge() > 0){
                Z2_lepplus_pt  = newlep->at(idL3)->getFourVector().Pt();
                Z2_lepminus_pt = newlep->at(idL4)->getFourVector().Pt();
            }else{
                Z2_lepplus_pt  = newlep->at(idL4)->getFourVector().Pt();
                Z2_lepminus_pt = newlep->at(idL3)->getFourVector().Pt();
            }
            return true;
        }
    }
    return false;
}
bool test_2e2mu(vector<Candidate*>* ele_4vec, vector<Candidate*>* muon_4vec, int& type){
    double tmpZ1 = 0, tmpZ2 = 0;
    int idL1 = -1, idL2 = -1, idL3 = -1, idL4 = -1;
    if(close2Z(ele_4vec, tmpZ1, idL1, idL2)){
        if(close2Z(muon_4vec, tmpZ2, idL3, idL4)){
            TLorentzVector Higgs_tlv(ele_4vec->at(idL1)->getFourVector());
            Higgs_tlv += ele_4vec->at(idL2)->getFourVector();
            Higgs_tlv += muon_4vec->at(idL3)->getFourVector();
            Higgs_tlv += muon_4vec->at(idL4)->getFourVector();
            m4l =  (float) Higgs_tlv.M();
            Hpt_ = (float) Higgs_tlv.Pt();
            Hphi_ = (float) Higgs_tlv.Phi();

            if(fabs(tmpZ1 - ZMASS) < fabs(tmpZ2 - ZMASS)){
                type = 3; //2e2mu
                mZ1 = tmpZ1; mZ2 = tmpZ2;
                if(ele_4vec->at(idL1)->getCharge() > 0){
                    Z1_lepplus_pt   = ele_4vec->at(idL1)->getFourVector().Pt();
                    Z1_lepminus_pt  = ele_4vec->at(idL2)->getFourVector().Pt();
                }else{
                    Z1_lepplus_pt   = ele_4vec->at(idL2)->getFourVector().Pt();
                    Z1_lepminus_pt  = ele_4vec->at(idL1)->getFourVector().Pt();
                }
                if(muon_4vec->at(idL3)->getCharge() > 0){
                    Z2_lepplus_pt   = muon_4vec->at(idL3)->getFourVector().Pt();
                    Z2_lepminus_pt  = muon_4vec->at(idL4)->getFourVector().Pt();
                }else{
                    Z2_lepplus_pt   = muon_4vec->at(idL4)->getFourVector().Pt();
                    Z2_lepminus_pt  = muon_4vec->at(idL3)->getFourVector().Pt();
                }
            }else{
                type = 4; //2mu2e
                mZ1 = tmpZ2; mZ2 = tmpZ1;
                if(ele_4vec->at(idL1)->getCharge() > 0){
                    Z2_lepplus_pt   = ele_4vec->at(idL1)->getFourVector().Pt();
                    Z2_lepminus_pt  = ele_4vec->at(idL2)->getFourVector().Pt();
                }else{
                    Z2_lepplus_pt   = ele_4vec->at(idL2)->getFourVector().Pt();
                    Z2_lepminus_pt  = ele_4vec->at(idL1)->getFourVector().Pt();
                }
                if(muon_4vec->at(idL3)->getCharge() > 0){
                    Z1_lepplus_pt   = muon_4vec->at(idL3)->getFourVector().Pt();
                    Z1_lepminus_pt  = muon_4vec->at(idL4)->getFourVector().Pt();
                }else{
                    Z1_lepplus_pt   = muon_4vec->at(idL4)->getFourVector().Pt();
                    Z1_lepminus_pt  = muon_4vec->at(idL3)->getFourVector().Pt();
                }
            }
            return true;
        }
    }
    return false;
}
#endif
