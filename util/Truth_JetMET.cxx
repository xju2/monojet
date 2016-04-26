// System include(s):
#include <memory>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TChain.h>
#include <TH1F.h>
#include <TLorentzVector.h>
#include <TVector2.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"

#include "xAODBase/IParticleHelpers.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "CPAnalysisExamples/errorcheck.h"
#include "SUSYTools/SUSYObjDef_xAOD.h"

// Other includes
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicCode.h"
#include "boost/unordered_map.hpp"

#include "MonoJet/Truth_JetMET.h"
#include "MyXAODTools/EventCounter.h"

using namespace std;

bool descend_on_pt(xAOD::IParticle* p1, xAOD::IParticle* p2){
    return p1->pt() > p2->pt();
}

int GAFinalHadronFlavourLabel (const xAOD::Jet* jet) {
    const std::string labelB = "GhostBHadronsFinal";
    const std::string labelC = "GhostCHadronsFinal";
    const std::string labelTau = "GhostTausFinal";
    std::vector<const xAOD::IParticle*> ghostB;
    if (jet->getAssociatedObjects<xAOD::IParticle>(labelB, ghostB) && ghostB.size() > 0) return 5;
    std::vector<const xAOD::IParticle*> ghostC;
    if (jet->getAssociatedObjects<xAOD::IParticle>(labelC, ghostC) && ghostC.size() > 0) return 4;
    std::vector<const xAOD::IParticle*> ghostTau;
    if (jet->getAssociatedObjects<xAOD::IParticle>(labelTau, ghostTau) && ghostTau.size() > 0) return 15;
    return 0;
}

