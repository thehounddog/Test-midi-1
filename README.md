# Test-midi-1
**MIDI controller for RC600**

This is a prototype for a MIDI controller to make adjustments to the current tempo of a BOSS RC600 looper. It was developed with VSCode / PlatformIO / Arduino. I am not a developer and this is my first C++ coding task. I'm sure the structure is all wrong - but it seems to work for now.

**Approach**
As far as I know, the RC600 doesn't show tempo information in it's midi message. It transmits MIDI Clock messages. This controller receives these messages and uses a timer to measure the period between the messages. The MIDI master (RC600) sends 24 clock messages per quarter note. (PPQN). I'm then using this period to calculate a BPM.

Once the controller has the BPM it can send a CC message back to the RC600 to adjust the BPM. I'm not aware of a straightforward message to do this. Hopefully there is one so I can change the method I am currently using with assigns.

I am using two assigns to cover the 40-300 range of tempo supported by the RC600. (actually I stop at 287bpm).

The first assign on CC4 covers 40- 167 BPM. The assign is set to tempo, velocity 0 (min), 127(max), and tempo 40(min) and 167(max).

So for example:
CC4, Velocity 0 is 40 BPM
CC4, Velocity 20 is 60 BPM
CC4, Velocity 107 is 147 BPM etc.

When the required tempo is above 160 BPM I switch to the second assign which has a different range (160 - 287).

**Hopefully someone can help with**
1) Improve my approach - it would be good not to use assigns. Is there a standard message to set Tempo that is supported by the RC600?
2) The BPM should be 24000/notetime but it was slughtly wrong (about 1%). My guess is that this is due to processing time required by the ESP32. I made adjustments to the 24000 number by trial and error until the BPM was correct. I ended up with 240110
3) improve my code - this is my first C++ project and I'm sure the code is a complete mess for anyone who knows what they are doing.

