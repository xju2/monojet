// System include(s):
#include <memory>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TChain.h>
#include <TH1F.h>
#include <TLorentzVector.h>

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
#include "xAODCore/ShallowCopy.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "CPAnalysisExamples/errorcheck.h"
#include "SUSYTools/SUSYObjDef_xAOD.h"

// Other includes
#include "MonoJet/Truth_JetMET.h"

using namespace std;
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

	ifstream input_file( inputFileName.c_str() );
	TString name_file;
	TChain* fc = new TChain("CollectionTree");
	while ( input_file>>name_file ) fc->Add(name_file);

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

	double MET_etx;
	double MET_ety;
    double MET_et;
    double MET_phi;

    double leading_y_pt;
    double sub_leading_y_pt;
    double m_yy;
    double pt_yy;


	TFile *fOutputFile = new TFile( "reduced_ntuple.root", "recreate" );
	TTree MyTree( "physics", "physics" );

	MyTree.Branch("RunNumber", &RunNumber, "RunNumber/I");
	MyTree.Branch("EventNumber", &EventNumber, "EventNumber/I");
	MyTree.Branch("mc_channel_number", &mc_channel_number, "mc_channel_number/I");
	MyTree.Branch("MCWeight", &MCWeight, "MCWeight/D");

	MyTree.Branch("MET_etx", &MET_etx, "MET_etx/D");
	MyTree.Branch("MET_ety", &MET_ety, "MET_ety/D");
	MyTree.Branch("MET_et", &MET_et, "MET_et/D");
	MyTree.Branch("MET_phi", &MET_phi, "MET_phi/D");

	MyTree.Branch("leading_y_pt", &leading_y_pt, "leading_y_pt/D");
	MyTree.Branch("sub_leading_y_pt", &sub_leading_y_pt, "sub_leading_y_pt/D");
    MyTree.Branch("m_yy", &m_yy, "m_yy/D");
    MyTree.Branch("pt_yy", &pt_yy, "pt_yy/D");

    
	for( Long64_t entry = 0; entry < entries; ++entry ) {

		// Tell the object which entry to look at:
		event.getEntry( entry );

		const xAOD::EventInfo* ei = 0;
		CHECK( event.retrieve( ei, "EventInfo" ) );

		RunNumber = ei->runNumber();
		EventNumber = ei->eventNumber();
		mc_channel_number = ei-> mcChannelNumber();
		MCWeight = ei->mcEventWeight();

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

       ////get photons 
       const xAOD::TruthParticleContainer* photons(0);
       CHECK( event.retrieve(photons,"TruthPhotons") );
       xAOD::TruthParticleContainer::const_iterator ph_itr = photons->begin();
       xAOD::TruthParticleContainer::const_iterator ph_end = photons->end();
       int index_leading = -1;
       int index_sub = -1;
       TLorentzVector tlv_leading ;
       TLorentzVector tlv_sub;
       int ph_id = 0;
       for(; ph_itr != ph_end; ph_itr ++){
           if( (*ph_itr)->status() == 1 && (*ph_itr)->absPdgId() == 22)
           {
               TLorentzVector ph;
               ph.SetPxPyPzE((*ph_itr)->px(),(*ph_itr)->py(),
                       (*ph_itr)->pz(), (*ph_itr)->e());
               if(ph.Eta() < 2.47) {
                   if(index_leading < 0 || ph.Pt() > tlv_leading.Pt()){
                       tlv_sub = tlv_leading;
                       tlv_leading = ph;
                       index_sub = index_leading;
                       index_leading = ph_id;
                   } else if(index_sub < 0 || ph.Pt() > tlv_sub.Pt()){
                       tlv_sub = ph;
                       index_sub = ph_id;
                   } else {
                       ;
                   }
               }
           }
           ph_id ++;
       }
       if (index_leading != -1 && index_sub != -1){
            TLorentzVector tlv_yy(tlv_leading+tlv_sub);
            m_yy = tlv_yy.M();
            pt_yy = tlv_yy.Pt();
            leading_y_pt = tlv_leading.Pt();
            sub_leading_y_pt = tlv_sub.Pt();
       } else {
            m_yy = -999;
            pt_yy = -999;
       }

		MyTree.Fill();
	}

	fOutputFile->cd();
	MyTree.Write();
	fOutputFile->Close();

	Info( APP_NAME, "Successfully finished analysis; Exitting..." );
	return 1;
}


