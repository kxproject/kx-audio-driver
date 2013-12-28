<!-- kX Project readme -->
<!-- Copyright (c) kX Project, 2001-2014. All rights reserved. -->
<html>
 <head>
  <title>kX Project Readme</title>
 </head>

<body>

<hr>
<h2>kX Driver Readme - generated __DATE__ __TIME__<br>kX Audio Driver (c) 2001-2014 Eugene Gavrilov.  All rights reserved.</h2>
<hr>

<h4>
The kX Project is an on-going independent development project; visit 
the kX Project website for the latest news and updates: 
<a href="http://www.kxproject.com" target=new>www.kxproject.com</a>

<br>

Please review both the FAQ and the list of Known Issues at the 
kX Project website prior to installing the kX Driver. Read the 
bug reporting procedure prior to submitting bug reports.

<br>

<h2>Contents</h2>

<ul>
<li><a href=#toc1>Description and Features</a></li>
<li><a href=#toc3>System Requirements</a></li>
<li><a href=#toc4>Installation</a></li>
<li><a href=#toc5>Known Issues</a></li>
<li><a href=#toc6>Contact</a></li>
<li><a href=#toc7>Version History</a></li>
<li><a href=#toc8>Future Development</a></li>
<li><a href=#toc9>FAQ</a></li>
<li><a href=#toc10>Trademarks & Legal Disclaimer</a></li>
<li><a href=#toc11>License Agreement</a></li>
</ul>

<a name=toc1></a><h2>1. Description and Features</h2>

#include "help\\intro.htm"

Development of the kX Driver is on-going and new features are continually 
being added; visit the <a href="http://www.kxproject.com">kX Project website</a> for news and updates.<br>

A Software Developers Kit (SDK) for the kX Application Programming Interface
(API) is available as a separate download from the kX Project website.<br>

<a name=toc3></a><h2>2. System Requirements</h2>

The kX driver is a WDM driver and will only install on systems running 
<strike>Microsoft Windows 98 Second Edition, Windows Millenium Edition, 
Windows 2000</strike>, Windows XP, Windows Vista, Windows 7 or later.<br>

<ul>
<li>At least one kX-compatible soundcard is required. Currently, kX-compatible 
soundcards include all EMU10k1 and EMU10k2-based PC soundcards manufactured 
by Creative Technology Ltd. and/or E-mu Sytems Inc. (review the FAQ for a list of cards).<br></li>

<li>A minimum of 64 MB of RAM is required.<br></li>

<li>A video adapter capable of displaying HiColor (16 bit) or TrueColor (24bit) 
is highly recommended.<br></li>
</ul>

<a name=toc4></a><h2>3. Installation</h2>

Removal of any and all manufacturer-supplied drivers and software is 
recommended.<br>

To install the kX Driver, run the installation executable. It will extract
all the driver files into your Program Files folder and will launch the
kxsetup.exe program to continue the installation; the 
driver files will be installed into the Windows System directory (Winnt\System32 or Windows\System). 
Follow the directions on-screen and re-boot once all drivers have been installed.<br>

<strike>On Windows 98 systems only, it may be necessary to re-boot a second time after 
installing, to ensure proper functioning of the kX Mixer</strike>.<br>

Should setup fail, the kX Driver can be installed manually via the Windows 
Device Manager (Update Driver function). When asked to Specify Location, browse 
to the kx.inf file in the directory on your harddrive where the driver package 
has been uncompressed (e.g. C:\Program Files\kX Project\35xx).<br>

Verify that that the following Multimedia 
Audio Properties are properly set (applies to Windows XP and Windows 2000 only): 
<ul>
<li> 'Hardware acceleration' set to 'Full'</li>
<li> 'Sample rate conversion quality' set to 'Best' </li>
<li> 'Speaker setup' set to '5.1 Surround sound' </li>
</ul>
(control panel/multimedia/audio/advanced properties)<br>

<b>These settings are required for proper operation of the kX Driver and kX Mixer!</b>.<br>

To launch the kX Mixer, double-click the kX icon in the windows tray. Right-click
the icon for access to additional functions. The kX Mixer is loaded automatically
on start-up. If you don't see kX Mixer icon in your system tray, the driver
might have installed incorrectly.<br>

<a name=toc5></a><h2>4. Known Issues</h2>

#include "help\\issues.htm"

<a name=toc6></a><h2>5. Contact</h2>

Post your comments & bug reports to our support forum.<br>

Visit the kX Project website for the latest updates and more information:
<a href="http://www.kxproject.com">www.kxproject.com</a><br>

kX Project Forums:
<ul>
 <li><a href="http://www.kxproject.com/forums.php" class=fade target=_blank>kX Project Official Forums</a></li>
</ul>

kX Project Contact information: <a href="http://www.kxproject.com/contact.php">www.kxproject.com/contact.php</a><br>

<a name=toc7></a><h2>6. Version History and Changelog</h2>

<a href='toc8'>(Skip)</a><br>

#include "help\\changes.htm"

<a name=toc8></a><h2>7. Planned Future Development</h2>

#include "help\\todo.htm"

<a name=toc9></a><h2>8. FAQ </h2>

<a href='toc10'>(Skip)</a><br>

#include "help\\faq.htm"

<a name=toc10></a><h2>9. Trademarks & Legal Disclaimer</h2>

#include "help\\copyright.htm"

<a name=toc11></a><h2>10. License Agreement</h2>

<pre>
#include "help\\license.txt"
</pre>

</body>
</html>
