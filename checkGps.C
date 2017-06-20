///////////////////////////////  macro written by Oindree Banerjee on April 28 2017 //////////////////////
/////////////////////////////// to check gps input file created for iceMC /////////////////////////////
////////////////////////////// by making plots //////////////////////////////////////

#include "TChain.h"
#include "TFile.h"
#include "TString.h"

using namespace std; 

void checkGps();

void checkGps()

{

  cout << "Hi I am going to make some plots to check that the input gps root file for iceMC, anita4gps_pitchroll.root, is looking alright" << endl; 

  TChain c("adu5PatTree");
  c.Add("anita4gps_pitchroll.root");

  TCanvas *cc = new TCanvas("cc","cc",1100,1100); 
  c.Draw("heading:realTime","","");
  cc -> SaveAs("plots/heading.png"); 
  cc -> SaveAs("plots/heading.root"); 
  delete cc; 

  TCanvas *dd = new TCanvas("dd","dd",1100,1100); 
  c.Draw("latitude:realTime","","");
  dd -> SaveAs("plots/latitude.png");
  dd -> SaveAs("plots/latitude.root"); 
  delete dd; 

  TCanvas *ee = new TCanvas("ee","ee",1100,1100); 
  c.Draw("longitude:realTime","","");
  ee -> SaveAs("plots/longitude.png");
  ee -> SaveAs("plots/longitude.root"); 
  delete ee; 

  TCanvas *ff = new TCanvas("ff","ff",1100,1100); 
  c.Draw("altitude:realTime","","");
  ff -> SaveAs("plots/altitude.png");
  ff -> SaveAs("plots/altitude.root"); 
  delete ff; 
  
  TCanvas *gg = new TCanvas("gg","gg",1100,1100); 
  c.Draw("pitch:realTime","","");
  gg -> SaveAs("plots/pitch.png");
  gg -> SaveAs("plots/pitch.root"); 
  delete gg; 

  TCanvas *hh = new TCanvas("hh","hh",1100,1100); 
  c.Draw("roll:realTime","","");
  hh -> SaveAs("plots/roll.png"); 
  hh -> SaveAs("plots/roll.root"); 
  delete hh; 


} //macro
