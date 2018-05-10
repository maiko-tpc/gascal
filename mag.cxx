/* Simulation program for Magboltz */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <TCanvas.h>
#include <TROOT.h>
#include <TApplication.h>

#include "MediumMagboltz.hh"
#include "FundamentalConstants.hh"

using namespace Garfield;
using namespace std;

int main(int argc, char *argv[]){

  
  // Magboltz configuration
  const int nFields = 40;
  const double emin = 100.;
  const double emax = 2000.;
  const double temperature = 293.15;  
  const bool useLog = false;
  const int ncoll = 10;

  double frac1, frac2, frac3;
  double press;
  char *gas1, *gas2, *gas3;
  
  char gasfile[100], datfile[100];

  if(argc == 8){
    gas1 = argv[1];
    frac1 = atof(argv[2]);
    gas2 = argv[3];
    frac2 = atof(argv[4]);
    gas3 = argv[5];
    frac3 = atof(argv[6]);
    press = atof(argv[7]);
  }


  else if(argc == 6){
    gas1 =argv[1];
    frac1=atof(argv[2]);
    gas2 =argv[3];
    frac2=atof(argv[4]);
    press=atof(argv[5]);
    
    if(fabs((frac1+frac2)-100) > 0.000001){
      cout << "" << endl;
      cout << "   error: frac1 + frac2 must be 100.0 !!" << endl;
      cout << " frsc1 + frac2 = " << frac1+frac2 << endl;
      cout << "" << endl;
      return 0;
    }
  }
  
  else if(argc == 4){
    gas1 =argv[1];
    frac1=atof(argv[2]);
    press=atof(argv[3]);

    if(frac1 != 100.0){
      cout << "" << endl;
      cout << "    useage: ./mag [gas1] 100 [hPa]" << endl;
      cout << "" << endl;
      return 0;
    }
  }

  else {
    cout << "" << endl;
    cout << "    useage: ./mag [gas1] [frac1] [gas2] [frac2] [gas3] [frac3] [hPa]" << endl;
    cout << "" << endl;
    return 0;
  }
  
  
  // output files

  /* 3 gas mix */
  if(argc == 8){
  sprintf(gasfile, "gasfile/%s(%s)+%s(%s)+%s(%s)_%s.gas", 
	  argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);

  sprintf(datfile, "dat/%s(%s)+%s(%s)+%s(%s)_%s.dat",     
	  argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
  }

  /* 2 gas mix */
  if(argc == 6){
  sprintf(gasfile, "gasfile/%s(%s)+%s(%s)_%s.gas", 
	  argv[1], argv[2], argv[3], argv[4], argv[5]);

  sprintf(datfile, "dat/%s(%s)+%s(%s)_%s.dat",     
	  argv[1], argv[2], argv[3], argv[4], argv[5]);
  }

  /* 1 gas mix */
  if(argc == 4){
  sprintf(gasfile, "gasfile/%s(%s)_%s.gas", 
	  argv[1], argv[2], argv[3]);

  sprintf(datfile, "dat/%s(%s)_%s.dat",     
	  argv[1], argv[2], argv[3]);
  }

  MediumMagboltz* gas = new MediumMagboltz();
  press = press/1013.0 * 760.0;  

  FILE *fp;
  if((fp = fopen(gasfile, "r")) == NULL){
    //    fclose(fp);
    cout << "" << endl;
    cout << "***************************" << endl;
    cout << "Start Magbolts calculation" << endl;
    printf("outfile= %s \n", gasfile);
    cout << "***************************" << endl;
    cout << "" << endl;    
    
    // set up gas
    gas -> SetTemperature(temperature);
    gas -> SetPressure(press);
    if(argc == 8) gas -> SetComposition(gas1, frac1, gas2, frac2, gas3, frac3);
    if(argc == 6) gas -> SetComposition(gas1, frac1, gas2, frac2);
    if(argc == 4) gas -> SetComposition(gas1, frac1);
    gas -> SetFieldGrid(emin, emax, nFields, useLog);
    gas -> EnableDebugging();
    gas -> GenerateGasTable(ncoll);
    gas -> DisableDebugging();
    gas -> WriteGasFile(gasfile);
  } 
  
  else {
    cout << "" << endl;
    cout << "***************************" << endl;
    cout << "gasfile already exist!!" << endl; 
    cout << "Create dat file..." << endl;
    cout << "***************************" << endl;
    cout << "" << endl;    
    fclose(fp);
  }
  
  
  
  // Making datfile output
  gas -> LoadGasFile(gasfile);
  
  std::vector<double> efields, bfields, angles;
  double driftv[nFields], diff_tra[nFields], diff_long[nFields], 
    town_a[nFields], attach[nFields];
  double ef[nFields];

  gas -> GetFieldGrid(efields, bfields, angles);
  

  FILE *outputfile;
  outputfile = fopen(datfile, "w");
  if (outputfile == NULL){
    cout << "datfile output ERROR!!" << endl;
    exit(1);
  }

  // Simulation Date
  time_t timer;
  struct tm *local;
  timer = time(NULL);
  local = localtime(&timer);

  fprintf(outputfile, "# Magboltz simulation for %s \n", gasfile);
  fprintf(outputfile, "# Simulation date: ");
  fprintf(outputfile, "%4d/", local->tm_year + 1900);
  fprintf(outputfile, "%2d/", local->tm_mon + 1);
  fprintf(outputfile, "%2d ", local->tm_mday);
  fprintf(outputfile, "%2d:", local->tm_hour);
  fprintf(outputfile, "%2d\n", local->tm_min);
  fprintf(outputfile, "\n");
  fprintf(outputfile, "# E(V/cm)     v(cm/us)    d_t(cm^(0.5))    d_l(cm^(0.5))   town_a(/cm)    attach(/cm) \n");


  // Get electron transport para and fill into the file.
  for(int i=0; i<efields.size(); i++){
    gas -> GetElectronVelocityE(i, 0, 0, driftv[i]);
    gas -> GetElectronTransverseDiffusion(i, 0, 0, diff_tra[i]);
    gas -> GetElectronLongitudinalDiffusion(i, 0, 0, diff_long[i]);
    gas -> GetElectronTownsend(i, 0, 0, town_a[i]);
    gas -> GetElectronAttachment(i, 0, 0, attach[i]);

    ef[i]= efields[i];

    // adjust the scale
    driftv[i]=driftv[i]*1000.;
    diff_tra[i]=diff_tra[i]*10000;
    diff_long[i]=diff_long[i]*10000;

    if(town_a[i]<0) town_a[i]=0;
    if(attach[i]<0) attach[i]=0;

    fprintf(outputfile, "%f    %f    %f       %f      %f       %f \n", 
	    ef[i], driftv[i], diff_tra[i], diff_long[i], town_a[i], attach[i]);
    }


  fclose(outputfile);
  return 0;

}
