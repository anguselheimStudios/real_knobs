/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2024 Filipe Coelho <falktx@falktx.com>
 * 
 * Modified by Anguselheim Studios 2025
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoPlugin.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

class real_knobs : public Plugin {
public:
    real_knobs()
        : Plugin(4, 0, 0), 
			control_channel(1),
			control_number(0x07),
			control_value(0),
			control_sensitivity(1.0f)			
			{
			}

protected:
   /* --------------------------------------------------------------------------------------------------------
    * Information */

    const char* getLabel() const override {
        return "real_knobs";
    }

    const char* getDescription() const override {
        return "Virtual Absolute CC knob for use with realtive endless encoders.";
    }

    const char* getMaker() const override {
        return "Anguselheim Studios";
    }

    const char* getHomePage() const override {
        return "TBD";
    }

    const char* getLicense() const override {
        return "ISC";
    }

    uint32_t getVersion() const override {
        return d_version(0, 1, 0);
    }

   /* --------------------------------------------------------------------------------------------------------
    * Init and Internal data */

	void initParameter(uint32_t index, Parameter& parameter) override {
        parameter.hints = kParameterIsAutomatable;
        
        switch(index) {
			case 0: 
				parameter.name = "Channel";
	            parameter.hints |= kParameterIsInteger;
	            parameter.ranges.min = 1;
	            parameter.ranges.max = 16;
	            parameter.ranges.def = 1;
	            break;
			case 1: 
				parameter.name = "CC_Number";
	            parameter.hints |= kParameterIsInteger;
	            parameter.ranges.min = 1;
	            parameter.ranges.max = 127;
	            parameter.ranges.def = 7;
	            break;
			case 2: 
				parameter.name = "CC_Value";
	            parameter.hints |= kParameterIsInteger;
	            parameter.ranges.min = 0;
	            parameter.ranges.max = 127;
	            parameter.ranges.def = 0;
	            break;
			case 3: 
				parameter.name = "Sensitivity";
	            parameter.ranges.min = 0.1f;
	            parameter.ranges.max = 5.0f;
	            parameter.ranges.def = 1.0f;
	            break;
			}
		parameter.symbol = parameter.name;
		}
		
		
    float getParameterValue(uint32_t index) const override {
        switch (index) {
        case 0:
            return control_channel;
        case 1:
            return control_number;
        case 2:
            return control_value;
        case 3:
            return control_sensitivity;
        }

        return 0.0f;
    }
     void setParameterValue(uint32_t index, float value) override {
        switch (index) {
        case 0:
            control_channel = value;
            break;
        case 1:
            control_number = value;
            break;
        case 2:
            control_value = value;
            break;
        case 3:
            control_sensitivity = value;
            break;
        }
    }

   /* --------------------------------------------------------------------------------------------------------
    * Audio/MIDI Processing */

    void run(const float**, float**, uint32_t,
             const MidiEvent* midiEvents, uint32_t midiEventCount) override {
        
    for (uint32_t i=0; i<midiEventCount; ++i) {
		// pass throuch anything that's not a CC untouched.
		if ((midiEvents[i].data[0] & 0xF0) != 0xB0) {
            writeMidiEvent(midiEvents[i]);
            continue;
        }
        // Same for anything on the wrong channel
        if ((midiEvents[i].data[0] & 0x0F) != (control_channel - 1)) {
            writeMidiEvent(midiEvents[i]);
            continue;
        }
        // and wrong CC number
        if ((midiEvents[i].data[1] & 0x7f) != control_number) {
            writeMidiEvent(midiEvents[i]);
            continue;
        }
        
        // Get incoming value
		uint8_t value_in = midiEvents[i].data[2] & 0x7f;
		uint8_t change_amount;
		struct MidiEvent event_out;
		
		// apply it to the cc value
		if(value_in < 64){
			change_amount = int(value_in * control_sensitivity + 1);
			control_value = control_value + change_amount;
			if (control_value > 127) {
				control_value = 127;
			}
		}
		else if(value_in > 64){
			change_amount = int((64-(value_in -63)) * control_sensitivity +1);
			control_value = control_value - change_amount;
			if (control_value < 0) {
				control_value = 0;
			}
		}
		
		// Pack it up and send it out
		event_out.frame = midiEvents[i].frame;
		event_out.size = 3;
		event_out.data[0] = 0xB0 | (control_channel - 1);
		event_out.data[1] = control_number;
		event_out.data[2] = control_value;
		
		writeMidiEvent(event_out);
    }
}

    // -------------------------------------------------------------------------------------------------------

private:

    uint8_t control_channel;
    int8_t control_number;
    int16_t control_value;
    float control_sensitivity; 

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(real_knobs)
};

/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

Plugin* createPlugin()
{
    return new real_knobs();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
