#include "AnitaConventions.h"
#include "AnitaGeomTool.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaEvent.h"
#include "Adu5Pat.h"
#include "TimedAnitaHeader.h"
#include "PrettyAnitaHk.h"
#include "ProgressBar.h"
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

using namespace Acclaim;
using namespace std;

void getCartesianCoords(Double_t lat, Double_t lon, Double_t alt, Double_t p[3]);

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
  cout << "first run is " << firstRun << endl; 
  cout << "last run is " << lastRun << endl; 

  AnitaVersion::set(4);

  FileStat_t staty;
  char gpsName[FILENAME_MAX];
  
  Adu5Pat *pat = 0;
  TChain *gpsChain = new TChain("adu5PatTree");
  
  for (int run=firstRun;run<lastRun+1; run++){
    sprintf(gpsName,"/unix/anita4/flight2016/root/run%d/timedGpsEvent%d.root",run,run);
    cout << gpsName << endl;
    if(gSystem->GetPathInfo(gpsName,staty)) {
      continue;
    }
    gpsChain->Add(gpsName);
  }
  
  gpsChain->SetBranchAddress("pat", &pat);
  
  gpsChain->BuildIndex("pat->realTime");
  
  UInt_t numEntries=gpsChain->GetEntries();
  cout << "There are " << numEntries << " in total "<< endl;
  const int granularity = 60;
  cout << "The chosen granularity is : " << granularity << " seconds" << endl;
  
  gpsChain->GetEntry(0);
  double firstTS = pat->realTime;
  gpsChain->GetEntry(numEntries-1);
  double lastTS = pat->realTime;
  
  time_t tt = firstTS;
  struct tm *tm = localtime(&tt);
  char date[30];
  strftime(date, sizeof(date), "%Y-%m-%d, %H:%M", tm);
  char today[30];
  strftime(today, sizeof(today), "%Y-%b-%d", tm);
  cout << "Events starting from : " << date << endl;
  tt = lastTS;
  tm = localtime(&tt);
  strftime(date, sizeof(date), "%Y-%m-%d, %H:%M", tm);
  cout << "And finishing on : " << date << endl;

  UInt_t realTime = firstTS;
  Float_t pitch, roll;
  Float_t heading, latitude, longitude, altitude;
  
  TTree* tree = new TTree();
  tree->Branch("heading",      &heading,      "heading/F"     );
  tree->Branch("latitude",     &latitude,     "latitude/F"    );
  tree->Branch("longitude",    &longitude,    "longitude/F"   );
  tree->Branch("altitude",     &altitude,     "altitude/F"    );
  tree->Branch("realTime",     &realTime,     "realTime/I"    );
  tree->Branch("pitch",        &pitch,        "pitch/F"       );
  tree->Branch("roll",         &roll,         "roll/F"        );
  
  
  // int count = 0;
  // TH2D *htemp = new TH2D("htemp", "", 1000, -2e3, +2e3, 1000, -2e3, +2e3);

  UInt_t realTime_old = firstTS;

  // Skip from 1420749855 to 1420751715 
  Int_t badPeriodStart = 1420749800;
  Int_t badPeriodEnd   = 1420751715;
              
  ProgressBar p((lastTS-firstTS)/granularity);
  
  //for (unsigned int entry=0;entry<numEntries;entry++){

  while(realTime_old<=lastTS){

    while (realTime_old>=badPeriodStart && realTime_old<=badPeriodEnd) realTime_old=realTime_old+granularity;

    int index = gpsChain->GetEntryNumberWithIndex(realTime_old);
    p++;

   

    if (index>0){
      gpsChain->GetEntry(index);

    } else {

      index = gpsChain->GetEntryNumberWithBestIndex(realTime_old);
      gpsChain->GetEntry(index);

      while (TMath::Abs((int)pat->realTime-(int)realTime_old)>70 || pat->realTime<realTime_old){
	cout << pat->realTime << " " << realTime_old << endl;
	realTime_old=realTime_old+granularity;
	index = gpsChain->GetEntryNumberWithBestIndex(realTime_old);
	gpsChain->GetEntry(index);
      }

    }


    // gpsChain->GetEntry(entry);
    // realTime = pat->realTime;
    // if (realTime-realTime_old<granularity) continue;
    // // cout << realTime << endl;
    // // int temp = int(realTime-firstTS);
    // // if ( (temp%granularity)==0){
    // while (pat->heading<0 || isnan(pat->heading)){
    //   entry++;
    //   gpsChain->GetEntry(entry);
    //   p++;
    // }
         
    realTime  = pat->realTime;
    heading   = pat->heading;
    latitude  = pat->latitude;
    longitude = pat->longitude;
    altitude  = pat->altitude;
    pitch     = pat->pitch;
    roll      = pat->roll;
    realTime_old = realTime+granularity;
    tree->Fill();



    // double pos[3];
    // getCartesianCoords(latitude, longitude, altitude, pos);
    // htemp->Fill(pos[0], pos[1]);
    // cout << pos[0] << " " << pos[1] << endl;
    // count ++;
      
  

  }
  
  cout << "Saving" << endl;
  TFile *output = new TFile ("anita4gps_pitchroll.root", "recreate");
  tree->Write("adu5PatTree");
  cout << "Saved" << endl;
  output->Close();

  return 0;

}

void getCartesianCoords(Double_t lat, Double_t lon, Double_t alt, Double_t p[3])
{
  if(lat<0) lat*=-1;
   //Note that x and y are switched to conform with previous standards
   lat*=TMath::DegToRad();
   lon*=TMath::DegToRad();
   //calculate x,y,z coordinates
   Double_t C2=pow(cos(lat)*cos(lat)+(1-FLATTENING_FACTOR)*(1-FLATTENING_FACTOR)*sin(lat)*sin(lat),-0.5);
   Double_t Q2=(1-FLATTENING_FACTOR)*(1-FLATTENING_FACTOR)*C2;
   p[1]=(R_EARTH*C2+alt)*TMath::Cos(lat)*TMath::Cos(lon)/1000;
   p[0]=(R_EARTH*C2+alt)*TMath::Cos(lat)*TMath::Sin(lon)/1000;
   p[2]=(R_EARTH*Q2+alt)*TMath::Sin(lat)/1000;
}
