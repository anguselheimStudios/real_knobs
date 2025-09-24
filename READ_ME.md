real_knobs is MIDI plugin that converts Relative Encoders, such as the ones found on the AKAI MPKmini MkIII, and applies the adjustment to a virtual "knob". It then sends out the new value on the same MIDI Channel and CC number. It is made in C++, with the DISTRHO Plugin Framework (DPF). It is designed to be built as an LV2 Plugin, but should be able to be built as an  or VST2 or 3, or standalone Jack plugin. This can be accomplished by uncommenting the appropriate lines in real_knobs/plugins/real_knobs_x8/makefile :

```
# --------------------------------------------------------------
# Enable plugin types 
# Uncomment the corresponding line to create the desired plugin type. 

#TARGETS += jack
TARGETS += lv2_dsp
#TARGETS += vst2
#TARGETS += vst3

all: $(TARGETS)

# --------------------------------------------------------------
```

## Building

#### Linux (and maybe Mac)
Using your terminal emulator of choice, navigate to the real_knobs/ directory. Run the command `make`. The binary should appear in the real_knobs/bin/ directory.

#### Windows (and Mac, if the above does not work)
I dunno.

## Installing

#### Linux/Mac
Copy the file to the directory where the plugins of that type are found. On Debian 12 that's /usr/lib/lv2, so you would:

```
sudo cp -r ./bin/real_knobs_x8.lv2 /usr/lib/lv2/real_knobs_x8.lv2
```

LV2 Plugins are directories, so don't forget the "-r".

If you're not on Debian, check to see where your distro keeps these files first. Stuff like "~/.vst3/" is also common.
#### Windows
You're on your own for now.

