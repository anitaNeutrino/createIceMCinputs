////////////////////////////////  macro written by Oindree Banerjee on April 28 2017 //////////////////////
/////////////////////////////// to check gps input file created for iceMC /////////////////////////////
////////////////////////////// by making plots //////////////////////////////////////

#include "TChain.h"
#include "TFile.h"
#include "iostream.h"
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

  TCanvas *cc = new TCanvas("cc","cc",1100,1100); 
  c.Draw("latitude:realTime","","");
  cc -> SaveAs("plots/latitude.png");
  cc -> SaveAs("plots/latitude.root"); 
  delete cc; 

  TCanvas *cc = new TCanvas("cc","cc",1100,1100); 
  c.Draw("longitude:realTime","","");
  cc -> SaveAs("plots/longitude.png");
  cc -> SaveAs("plots/longitude.root"); 
  delete cc; 

  TCanvas *cc = new TCanvas("cc","cc",1100,1100); 
  c.Draw("altitude:realTime","","");
  cc -> SaveAs("plots/altitude.png");
  cc -> SaveAs("plots/altitude.root"); 
  delete cc; 
  
  TCanvas *cc = new TCanvas("cc","cc",1100,1100); 
  c.Draw("pitch:realTime","","");
  cc -> SaveAs("plots/pitch.png");
  cc -> SaveAs("plots/pitch.root"); 
  delete cc; 

  TCanvas *cc = new TCanvas("cc","cc",1100,1100); 
  c.Draw("roll:realTime","","");
  cc -> SaveAs("plots/roll.png"); 
  cc -> SaveAs("plots/roll.root"); 
  delete cc; 


} //macro
