/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2024 Filipe Coelho <falktx@falktx.com>
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



#include "DistrhoPlugin.hpp"
#include <string>


START_NAMESPACE_DISTRHO


// Set the number of knobs here
const uint8_t NUM_KNOBS = 8;



// -----------------------------------------------------------------------------------------------------------

class real_knobs_x8 : public Plugin {
public:
    real_knobs_x8()
        : Plugin(3 * NUM_KNOBS, 0, 0) {
			
			}

protected:
   /* --------------------------------------------------------------------------------------------------------
    * Information */

    const char* getLabel() const override {
        return "real_knobs_x8";
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
    

	
	//This is the data structure for each knob
	struct knob{
		uint8_t control_channel = 1;
		uint8_t control_number = 7;
		int16_t control_value = 0;
	};
	
	// standard index for param related swithes 
	enum knob_params {
		control_channel,
		control_number,
		control_value
	};
	
	/* knob_param_pair is used as a return value for 
	 * index_to_knob_param_pair. 
	 * 
	 * knob_num is the index of the knob in params[].
	 * param_num is the index of the parameter in the knob struct. 
	 */
	struct knob_param_pair {
		uint8_t knob_num;
		uint8_t param_num;
	};
	
	/* This function converts the Akai MPK mini mkIII's relative 
	 * encoder's output to a signed 8 bit intiger showing the ammount 
	 * of change.
	 */
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
	
	/* This function converts DPF's paramater index to a knob_param_pair 
	 * struct giving you the index of the knob in params[] and the index
	 * of the paramater in the switch statements associated 
	 * knobs struct.
	 */
	knob_param_pair index_to_knob_param_pair(uint32_t index, const uint8_t num_knobs = NUM_KNOBS) const {
		knob_param_pair result;
		result.knob_num = index;
		result.param_num = 0;
		while (result.knob_num  >= num_knobs) {
			result.knob_num = result.knob_num - num_knobs;
			result.param_num++;
		}
		return result;
	}
	
	// initialize the parameters
	void initParameter(uint32_t index, Parameter& parameter) override {
       
        parameter.hints = kParameterIsAutomatable;

		knob_param_pair indices = index_to_knob_param_pair(index);
        
        knob temp_knob;
		temp_knob.control_channel = 1;
		temp_knob.control_number = 102 + indices.knob_num;
		temp_knob.control_value = 1;

		params[indices.knob_num] = temp_knob;
		
        switch(indices.param_num) {
			case control_channel: 
				parameter.name = ("Channel_" + String(indices.knob_num));
	            parameter.hints |= kParameterIsInteger;
	            parameter.ranges.min = 1;
	            parameter.ranges.max = 16;
	            parameter.ranges.def = 1;
	            break;
			case control_number: 
				parameter.name = ("CC_Number_" + String(indices.knob_num));
	            parameter.hints |= kParameterIsInteger;
	            parameter.ranges.min = 1;
	            parameter.ranges.max = 127;
	            parameter.ranges.def = 102 + indices.knob_num;
	            break;
			case control_value: 
				parameter.name = ("CC_Value_" + String(indices.knob_num));
	            parameter.hints |= kParameterIsInteger;
	            parameter.ranges.min = 0;
	            parameter.ranges.max = 127;
	            parameter.ranges.def = 0;
	            break;

		}
			
	parameter.symbol = parameter.name;
	}
		
	//Parameter Getter
    float getParameterValue(uint32_t index) const override {
		
		knob_param_pair indices = index_to_knob_param_pair(index);

		switch (indices.param_num){
			case control_channel:
				return params[indices.knob_num].control_channel;
				break;
			case control_number:
				return params[indices.knob_num].control_number;
				break;
			case control_value:
				return params[indices.knob_num].control_value;
				break;
			}
			return -1;			
	}

	//Parameter Getter
    void setParameterValue(uint32_t index, float value) override {

		knob_param_pair indices = index_to_knob_param_pair(index);
		
		switch (indices.param_num){
			case control_channel:
				params[indices.knob_num].control_channel = value;
				break;
			case control_number:
				params[indices.knob_num].control_number = value;
				break;
			case control_value:
				params[indices.knob_num].control_value = value;
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
			bool unused = true;
			for (uint8_t k = 0; k < NUM_KNOBS; k++){
				
				if (chan == (params[k].control_channel - 1)
				&& cc_num == (params[k].control_number)) {
					
					unused = false;
					// Get incoming value
					uint8_t value_in = midiEvents[i].data[2] & 0x7f;

					
					// apply it to the cc value
					params[k].control_value = params[k].control_value + relative_to_signed(value_in);
					
					if (params[k].control_value > 127) {
						params[k].control_value = 127;
					}
					else if (params[k].control_value < 0) {
						params[k].control_value = 0;
					}
					
					// Pack it up and send it out
					struct MidiEvent event_out;
					event_out.frame = midiEvents[i].frame;
					event_out.size = 3;
					event_out.data[0] = 0xB0 | (params[k].control_channel - 1);
					event_out.data[1] = params[k].control_number;
					event_out.data[2] = params[k].control_value;
					
					writeMidiEvent(event_out);
					continue;
				}

			}

			// pass it through if it didn't get filtered
			if (unused){
				writeMidiEvent(midiEvents[i]);
			}
			
		}
	
	}
	
    // -------------------------------------------------------------------------------------------------------

private:
	// parameters 
	knob params[NUM_KNOBS];
	
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(real_knobs_x8)
};

/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

Plugin* createPlugin(){
    return new real_knobs_x8();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
