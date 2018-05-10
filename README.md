Program for Magboltz calculation and plot
====

## Description
The program "mag" is to simulate drift velocity, diffusion, and townsent coefficietnt as a fuction of the drift field.  
The program "plot" is to plot the results of "mag".  
The output files (*.gas, *.dat) are not committed to the repository.

## Requirement
[Garfield++](https://garfieldpp.web.cern.ch/garfieldpp/) and [ROOT](https://root.cern.ch/) should be installed.

## Usage
-./mag [gas1] [frac1] [gas2] [frac2] [gas3] [frac3] [hPa]  
-./plot [gas1] [frac1] [gas2] [frac2] [gas3] [frac3] [hPa] [opt]  
        [opt] = dv     (for drift velocity)  
	        dt     (for transverse diffusion)  
		dt     (for longitudinal diffusion)  
		town   (for townsend coefficient)  
		attach (for attachment coefficient)  

-Examples of mag:  
	  -./mag He 100 500 (for pure gas)  
	  -./mag He 96 CO2 4 500 (for mixture gas)  
	  -./mag He 95 CO2 3 iC4H10 2 1000 (up to 3 gasses can be used)  
  
-Example of plot  
	 -./plot He 100 500 dv (plot drift velocity)  
	 -./plot He(96)+CO2(4)_500.dat dt (you can give the files directly)
	 
	 

