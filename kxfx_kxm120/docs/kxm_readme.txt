stylus kx modular system 1
--------------------------

the kxm system 1 is a virtual analog modular synthesizer like moog modular
or doepfer on the soundblaster cards sblive!, audigy with kx- driver.


module overview:

kxm 1xx - keyboard related stuff (kbu's, octave divide/mult..) 
kxm 2xx - vco's
kxm 3xx - mixers
kxm 4xx - envelopes
kxm 5xx - vcf's
kxm 6xx - vca's
kxm 7xx - lfo's
kxm 8xx - shapers
kxm 9xx - inline effects
kxm 1xxx - sequencer, arpegiator (maybe soon..)


some specifications:

kxm 100: cv range 0..1
         gate range -1..1
         3 octaves
kxm 2xx: output range -0.25..0.25
kxm 4xx: output range 0..1
kxm 7xx: output range -0.5..0.5 (bipolar mode)
         output range 0..1 (unipolar mode)
                      0..-1 (v1.1)

*** use to play via midi "kxm 120"! ***

the following notes depends on older versions:

important note: the kx modular needs to play via midi keyboard the midi software
"hubi's midi transformator" (v0.8) with a special setup! this software is free.
you should search & download it from the internet. the setup file "kxm.trf" can 
be found in this release. 


let's start..

1. register the plugin'S you want by double leftclick on it
2. connect the modules the way you want
3. start "hubi's midi transformator", load the setup "kxm.trf", check the midi in/out port panel!
   input should be a mpu-401, output must be kx control sbxx
4. go to kx automation panel and choose "kxm 100", assign to "note" the midi event "note key"
   on channel 1, then assign to "gate" the midi event "noteon (type 2)" on channel 2, key c-1(0)
   it's very important that these assigns are correct! and yes, in this release the kxm
   uses 2 midichannels to play. later version will have an inbuilt midi interface.
5. play the kxm with your keyboard on channel 1
6. if you don't hear anything check module connections & setup again
7. (you can load a preset "kxm_brass.kx" in the dsp on a audigy (sb0090))

have fun!


this is a prerelease documentation. for more information go to http://www.driverheaven.net/effects-dsp/ 
[http://www.driverheaven.net/effects-dsp/153039-synthesizer-kx-modular-system-1-a.html]
or http://stylus.siteboard.de.

date. 04/23/2008


stylus