int main( int argc, char* argv[] ) {
    
    if(argc > 1 && string(argv[1]) == "help"){
        cout << argv[0] << " toberun.txt num_evts" << endl;
        return 0;
    }
	StatusCode::enableFailure();
	CP::SystematicCode::enableFailure();
	CP::CorrectionCode::enableFailure();

	// The application's name:
	const char* APP_NAME = argv[ 0 ];

	// Initialise the application:
	CHECK( xAOD::Init( APP_NAME ) );
    string inputFileName = "toberun.txt";
    if(argc > 1){
        inputFileName = string(argv[1]);
    }

	TChain* fc = EventCounter::getChain(inputFileName.c_str(), "CollectionTree"); 

	// Create a TEvent object:
	xAOD::TEvent event( xAOD::TEvent::kClassAccess );
	CHECK( event.readFrom( fc ) );
	Info( APP_NAME, "Number of events in the chain: %i",
			static_cast< int >( event.getEntries() ) );


	// Decide how many events to run over:
	Long64_t entries = event.getEntries();
	if( argc > 2 ) {
		const Long64_t e = atoll( argv[ 2 ] );
		if( e>0 && e < entries ) {
			entries = e;
		}
	}
    Info(APP_NAME, "Will process: %d", (int)entries);
    

	int RunNumber;
	int EventNumber;
	int mc_channel_number;
	double MCWeight;
    double mc_weight_up;
    double mc_weight_down;
    vector<double>* mc_weights = new vector<double>();

	std::vector<double> jet_pt;
	std::vector<double> jet_eta;
	std::vector<double> jet_phi;
	std::vector<double> jet_E;
	std::vector<double> jet_dphi_MET;
	std::vector<double> jet_mv2c20;
	std::vector<int> jet_flavor;


	double MET_etx;
	double MET_ety;
    double MET_et;
    double MET_phi;

    int type;

	TFile *fOutputFile = new TFile( "reduced_ntuple.root", "recreate" );
	TTree MyTree( "physics", "physics" );

	MyTree.Branch("RunNumber", &RunNumber, "RunNumber/I");
	MyTree.Branch("EventNumber", &EventNumber, "EventNumber/I");
	MyTree.Branch("mc_channel_number", &mc_channel_number, "mc_channel_number/I");

	MyTree.Branch("jet_pt", &jet_pt);
	MyTree.Branch("jet_eta", &jet_eta);
	MyTree.Branch("jet_phi", &jet_phi);
	MyTree.Branch("jet_E", &jet_E);
	MyTree.Branch("jet_dphi_MET", &jet_dphi_MET);
	MyTree.Branch("jet_MV2c20", &jet_mv2c20);
	MyTree.Branch("jet_flavor", &jet_flavor);

	MyTree.Branch("MET_etx", &MET_etx, "MET_etx/D");
	MyTree.Branch("MET_ety", &MET_ety, "MET_ety/D");
	MyTree.Branch("MET_et", &MET_et, "MET_et/D");
	MyTree.Branch("MET_phi", &MET_phi, "MET_phi/D");

    MyTree.Branch("m4l", &m4l, "m4l/F");
    MyTree.Branch("Hpt", &Hpt_, "Hpt/F");
    MyTree.Branch("Hphi", &Hphi_, "Hphi/F");

    MyTree.Branch("mZ1", &mZ1, "mZ1/F");
    MyTree.Branch("mZ2", &mZ2, "mZ2/F");
    MyTree.Branch("Z1_lepplus_pt", &Z1_lepplus_pt, "Z1_lepplus_pt/F");
    MyTree.Branch("Z1_lepminus_pt", &Z1_lepminus_pt, "Z1_lepminus_pt/F");
    MyTree.Branch("Z2_lepplus_pt", &Z2_lepplus_pt, "Z2_lepplus_pt/F");
    MyTree.Branch("Z2_lepminus_pt", &Z2_lepminus_pt, "Z2_lepminus_pt/F");

    MyTree.Branch("type", &type, "type/I");

	MyTree.Branch("MCWeight", &MCWeight, "MCWeight/D");
	MyTree.Branch("MCWeightUp", &mc_weight_up, "MCWeightUp/D");
	MyTree.Branch("MCWeightDown", &mc_weight_down, "MCWeightDown/D");
	MyTree.Branch("MCWeights", &mc_weights);

    // save truth information
    double truth_h_mass;
    double truth_z1_mass;
    double truth_z2_mass;
    double truth_h_pt;
    double truth_z1_pt;
    double truth_z2_pt;

    double truth_j1_pt;
    double truth_j2_pt;
    double truth_j3_pt;
    double truth_j4_pt;
    double truth_j1_phi;
    double truth_j2_phi;
    double truth_j3_phi;
    double truth_j4_phi;

    MyTree.Branch("truth_h_mass", & truth_h_mass, "truth_h_mass/D");
    MyTree.Branch("truth_z1_mass", & truth_z1_mass, "truth_z1_mass/D");
    MyTree.Branch("truth_z2_mass", & truth_z2_mass, "truth_z2_mass/D");

    MyTree.Branch("truth_h_pt", & truth_h_pt, "truth_h_pt/D");
    MyTree.Branch("truth_z1_pt", & truth_z1_pt, "truth_z1_pt/D");
    MyTree.Branch("truth_z2_pt", & truth_z2_pt, "truth_z2_pt/D");

    MyTree.Branch("truth_j1_pt", & truth_j1_pt, "truth_j1_pt/D");
    MyTree.Branch("truth_j2_pt", & truth_j2_pt, "truth_j2_pt/D");
    MyTree.Branch("truth_j3_pt", & truth_j3_pt, "truth_j3_pt/D");
    MyTree.Branch("truth_j4_pt", & truth_j4_pt, "truth_j4_pt/D");

    MyTree.Branch("truth_j1_phi", & truth_j1_phi, "truth_j1_phi/D");
    MyTree.Branch("truth_j2_phi", & truth_j2_phi, "truth_j2_phi/D");
    MyTree.Branch("truth_j3_phi", & truth_j3_phi, "truth_j3_phi/D");
    MyTree.Branch("truth_j4_phi", & truth_j4_phi, "truth_j4_phi/D");

	for( Long64_t entry = 0; entry < entries; ++entry ) {

		jet_pt.clear();
		jet_eta.clear();
		jet_phi.clear();
		jet_E.clear();
        jet_dphi_MET.clear();
        jet_mv2c20.clear();
        jet_flavor.clear();

        mc_weights->clear();

		// Tell the object which entry to look at:
		event.getEntry( entry );

		const xAOD::EventInfo* ei = 0;
		CHECK( event.retrieve( ei, "EventInfo" ) );

		RunNumber = ei->runNumber();
		EventNumber = ei->eventNumber();
		mc_channel_number = ei-> mcChannelNumber();

		MCWeight = ei->mcEventWeight();

        const vector<float>& weights = ei->mcEventWeights();
        if(weights.size() > 2) {
            mc_weight_up = weights.at(1);
            mc_weight_down = weights.at(2);
        }
        for(int i = 0; i < (int)weights.size(); i ++){
            mc_weights->push_back(weights.at(i));
        }


		const xAOD::MissingETContainer* met = 0;
		CHECK( event.retrieve( met, "MET_Truth" ) );
		xAOD::MissingETContainer::const_iterator met_it = met->find("NonInt");
		if (met_it == met->end())
		{
			Error( APP_NAME, "No NonInt inside MET container" );
		}

		MET_etx = (*met_it)->mpx();
		MET_ety = (*met_it)->mpy();
        MET_et = sqrt(MET_etx*MET_etx + MET_ety*MET_ety);
        MET_phi = (*met_it)->phi();

		// Get the Jets from the event:
		const xAOD::JetContainer* jets_origin = 0;
        CHECK( event.retrieve( jets_origin, "AntiKt4TruthJets" ) );
        std::pair<xAOD::JetContainer*, xAOD::ShallowAuxContainer*> jet_shallowcopy = xAOD::shallowCopyContainer(*jets_origin);
        xAOD::JetContainer* jets = jet_shallowcopy.first;
        sort(jets->begin(), jets->end(), descend_on_pt);

		xAOD::JetContainer::const_iterator jet_itr = jets->begin();
		xAOD::JetContainer::const_iterator jet_end = jets->end();
    
        string m_taggerName = "MV2c20";
		for( ; jet_itr != jet_end; ++jet_itr ) {
            double dphi = fabs(TVector2::Phi_mpi_pi((*met_it)->phi() - (*jet_itr)->phi()));
            jet_pt.push_back( (*jet_itr)->pt() );
            jet_eta.push_back( (*jet_itr)->eta() );
            jet_phi.push_back( (*jet_itr)->phi() );
            jet_E.push_back( (*jet_itr)->e() );
            jet_dphi_MET.push_back( dphi );
            const xAOD::BTagging* btag = (*jet_itr)->btagging();
            double weight_mv2 (-10.0);
            if((!btag) || (!btag->MVx_discriminant(m_taggerName, weight_mv2))) {
               ; 
            }
            jet_mv2c20.push_back(weight_mv2);
            jet_flavor.push_back(GAFinalHadronFlavourLabel((*jet_itr)));
        }

        // Get truth particles
       const xAOD::TruthParticleContainer* particles(0);
       CHECK(event.retrieve(particles, "TruthParticles"));
       auto par_itr = particles->begin();
       auto par_end = particles->end();
       truth_z1_mass = -1;
       truth_z2_mass = -1;
       truth_z1_pt = -1;
       truth_z2_pt = -1;

       truth_j1_pt = -1;
       truth_j2_pt = -1;
       truth_j3_pt = -1;
       truth_j4_pt = -1;

       for(; par_itr != par_end; par_itr++){
            const xAOD::TruthParticle* p = (*par_itr);
            int pdgId = p->pdgId();
            if(pdgId == 25) {
                truth_h_mass = p->m();
                truth_h_pt = sqrt(p->px()*p->px()+p->py()*p->py());
                if(p->hasDecayVtx()) { // find decay vertex of Higgs
                    const xAOD::TruthVertex* hzz_vtx = p->decayVtx();
                    int h_daugthers = hzz_vtx->nOutgoingParticles();
                    if(h_daugthers != 2) {
                        /***
                        cerr << "Higgs decays to " << h_daugthers << " daughters!" << endl;
                        const xAOD::TruthParticle* tmp = hzz_vtx->outgoingParticle(0);
                        cerr << "\t pdgID: " << tmp->pdgId() << endl; 
                        cerr << "\t mass: " << tmp->m() << endl;
                        **/
                        continue;
                    }
                    for(int i_h_daughter = 0; i_h_daughter < h_daugthers; i_h_daughter++){
                        const xAOD::TruthParticle* z = hzz_vtx->outgoingParticle(i_h_daughter);
                        bool is_leading_z = true;
                        double z_pt = sqrt(z->px()*z->px()+z->py()*z->py());
                        if(z->pdgId() != 23) continue;
                        if(truth_z1_mass < 0) {
                            truth_z1_mass = z->m();
                            truth_z1_pt = z_pt;
                        } else {
                            is_leading_z = false;
                            truth_z2_mass = z->m();
                            truth_z2_pt = z_pt;
                        }
                        const xAOD::TruthVertex* zqq_vtx = z->decayVtx();
                        int z_daughters = zqq_vtx->nOutgoingParticles();
                        if(z_daughters != 2) {
                            cerr << "Z decays to " << z_daughters << " daughters!" << endl;
                            continue;
                        }
                        for(int i_z_daughter = 0; i_z_daughter < z_daughters; i_z_daughter++){
                            const xAOD::TruthParticle* quark = zqq_vtx->outgoingParticle(i_z_daughter);
                            TVector2 quark_xy(quark->px(), quark->py());
                            double quark_pt = quark_xy.Mod();
                            double quark_phi = TVector2::Phi_mpi_pi(quark_xy.Phi());
                            if(is_leading_z){
                                if (truth_j1_pt < 0) {
                                    truth_j1_pt = quark_pt;
                                    truth_j1_phi = quark_phi;
                                }else{ 
                                    truth_j2_pt = quark_pt;
                                    truth_j2_phi = quark_phi;
                                }
                            }else{
                                if (truth_j3_pt < 0) {
                                    truth_j3_pt = quark_pt;
                                    truth_j3_phi = quark_phi;
                                } else {
                                    truth_j4_pt = quark_pt;
                                    truth_j4_phi = quark_phi;
                                }
                            }
                        }
                    }
                }
                break;
            }
       }
       if( fabs(truth_z1_mass-ZMASS) > fabs(truth_z2_mass-ZMASS)){
          swap(truth_z1_mass, truth_z2_mass);
          swap(truth_j1_pt, truth_j3_pt);
          swap(truth_j1_phi, truth_j3_phi);

          swap(truth_j2_pt, truth_j4_pt);
          swap(truth_j2_phi, truth_j4_phi);
       }
       if(truth_j1_pt < truth_j2_pt){ 
           swap(truth_j1_pt, truth_j2_pt);
           swap(truth_j1_phi, truth_j2_phi);
       }
       if(truth_j3_pt < truth_j4_pt){  
           swap(truth_j3_pt, truth_j4_pt);
           swap(truth_j3_phi, truth_j4_phi);
       }

       ////get electrons
       const xAOD::TruthParticleContainer* electrons(0);
       CHECK( event.retrieve(electrons,"TruthElectrons") );
       xAOD::TruthParticleContainer::const_iterator ele_itr = electrons->begin();
       xAOD::TruthParticleContainer::const_iterator ele_end = electrons->end();
       vector<Candidate*>* ele_4vec = new vector<Candidate*>();
       int ele_index = 0;
       // cout << "total electrons: " << electrons->size() << endl;
       for(; ele_itr != ele_end; ele_itr ++){
           if( (*ele_itr)->status() == 1 && (*ele_itr)->absPdgId() == 11)
           {
               TLorentzVector ele;
               ele.SetPxPyPzE((*ele_itr)->px(),(*ele_itr)->py(),
                       (*ele_itr)->pz(), (*ele_itr)->e());
               if(ele.Pt() > 7e3 && ele.Eta() < 2.47){
                   float charge = (*ele_itr)->pdgId()/11.;
                   Candidate* can = new Candidate(ele_index, ele);
                   can->setCharge(charge);
                   ele_4vec ->push_back(can);
               }
           }
           ele_index ++;
       }
       // Info(APP_NAME, "Number of electrons: %d", (int)ele_4vec->size());

       const xAOD::TruthParticleContainer* muons(0);
       CHECK( event.retrieve(muons, "TruthMuons") );
       xAOD::TruthParticleContainer::const_iterator muon_itr = muons->begin();
       xAOD::TruthParticleContainer::const_iterator muon_end = muons->end();
       // cout << "total muons: " << muons->size() << endl;
       vector<Candidate*>* muon_4vec = new vector<Candidate*>();
       int index = 0;
       for(; muon_itr != muon_end; muon_itr++){
           if( (*muon_itr)->status() == 1 && (*muon_itr)->absPdgId() == 13)
           {
               TLorentzVector mu ;
               mu.SetPxPyPzE((*muon_itr)->px(), (*muon_itr)->py(),
                       (*muon_itr)->pz(), (*muon_itr)->e());
               if(mu.Pt() > 6e3 && mu.Eta() < 2.5){
                   float charge = (*muon_itr)->pdgId()/13.;
                   Candidate* can = new Candidate(index, mu);
                   can->setCharge(charge);
                   muon_4vec->push_back(can);
               }
           }
           index ++;
       }
       // Info(APP_NAME, "Number of muons: %d", (int)muon_4vec->size());

       if(ele_4vec->size() >= 4 && test_4mu4e(ele_4vec)){
           type = 1; //4electrons
       }else if(muon_4vec->size() >= 4 && test_4mu4e(muon_4vec)){
           type = 2; //4muons
       }else if (ele_4vec->size() >=2 && muon_4vec->size() >= 2){ 
           test_2e2mu(ele_4vec, muon_4vec, type);
       }else{
            type = -1;
       }


		MyTree.Fill();
        for(int i =0; i < (int)ele_4vec->size(); i++){
            delete ele_4vec->at(i);
        }
        delete ele_4vec;
        for(int i = 0; i < (int)muon_4vec->size(); i++){
            delete muon_4vec->at(i);
        }
        delete muon_4vec;
	}
    delete mc_weights;

	fOutputFile->cd();
	MyTree.Write();
	fOutputFile->Close();

	Info( APP_NAME, "Successfully finished analysis; Exitting..." );
	return 1;
}


