#include "instrument.hpp"
// TODO


DefaultInstrument::DefaultInstrument(const Track & track)
{
	_track = track;
	Midiclock = track.getTicksPerBeat();
	end = false;
	overall_time = 0.0;
	note_length = 0.0;
	push_on_notes();
}
//*******************************************************
//checks if endtrackevent has been processed
//*******************************************************
bool DefaultInstrument::halted()
{
	if (end == true)
		return true;
	else
		return false;
}
//*******************************************************
//Calculate sample
//********************************************************
double DefaultInstrument::sample(double deltaT)
{
	overall_time = deltaT;
	_sample = 0;
	end = false;
	notes_processed = false;
	while (notes_processed == false)
	{
		notes_processed = processed();
	}
	return _sample;
}


//*******************************************************
//Calculate note frequency: Had to include lookup table
//because some of the notes are off on the specs
//********************************************************
void DefaultInstrument::freq_convert(const int num)

{
	if (num == 65)
		ON_midi_note_info.note_frequency = 349.63;
	else if (num == 72)
		ON_midi_note_info.note_frequency = 523.26;
	else if (num == 73)
		ON_midi_note_info.note_frequency = 554.36;
	else if (num == 74)
		ON_midi_note_info.note_frequency = 587.32;
	else if (num == 75)
		ON_midi_note_info.note_frequency = 622.26;
	else if (num == 76)
		ON_midi_note_info.note_frequency = 659.26;
	else if (num == 77)
		ON_midi_note_info.note_frequency = 699.26;
	else if (num == 78)
		ON_midi_note_info.note_frequency = 739.988;
	else if (num == 79)
		ON_midi_note_info.note_frequency = 784.0;
	else if (num == 80)
		ON_midi_note_info.note_frequency = 830.61;
	else if (num == 81)
		ON_midi_note_info.note_frequency = 880.0;
	else if (num == 82)
		ON_midi_note_info.note_frequency = 932.328;
	else if (num == 83)
		ON_midi_note_info.note_frequency = 987.76;
	else if (num == 84)
		ON_midi_note_info.note_frequency = 1046.52;
	else {
		ON_midi_note_info.note_frequency = std::pow(2.0, (double(num) - 69.0) / 12.0) * 440.0; //note to frequency conversion formula (wikipedia midi frequency formula page)
	}
}

//*******************************************************
//Envelope function
//********************************************************

void DefaultInstrument::envelope(const double _time)
{
	if ((_time <= t_index.t1) && (_time >= t_index.t0))
		t_index.env = ((1.0 / t_index.t1)*_time);
	else if ((_time <= t_index.t2) && (_time > t_index.t1))
		t_index.env = 1.0 - (_time - t_index.t1);
	else if ((_time <= t_index.t3) && (_time > t_index.t2))
		t_index.env = t_index.S;
	else if ((_time <= t_index.t4) && (_time > t_index.t3))
		t_index.env = t_index.S - ((t_index.S / (t_index.t4 - t_index.t3))*(_time - t_index.t3));
	else
		t_index.env = 0;
}

//*******************************************************
//Calculate real time of note
//********************************************************
double DefaultInstrument::real_time(intmax_t tempo, intmax_t tick_time)
{
	double real = double(static_cast<double>((tick_time * tempo)) / double(Midiclock*std::pow(10, 6)));
	return real;
}

//*******************************************************
//Push on notes only in a container
//********************************************************
void DefaultInstrument::push_on_notes()
{
	for (auto it = _track.begin(); it != _track.end(); it++)
	{
		if (it->isTempoEvent())
		{
			TempoEvent t1 = it->asTempoEvent();
			tempo = t1.getTempo();
		}
		else if (it->isNoteEvent())
		{
			NoteEvent e = it->asNoteEvent();
			if (e.getNoteOn() == true)
			{
				ON_midi_note_info.on_time = real_time(tempo, it->getTickTime());
				freq_convert(e.getNoteNumber());
				ON_midi_note_info.note_velocity = double(e.getNoteVelocity());
				auto j = _track.begin();
				bool match = false;
				while (!match)
				{
					if (j->isNoteEvent() && j->getTickTime()>=it->getTickTime())
					{
						NoteEvent e2 = j->asNoteEvent();
						if (e2.getNoteOn() == false && e2.getNoteNumber() == e.getNoteNumber())
						{
							ON_midi_note_info.off_time = real_time(tempo, j->getTickTime());
							ON_note_container.insert(std::make_pair<int8_t,on_mid_notes>(e.getNoteNumber(),
								on_mid_notes(ON_midi_note_info.off_time, ON_midi_note_info.on_time,
									ON_midi_note_info.note_frequency, ON_midi_note_info.note_velocity)));

							match = true;
						}
					}
					j++;
				}
			}


		}

		else if (it->isEndTrackEvent())
		{
			end_time = real_time(tempo, it->getTickTime());
		}
	}
}

//*******************************************************
//Calculate sample of notes if note_time <= overall_time
//********************************************************
bool DefaultInstrument::processed()
{
	for (auto it = ON_note_container.begin(); it != ON_note_container.end(); it++)
	{
		if (it->second.on_time <= overall_time)
		{
			note_length = overall_time - it->second.on_time;
			envelope(note_length);
			_sample += 200.0 * (it->second.note_velocity*t_index.env*sin(2.0*M_PI*(it->second.note_frequency)*note_length));
		}
	}
	if (overall_time >= end_time)
	{
		end = true;
	}
	return true;
}