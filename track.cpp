#include "track.hpp"

// TODO

//***********************************************
//Default Track Constructor
//***********************************************
Track::Track(intmax_t ticks_per_beat):_ticks_per_beat_(ticks_per_beat)
{}

//***********************************************
//Return ticks per beat in a track
//***********************************************
intmax_t Track::getTicksPerBeat() const
{
	return _ticks_per_beat_;
}

//***********************************************
//Iterator to the beginning event in a Track
//***********************************************
std::list<MIDIEvent>::const_iterator Track::begin() const
{
	std::list<MIDIEvent>::const_iterator it;
	return(it = _eventlist.begin()); //itertor to the first event

}

//***********************************************
//Iterator to the last event in a Track
//***********************************************
std::list<MIDIEvent>::const_iterator Track::end() const
{
	std::list<MIDIEvent>::const_iterator it;
	return(it = _eventlist.end()); //iterator to a point after the last event
}

//***********************************************
//Add a EndTrackEvent in Track
//***********************************************
void Track::addEndEvent(intmax_t tick_time)
{
	EndTrackEvent e;
	MIDIEvent MM(e,tick_time);
	_eventlist.push_back(MM);
}

//***********************************************
//Add a TempoEvent in Track
//***********************************************
void Track::addTempoEvent(intmax_t tempo, intmax_t tick_time)
{
	TempoEvent e(tempo);
	MIDIEvent MM(e, tick_time);
	_eventlist.push_back(MM);
}

//***********************************************
//Add a NoteEvent in Track
//***********************************************
void Track::addNoteEvent(uint8_t code, uint8_t data1, uint8_t data2, intmax_t tick_time)
{
	if (code == 9)
		_on = true;
	else if (code == 8||data2==0)
		_on = false;
	else {}
	NoteEvent e(_on, data1, data2);
	MIDIEvent MM(e, tick_time);
	_eventlist.push_back(MM);
}
