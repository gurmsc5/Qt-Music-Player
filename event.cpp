#include "event.hpp"

// TODO
//***********************************************
//Default TempoEvent constructor
//***********************************************
TempoEvent::TempoEvent(intmax_t t):tempo_t(t)
{
}

//***********************************************
//returns the tempo
//***********************************************
intmax_t TempoEvent::getTempo() const
{
	return tempo_t;
}

//***********************************************
//Default NoteEvent constructor
//***********************************************
NoteEvent::NoteEvent(bool on, int8_t notenum, int8_t notevelocity):_on_off(on),_note_num(notenum),_note_velocity(notevelocity)
{
}

//***********************************************
//Returns bool if note is ON/OFF
//***********************************************
bool NoteEvent::getNoteOn()
{
	if (_on_off)
		return true;
	else
		return false;
}

//***********************************************
//Returns the note number 0-127
//***********************************************
int8_t NoteEvent::getNoteNumber() const
{
	return _note_num;
}

//***********************************************
//Returns note velocity
//***********************************************
int8_t NoteEvent::getNoteVelocity() const
{
	return _note_velocity;
}

//***********************************************
//Default MIDIEvent of TempoEvent type constructor
//***********************************************
MIDIEvent::MIDIEvent(TempoEvent e, intmax_t ticktime):_ticktime(ticktime)
{
	_EventType = _TempoEvent;
	event._tempoEvent = e;
}

//***********************************************
//Default MIDIEvent of EndTrackEvent type constructor
//***********************************************
MIDIEvent::MIDIEvent(EndTrackEvent e, intmax_t ticktime):_ticktime(ticktime)
{
	_EventType = _EndTrackEvent;
	event._EndTrackEvent = e;
}

//***********************************************
//Default MIDIEvent of NoteEvent type constructor
//***********************************************
MIDIEvent::MIDIEvent(NoteEvent e, intmax_t ticktime):_ticktime(ticktime)
{
	_EventType = _NoteEvent;
	event._noteEvent = e;
}
//***********************************************
//returns true if MIDIEvent is TempoEvent
//***********************************************
bool MIDIEvent::isTempoEvent() const
{
	if (_EventType == _TempoEvent)
		return true;
	else
		return false;
}

//***********************************************
//returns true if MIDIEvent is EndTrackEvent
//***********************************************
bool MIDIEvent::isEndTrackEvent() const
{
	if (_EventType == _EndTrackEvent)
		return true;
	else
		return false;
}

//***********************************************
//returns true if MIDIEvent is NoteEvent
//***********************************************
bool MIDIEvent::isNoteEvent() const
{
	if (_EventType == _NoteEvent)
		return true;
	else
		return false;
}

//***********************************************
//returns ticktime for MIDIEvent
//***********************************************
intmax_t MIDIEvent::getTickTime() const
{
	return _ticktime;
}

//***********************************************
//returns TempoEvent if MIDIEvent is of that type
//***********************************************
TempoEvent MIDIEvent::asTempoEvent() const
{
	if (isTempoEvent())
	{
		return event._tempoEvent;
	}
	else
	{}
}

//***********************************************
//returns NoteEvent if MIDIEvent is of that type
//***********************************************
NoteEvent MIDIEvent::asNoteEvent() const
{
	if (isNoteEvent())
	{
		return event._noteEvent;
	}
	else {}
}

