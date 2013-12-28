Noise Canceller plugins for the kX drivers
Author : Tril

I included NCCG (noise canceller code generator).  It's a console app I made to write the .da files.  I included the source code and the compiled .exe.

nncg takes two parameter
First parameter : number of coefficients
Second parameter : Where are the delayed Reference samples stored (enter a number)
	1 - General Purpose Registers (default)
	2 - Itram

The second parameter is optional.

How to use nncg
Example : 

nncg 50 1
nncg 50

These two command do the same thing.  They write a file called nc50.da in the current directory.

You have to add a GUID in the .da file, place it somewhere on your harddrive and register it.  To add a GUID, right-click the file and click Edit, then follow the directions written in the file on how to add a GUID.