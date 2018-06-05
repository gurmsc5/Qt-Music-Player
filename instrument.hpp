/** \file The DefaultInstrument Module
 */
#ifndef INSTRUMENT_HPP
#define INSTRUMENT_HPP
#define _USE_MATH_DEFINES
#include "event.hpp"
#include "track.hpp"
#include <cmath>
#include <unordered_map>
/** The default instrument. A simple fixed length note instrument using a
 * a sinusoid weighted by an envelope function.
 */
class DefaultInstrument{
public:

  /** No default constructor. A DefaultInstrument must have an associated 
   * track. 
   */
  DefaultInstrument() = delete;

  /** Cannot be copied. 
   */
  DefaultInstrument(const DefaultInstrument & ) = delete;

  /** Cannot be assigned.
   */
  DefaultInstrument & operator=(const DefaultInstrument & ) = delete;

  /** Construct a Default Instrument using the specified track.
   * \param track the track to read events from. 
   */
  DefaultInstrument(const Track & track);

  /** Returns true of the DefaultInstrument has stopped processing events.
   * This can occu e.g. if all events have been consumed or a EndTrackEvent
   * has been processed.
   */
  bool halted();

  /** Return the sample after the (real) time increment. Samples the 
   * instrument based on its state after processing all events prior 
   * to this time.
   * \param deltaT the time increment in seconds.
   */
  double sample(double deltaT);
  //MIDI time is represented by a tick integer
  // T = microseconds per beat (defined by tempo)
  // M = MIDI clock ticks per beat(defined in MIDI file)
  // T' = (T/10^6) seconds per beat
  // Hence seconds per MIDI clock tick = T' / M
  // process any new track events that become active since the last sample time
  // Sum the waveforms for the currently active notes
  // Must maintain a container of notes active at the current time
  //	- note number
  //	- note velocity
  //	- note duration
  //	- time turned on
  
  // Use a Track iterator to process all events up to the current sample
  //Probably want to define private helper functions to:
  //	-convert a note number to frequency
  //	-determine length of a note
  //	-evaluate the envelope function
  //	-sum the active notes
private:
	// TODO
	// container of active notes
	// for each active note: note number
	Track _track;
	struct on_mid_notes
	{
		double off_time = 0;
		double on_time = 0;
		double note_frequency = 0;
		double note_velocity = 0;
		on_mid_notes() {};
		on_mid_notes(double off_t, double on_t,
			double note_freq, double note_vel) :off_time(off_t),
			on_time(on_t), note_frequency(note_freq),
			note_velocity(note_vel) {};

	}ON_midi_note_info;
	std::unordered_map<int8_t,on_mid_notes> ON_note_container;
	
	intmax_t Midiclock;
	double _sample = 0;
	intmax_t tempo = 0;
	
	double note_length = 0.0;
	double overall_time = 0.0;
	double end_time = 0.0;

	struct time_index
	{
		double env = 0.0;
		double t4 = 0.25;
		double t0 = 0;
		double t1 = 0.25*t4;
		double t2 = 0.45*t4;
		double t3 = 0.80*t4;
		double S = 0.8;
	} t_index;

	bool end = false;

	//
	bool notes_processed = false;

	void freq_convert(const int num);
	void envelope(const double _time);
	double real_time(intmax_t tempo, intmax_t tick_time);
	void push_on_notes();

	bool processed();
};


#endif
