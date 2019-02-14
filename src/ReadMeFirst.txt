Unpack the ZIP file into an empty directory,
the name suggested is ARM_Radio

Inside the ARM_SDR.uvproj  XML file, in the 
<IncludeFiles> tag, there is a list of paths
for the include files.
Apart from the project directory, the path for
the various system files specified there is
that of PC where the project was developed, namely : 

H:\Applications\Keil_v5\ARM\Pack

Please change it according to the Keil compiler
installation on your PC.

Alternatively, after having started uVision, you
can do the same by selecting the "Options for Target" 
choice, "C/C++" tab, and change the paths there.

This project was built and tested with 
Keil uVision 5.15 with the Legacy Support installed, which
can be downloaded from here :

http://www2.keil.com/mdk5/legacy/

The ARM_Radio.bin object file produced by the 
compilator can be downloaded to the flash memory of
the ARM chip by using the ST-Link utility found here:

http://www.st.com/web/catalog/tools/FM147/SC1887/PF258168#


Alberto di Bene    i2phd@weaksignals.com


---------------------------------------------------------------
						                                      
This file is part of ARM_Radio.

ARM_Radio is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ARM_Radio is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ARM_Radio.  It is contained in the file Copying.txt in the
same ZIP file where this file was extracted from.				                                      
---------------------------------------------------------------

