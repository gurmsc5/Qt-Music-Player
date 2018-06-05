/** \file Synthesizer Module
 */
#ifndef SYNTHESIZER_HPP
#define SYNTHESIZER_HPP

#include <iostream>
#include "track.hpp"
#include "midi.hpp"
#include "shared_double_buffer.hpp"
#include "message_queue.hpp"
#include "instrument.hpp"
#include <string>
/** \fn Synthesize a Track into a Signal.
 * Returned signal is empty on failure.
 * \param track the track to synthesize.
 */

class Synthesizer {
public:
	void synthesize(DoubleBuffer<double> *D_B, MessageQueue<std::string> *M_Q,bool visible); //Main state machine loop
	MessageQueue<std::string> *MQ;
	Synthesizer();
	~Synthesizer();
private:
	DoubleBuffer<double> *DB;
	double sampleRate = 0.0;
	bool _visible = false;
	TrackListType tracklist;
	DefaultInstrument *Instrument;
	Track _track; //holds all tracks
	double sample = 0.0;
	enum states { Play, Pause, Stop,CONTROL } State; //states dependent on the button pressed

	//returns the state depending on what is in the Message Queue
	states enum_to_string(const std::string & s, MessageQueue<std::string> *MQ,states &state)
	{
		if (s == "Play") {
			MQ->wait_and_pop();
			return Play;
		}
		else if (s == "Stop") {
			MQ->wait_and_pop();
			return Stop;
		}
		else
			return CONTROL;
		
	}
};
#endif
