// NCCG.cpp : Defines the entry point for the console application.
// Noise Canceller Code Generator

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#include "stdafx.h"
#include <fstream> // Used to write to file
#include <sstream> // Used to convert char to int

using namespace std;

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		printf("nncg takes two parameter\n");
		printf("First parameter : number of coefficients\n");
		printf("Second parameter : Where are the delayed Reference samples stored (enter a number)\n");
		printf("	1 - General Purpose Registers (default)\n");
		printf("	2 - Itram\n");	
	}
	
	if (argc > 3)
	{
		printf("Too many parameters\n");
		printf("nncg takes two parameter\n");
		printf("First parameter : number of coefficients\n");
		printf("Second parameter : Where are the delayed Reference samples stored (enter a number)\n");
		printf("	1 - General Purpose Registers (default)\n");
		printf("	2 - Itram\n");
	}
	
	if (argc == 2 || argc == 3) 
	{
		istringstream s1 ( argv[1] ) ;
		int ncoeff;
		s1 >> ncoeff;
		
		if (ncoeff >= 1000)
		{
			printf("Coefficient parameter too big\n");
		}
		else
		{
			int StorageType=0;

			if (argc == 2)
				StorageType = 1;
			else if (argc == 3) 
			{
				istringstream s2 ( argv[2] ) ;
				s2 >> StorageType;
			}
	
			if (StorageType == 1)
			{
				char filename[256];
			
				strcpy(filename,"nc");
				strcat(filename,argv[1]);
				strcat(filename,".da");
				ofstream nccgfile( filename , ios::out );
				if ( !nccgfile ) 
					printf("The file could not be created.");
					
				if ( nccgfile )
				{
					printf("The file was created.\n");
			
					nccgfile << "; New microcode" << endl;
					nccgfile << "name \"nc" << ncoeff << "\";" << endl;
					nccgfile << "copyright \"Copyright (c) 2005. Tril\";" << endl;
					nccgfile << "created \"2005\";" << endl;
					nccgfile << "engine \"kX\";" << endl;
					nccgfile << "; comment \"\"" << endl;
					nccgfile << "; guid to add here.  Right-click the mixer icon, go to Settings," << endl;
					nccgfile << "; click Generate GUID and paste it below in \"\"" << endl;
					nccgfile << "guid \"\"" << endl;
					nccgfile << "; -- generated GUID above" << endl << endl;
			
					nccgfile << "; This filter uses the LMS algorithm to adapt dynamically" << endl;
					nccgfile << "; the FIR filter coefficients." << endl;
					nccgfile << "; Primary is noise and desired signal" << endl;
					nccgfile << "; Reference is noise only" << endl;
					nccgfile << "; It works better at removing periodic noise." << endl;
					nccgfile << "; There are two outputs :" << endl;
					nccgfile << "; - Correlated output is the output where the noise was substracted." << endl;
					nccgfile << "; - Uncorrelated output is what was substracted from the output." << endl << endl;
			
					nccgfile << "input Primary, Reference;" << endl;
					nccgfile << "output OutCorr, OutUncorr;" << endl << endl;
					
					nccgfile << "; Mu is a coefficent to control the convergence speed.  You want it" << endl;
					nccgfile << "; as big as possible without causing the filter to diverge." << endl << endl;
					nccgfile << "control MasterVol=1, CorrVol=1 , UncorrVol=1, Mu=1;" << endl << endl;
	              
					nccgfile << "; Coefficients" << endl;
		
					int i;
					for (i = (ncoeff - 1); i >= 0; i--)
					{
						if (i <= 9)
						{
							nccgfile << "static H00" << i << "=0;" << endl;
						}
						else if (i >= 10 && i <= 99)
						{
							nccgfile << "static H0" << i << "=0;" << endl;
						}
						else if (i >= 100 && i <= 999)
						{
							nccgfile << "static H" << i << "=0;" << endl;
						}
					}
			
					nccgfile << endl;
					nccgfile << "; Delayed Reference" << endl;
		
					for (i = (ncoeff - 1); i >= 0; i--)
					{
						if (i <= 9)
						{
							nccgfile << "static R00" << i << "=0;" << endl;
						}
						else if (i >= 10 && i <= 99)
						{
							nccgfile << "static R0" << i << "=0;" << endl;
						}
						else if (i >= 100 && i <= 999)
						{
							nccgfile << "static R" << i << "=0;" << endl;
						}
					}
			
					nccgfile << endl;
					nccgfile << "static FilterOutput=0, Error=0;" << endl;
					nccgfile << "temp x, MuXError;" << endl;
					nccgfile << "temp CorrVolume, UncorrVolume;" << endl << endl;
				          
					nccgfile << "; code" << endl << endl;
			
					nccgfile << "; hn+1[i] = hn[i] + mu*errorn*input[n-i]" << endl;
					nccgfile << "; where" << endl;
					nccgfile << "; hn+1[i] : final LMS coefficient update" << endl;
					nccgfile << "; mu : step-size" << endl;
					nccgfile << "; n : sample" << endl;
					nccgfile << "; i : coefficient number" << endl << endl;;
				 
					nccgfile << "macs R000, Reference, 0, 0;" << endl << endl;
			
					nccgfile << "; Calculate FilterOutput with Fir filter" << endl;
					nccgfile << "macs 	0,  0,  0,  0;" << endl;
			
					for (i = (ncoeff - 1); i >= 0; i--)
					{
						if (i <= 9)
						{
							nccgfile << "macmv   x,  x,  " << "R00" << i << ", H00" << i << ";" << endl;
						}
						else if (i >= 10 && i <= 99)
						{
							nccgfile << "macmv   x,  x,  " << "R0" << i << ", H0" << i << ";" << endl;
						}
						else if (i >= 100 && i <= 999)
						{
							nccgfile << "macmv   x,  x,  " << "R" << i << ", H" << i << ";" << endl;
						}
					}
		
					nccgfile << "macs  x, accum, 0,0;" << endl;
					nccgfile << "macs  FilterOutput, x, 0, 0;" << endl << endl;
			
					nccgfile << "; Calculate error" << endl;
					nccgfile << "macints Error, Primary, FilterOutput, -1;" << endl << endl;
			
					nccgfile << "; Calculate coefficients" << endl;
					nccgfile << "macs MuXError, 0, Mu, Error;" << endl;
					for (i = (ncoeff - 1); i >= 0; i--)
					{
						if (i <= 9)
						{
							nccgfile << "macs H00" << i << ", H00" << i <<", MuXError, R00" << i << ";" << endl;
						}
						else if (i >= 10 && i <= 99)
						{
							nccgfile << "macs H0" << i << ", H0" << i <<", MuXError, R0" << i << ";" << endl;
						}
						else if (i >= 100 && i <= 999)
						{
							nccgfile << "macs H" << i << ", H" << i <<", MuXError, R" << i << ";" << endl;
						}
					}
				
					nccgfile << endl;
					nccgfile << "; Output" << endl;
					nccgfile << "macs CorrVolume, 0, CorrVol, MasterVol;" << endl;
					nccgfile << "macs UncorrVolume, 0, UncorrVol, MasterVol;" << endl;
					nccgfile << "macs OutCorr, 0, Error, CorrVolume;" << endl;
					nccgfile << "macs OutUncorr, 0, FilterOutput, UncorrVolume;" << endl << endl;
		
					nccgfile << "; Delay Reference" << endl;
					for (i = (ncoeff - 1); i >= 1; i--)
					{
						if (i <= 9)
						{
							nccgfile << "macs R00" << i << ", R00" << i - 1 << ", 0, 0;" << endl;
						}
						else if (i == 10)
						{
							nccgfile << "macs R0" << i << ", R00" << i - 1 << ", 0, 0;" << endl;
						}
						else if (i >= 11 && i <= 99)
						{
							nccgfile << "macs R0" << i << ", R0" << i - 1 << ", 0, 0;" << endl;
						}
						else if (i == 100)
						{
							nccgfile << "macs R" << i << ", R0" << i - 1 << ", 0, 0;" << endl;
						}
						else if (i >= 101 && i <= 999)
						{
							nccgfile << "macs R" << i << ", R" << i - 1 << ", 0, 0;" << endl;
						}
					}

					nccgfile << endl << "end" << endl << endl;
		
					printf("Write to file completed.\nAdd a GUID in the .da file, place it somewhere on your harddrive and register it.");
				}
				nccgfile.close();
			}
			else if (StorageType == 2)
			{
				char filename[256];
			
				strcpy(filename,"nc");
				strcat(filename,argv[1]);
				strcat(filename,".da");
				ofstream nccgfile( filename , ios::out );
				if ( !nccgfile ) 
					printf("The file could not be created.");
					
				if ( nccgfile )
				{
					printf("The file was created.\n");
		
					nccgfile << "; New microcode" << endl;
					nccgfile << "name \"nc" << ncoeff << "\";" << endl;
					nccgfile << "copyright \"Copyright (c) 2005. Tril\";" << endl;
					nccgfile << "created \"2005\";" << endl;
					nccgfile << "engine \"kX\";" << endl;
					nccgfile << "; comment \"\"" << endl;
					nccgfile << "; guid to add here.  Right-click the mixer icon, go to Settings," << endl;
					nccgfile << "; click Generate GUID and paste it below in \"\"" << endl;
					nccgfile << "guid \"\"" << endl;
					nccgfile << "; -- generated GUID above" << endl << endl;
			
					nccgfile << "; itramsize " << ncoeff - 1 << endl << endl;
		
					nccgfile << "; This filter uses the LMS algorithm to adapt dynamically" << endl;
					nccgfile << "; the FIR filter coefficients." << endl;
					nccgfile << "; Primary is noise and desired signal" << endl;
					nccgfile << "; Reference is noise only" << endl;
					nccgfile << "; It works better at removing periodic noise." << endl;
					nccgfile << "; There are two outputs :" << endl;
					nccgfile << "; - Correlated output is the output where the noise was substracted." << endl;
					nccgfile << "; - Uncorrelated output is what was substracted from the output." << endl << endl;
			
					nccgfile << "input Primary, Reference;" << endl;
					nccgfile << "output OutCorr, OutUncorr;" << endl << endl;
					
					nccgfile << "; Mu is a coefficent to control the convergence speed.  You want it" << endl;
					nccgfile << "; as big as possible without causing the filter to diverge." << endl << endl;
					nccgfile << "control MasterVol=1, CorrVol=1 , UncorrVol=1, Mu=1;" << endl << endl;
	              
					nccgfile << "; Coefficients" << endl;
		
					int i;
					for (i = (ncoeff - 1); i >= 0; i--)
					{
						if (i <= 9)
						{
							nccgfile << "static H00" << i << "=0;" << endl;
						}
						else if (i >= 10 && i <= 99)
						{
							nccgfile << "static H0" << i << "=0;" << endl;
						}
						else if (i >= 100 && i <= 999)
						{
							nccgfile << "static H" << i << "=0;" << endl;
						}
					}
			
					nccgfile << endl;
					nccgfile << "; Delayed Reference" << endl;
					nccgfile << "static R001=0;" << endl;
					nccgfile << "static R000=0;" << endl << endl;

					nccgfile << "static FilterOutput=0, Error=0;" << endl;
					nccgfile << "temp x, MuXError;" << endl;
					nccgfile << "temp CorrVolume, UncorrVolume;" << endl << endl;
				          
					nccgfile << "; itram" << endl;
					nccgfile << "idelay write wReference at 0;" << endl << endl;
					
					for (i = (ncoeff - 1); i >= 2; i--)
					{
						if (i <= 9)
						{
							nccgfile << "idelay read R00" << i << " at " << i << ";" << endl;
						}
						else if (i >= 10 && i <= 99)
						{
							nccgfile << "idelay read R0" << i << " at " << i << ";" << endl;
						}
						else if (i >= 100 && i <= 999)
						{
							nccgfile << "idelay read R" << i << " at " << i << ";" << endl;
						}
					}
					nccgfile << endl << endl;

					nccgfile << "; code" << endl << endl;;

					nccgfile << "; hn+1[i] = hn[i] + mu*errorn*input[n-i]" << endl;
					nccgfile << "; where" << endl;
					nccgfile << "; hn+1[i] : final LMS coefficient update" << endl;
					nccgfile << "; mu : step-size" << endl;
					nccgfile << "; n : sample" << endl;
					nccgfile << "; i : coefficient number" << endl << endl;;
				 
					nccgfile << "macs R000, Reference, 0, 0;" << endl;
					nccgfile << "macs wReference, R000, 0, 0;" << endl << endl;
			
					nccgfile << "; Calculate FilterOutput with Fir filter" << endl;
					nccgfile << "macs 	0,  0,  0,  0;" << endl;
			
					for (i = (ncoeff - 1); i >= 0; i--)
					{
						if (i <= 9)
						{
							nccgfile << "macmv   x,  x,  " << "R00" << i << ", H00" << i << ";" << endl;
						}
						else if (i >= 10 && i <= 99)
						{
							nccgfile << "macmv   x,  x,  " << "R0" << i << ", H0" << i << ";" << endl;
						}
						else if (i >= 100 && i <= 999)
						{
							nccgfile << "macmv   x,  x,  " << "R" << i << ", H" << i << ";" << endl;
						}
					}
		
					nccgfile << "macs  x, accum, 0,0;" << endl;
					nccgfile << "macs  FilterOutput, x, 0, 0;" << endl << endl;
			
					nccgfile << "; Calculate error" << endl;
					nccgfile << "macints Error, Primary, FilterOutput, -1;" << endl << endl;
			
					nccgfile << "; Calculate coefficients" << endl;
					nccgfile << "macs MuXError, 0, Mu, Error;" << endl;
					for (i = (ncoeff - 1); i >= 0; i--)
					{
						if (i <= 9)
						{
							nccgfile << "macs H00" << i << ", H00" << i <<", MuXError, R00" << i << ";" << endl;
						}
						else if (i >= 10 && i <= 99)
						{
							nccgfile << "macs H0" << i << ", H0" << i <<", MuXError, R0" << i << ";" << endl;
						}
						else if (i >= 100 && i <= 999)
						{
							nccgfile << "macs H" << i << ", H" << i <<", MuXError, R" << i << ";" << endl;
						}
					}
				
					nccgfile << endl;
					nccgfile << "; Output" << endl;
					nccgfile << "macs CorrVolume, 0, CorrVol, MasterVol;" << endl;
					nccgfile << "macs UncorrVolume, 0, UncorrVol, MasterVol;" << endl;
					nccgfile << "macs OutCorr, 0, Error, CorrVolume;" << endl;
					nccgfile << "macs OutUncorr, 0, FilterOutput, UncorrVolume;" << endl << endl;
		
					nccgfile << "; Delay Reference" << endl;		
					nccgfile << "macs R001, R000, 0, 0;" << endl << endl;

					nccgfile << "end" << endl << endl;
		
					printf("Write to file completed.\nAdd a GUID in the .da file, place it somewhere on your harddrive and register it.");
				}
				nccgfile.close();
			}
		}
	}
	

	return 0;
}

