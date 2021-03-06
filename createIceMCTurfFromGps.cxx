#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaEvent.h"
#include "TurfRate.h"
#include "SummedTurfRate.h"
#include "TimedAnitaHeader.h"
#include "PrettyAnitaHk.h"
// #include "ProgressBar.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH2.h"
#include "TColor.h"
#include "TMath.h"
#include "TStyle.h"
#include "TSystem.h"
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[]){

  if(!(argc==3 || argc==2)){
    std::cerr << "Usage 1: " << argv[0] << " [run]" << std::endl;
    std::cerr << "Usage 2: " << argv[0] << " [firstRun] [lastRun]" << std::endl;
    return 1;
  }

  std::cout << argv[0] << "\t" << argv[1];
  if(argc==3){std::cout << "\t" << argv[2];}
  std::cout << std::endl;
  const Int_t firstRun = atoi(argv[1]);
  const Int_t lastRun = argc==3 ? atoi(argv[2]) : firstRun;


  FileStat_t staty;
  char turfName[FILENAME_MAX];
  // char gpsName[FILENAME_MAX];

  //  AnitaVersion::set(3);

  TFile *finput = new TFile("anita3gps_pitchroll_3.root", "read");
  TTree *icemcGpsTree = (TTree*)finput->Get("adu5PatTree");
  icemcGpsTree->SetName("icemcGpsTree");
  
  Int_t icemcRealTime;
  icemcGpsTree->SetBranchAddress("realTime", &icemcRealTime);

  Int_t numGpsEntries = icemcGpsTree->GetEntries();


  TurfRate *turf =0;
  // Adu5Pat *pat = 0;
  TChain *turfChain = new TChain("turfRateTree");
  // TChain *gpsChain = new TChain("adu5PatTree");

  for (int run=firstRun;run<lastRun+1; run++){
    sprintf(turfName,"/unix/anita3/flight1415/root/run%d/turfRateFile%d.root",run,run);
    // sprintf(gpsName,"/unix/anita3/flight1415/root/run%d/gpsFile%d.root",run,run);
    if(gSystem->GetPathInfo(turfName,staty)) {
      continue;
    }
    turfChain->Add(turfName);
    // gpsChain->Add(gpsName);
  }

  turfChain->SetBranchAddress("turf",&turf);
  // gpsChain->SetBranchAddress("pat", &pat);

  turfChain->BuildIndex("turf->realTime");

  UInt_t numEntries=turfChain->GetEntries();
  cout << "There are " << numEntries << " in total "<< endl;


  UInt_t phiTrigMask ;
  UInt_t l1TrigMask ;
  UInt_t phiTrigMaskH ;
  UInt_t l1TrigMaskH ;
  Double_t deadTime ;
  UInt_t realTime ;


  TTree* tree = new TTree();
  tree->Branch("realTime",     &realTime,     "realTime/i"    );
  tree->Branch("phiTrigMask",  &phiTrigMask,  "phiTrigMask/I" );
  tree->Branch("phiTrigMaskH", &phiTrigMaskH, "phiTrigMaskH/I");
  tree->Branch("l1TrigMask",   &l1TrigMask,   "l1TrigMask/I"  );
  tree->Branch("l1TrigMaskH",  &l1TrigMaskH,  "l1TrigMaskH/I" );
  tree->Branch("deadTime",     &deadTime,     "deadTime/D"    );


  // ProgressBar p(numGpsEntries);

  for (unsigned int ientry=0; ientry<numGpsEntries; ientry++){
    icemcGpsTree->GetEntry(ientry);
    
   
    int index = turfChain->GetEntryNumberWithBestIndex(icemcRealTime);

    if (index<0) {
      realTime = icemcRealTime;
      phiTrigMask=l1TrigMask=phiTrigMaskH=l1TrigMaskH=65536;
      deadTime = 0;
      cout << "Can't find even best Index for this " << icemcRealTime << endl;

    } else {

      turfChain->GetEntry(index);
      
      realTime = turf->realTime;
      int diffTime = (int)icemcRealTime - (int)realTime;

      if (TMath::Abs(diffTime)>60) cout << icemcRealTime << " " << realTime << " " << diffTime << endl;
      
      phiTrigMask  = (turf->phiTrigMask );
      l1TrigMask   = (turf->l1TrigMask  );
      phiTrigMaskH = (turf->phiTrigMaskH);
      l1TrigMaskH  = (turf->l1TrigMaskH );
      deadTime     = (turf->getDeadTimeFrac() );
      std::cout << l1TrigMask << " " << l1TrigMaskH << std::endl;

    }
    tree->Fill();
      
    // p++;
  }
  
  cout << "Creating output" << endl;
  TFile *output = new TFile ("partial/SampleTurf_icemc_anita3.root", "recreate");
  tree->Write("turfrate_icemc");
  cout << "Done " << endl;
  output->Close();
  cout << "Closed " << endl;

  return 0;

}
