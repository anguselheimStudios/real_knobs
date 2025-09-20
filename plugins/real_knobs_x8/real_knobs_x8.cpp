/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2024 Filipe Coelho <falktx@falktx.com>
 * 
 * 
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
 * 
 * Modified by Anguselheim Studios 2025
 */

// ????

#include "DistrhoPlugin.hpp"
#include <string>


START_NAMESPACE_DISTRHO




const uint8_t NUM_KNOBS = 8;

struct knob{
	uint8_t control_channel = 1;
	uint8_t control_number = 7;
	int16_t control_value = 0;
	uint8_t control_sensitivity = 1;

};

int8_t relative_to_signed(int8_t input_value) {
	int8_t output_value = (int8_t) input_value;
	if (output_value > 64){
		output_value = -(128 - input_value);
	}
	else if (output_value == 64){
		output_value = 0;
	}
	return output_value;
}

// -----------------------------------------------------------------------------------------------------------

class real_knobs : public Plugin {
public:
    real_knobs()
        : Plugin(4* NUM_KNOBS, 0, 0) 
			
			{
			}

protected:
   /* --------------------------------------------------------------------------------------------------------
    * Information */

    const char* getLabel() const override {
        return "real_knobs _x8";
    }

    const char* getDescription() const override {
        return "8 virtual Absolute CC knobs for use with realtive endless encoders.";
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
        
        uint32_t param = 0;
		uint32_t  active_knob = index; // odd name due to conflict with name of struct
		while (active_knob  >= NUM_KNOBS) {
			active_knob = active_knob - NUM_KNOBS;
			param = param + 1;
		}
        
        		knob temp_knob;
		temp_knob.control_channel = 1;
		temp_knob.control_number = 102 + index;
		temp_knob.control_value = 1;
		temp_knob.control_sensitivity = 1;
		params[active_knob] = temp_knob;
		
		
        switch(param) {
			case 0: 
				parameter.name = ("Channel" + String(active_knob));
	            parameter.hints |= kParameterIsInteger;
	            parameter.ranges.min = 1;
	            parameter.ranges.max = 16;
	            parameter.ranges.def = 1;
	            break;
			case 1: 
				parameter.name = ("CC_Number" + String(active_knob));
	            parameter.hints |= kParameterIsInteger;
	            parameter.ranges.min = 1;
	            parameter.ranges.max = 127;
	            parameter.ranges.def = 102 + active_knob;
	            break;
			case 2: 
				parameter.name = ("CC_Value" + String(active_knob));
	            parameter.hints |= kParameterIsInteger;
	            parameter.ranges.min = 0;
	            parameter.ranges.max = 127;
	            parameter.ranges.def = 0;
	            break;
			case 3: 
				parameter.name = ("Sensitivity" + String(active_knob));
				parameter.hints |= kParameterIsInteger;
	            parameter.ranges.min = 1;
	            parameter.ranges.max = 10;
	            parameter.ranges.def = 1;
	            break;
			}
	parameter.symbol = parameter.name;
	}
		
		
    float getParameterValue(uint32_t index) const override {
       	uint32_t param = 0;
		uint32_t  active_knob = index; // odd name due to conflict with name of struct
		while (active_knob  >= NUM_KNOBS) {
			active_knob = active_knob - NUM_KNOBS;
			param = param + 1;
		}
		switch (param){
			case 0:
				return params[active_knob].control_channel;
				break;
			case 1:
				return params[active_knob].control_number;
				break;
			case 2:
				return params[active_knob].control_value;
				break;
			case 3:
				return params[active_knob].control_sensitivity;
				break;
			}
			return -1;			
	}


     void setParameterValue(uint32_t index, float value) override {
		uint32_t param = 0;
		uint32_t  active_knob = index; // odd name due to conflict with name of struct
		while (active_knob  >= NUM_KNOBS) {
			active_knob = active_knob - NUM_KNOBS;
			param = param + 1;
		}
		switch (param){
			case 0:
				params[active_knob].control_channel = value;
				break;
			case 1:
				params[active_knob].control_number = value;
				break;
			case 2:
				params[active_knob].control_value = value;
				break;
			case 3:
				params[active_knob].control_sensitivity = value;
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
			
			uint8_t chan = midiEvents[i].data[0] & 0x0F;
			uint8_t cc_num = midiEvents[i].data[1] & 0x7f;
			
			for (uint8_t k = 0; k < NUM_KNOBS; k++){
				
				if (chan == (params[k].control_channel - 1)
				&& cc_num == (params[k].control_number)) {
					
					// Get incoming value
					uint8_t value_in = midiEvents[i].data[2] & 0x7f;
					struct MidiEvent event_out;
					
					// apply it to the cc value
					params[k].control_value = params[k].control_value + relative_to_signed(value_in);
					

					if (params[k].control_value > 127) {
						params[k].control_value = 127;
					}
					else if (params[k].control_value < 0) {
						params[k].control_value = 0;
					}

					
					// Pack it up and send it out
					event_out.frame = midiEvents[i].frame;
					event_out.size = 3;
					event_out.data[0] = 0xB0 | (params[k].control_channel - 1);
					event_out.data[1] = params[k].control_number;
					event_out.data[2] = params[k].control_value;
					
					writeMidiEvent(event_out);
					continue;
				}
				
				// pass it through if it didn't get filtered
	            writeMidiEvent(midiEvents[i]);
				
				

			}


		}
	
	}
	
	

    // -------------------------------------------------------------------------------------------------------

private:

	knob params[NUM_KNOBS];
	


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
