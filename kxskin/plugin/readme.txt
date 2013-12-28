Skinning plugins
================

In order to add custom skins for the kXL effects, it 
is necessary to generate a generic kXS file (by using
WinRar2) with a custom 'kxskin.ini' similar to the one
found in this folder and the necessary images.

(e.g. "rar a my_skin.kxs")
don't forget that the 'readme.txt' file (this file)
is not required :-)

After the kXS and kXL files are both compiled, one
should simply concatenate them by issuing a command:

copy /b <kxl file> + <kxs file>

kX Plugin Manager will automatically search the kXL
library for any custom skins and will attach them if
necessary. The attached skins have priority over any
'global' skins currently in use.

Most of the sections mentioned in the kxskin.ini file
are already found in the default 'Aqua' skin.

You may need to use a more complicated source code
in order to use skinned sliders. That is, before
executing 'create_hslider', 'create_hfader' etc...
you will need to add the following code:

fader.slider.SetBitmaps(
	mf.load_image("path-to-your-slider-image"),
  	mf.load_image("path-to-your-slider-thumb-image"),
  	mf.load_image("path-to-your-slider-thumb-image"),
  	mf.load_image("path-to-your-slider-thumb-image"));
// refer to the kSlider.SetBitmaps function for details
