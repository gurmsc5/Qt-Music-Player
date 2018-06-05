#include "synthesizer.hpp"

// TODO
Synthesizer::Synthesizer():Instrument(nullptr)
{}

Synthesizer::~Synthesizer()
{
	delete Instrument;
}

void Synthesizer::synthesize(DoubleBuffer<double> * D_B, MessageQueue<std::string>* M_Q, bool visible)
{
	DB = D_B;
	MQ = M_Q;
	int i = 0;
	_visible = visible;
	
	sampleRate = std::stoi(MQ->peek());
	MQ->wait_and_pop();
	
	states State = CONTROL;
	while (_visible)
	{
		//*******************************************
		//State machine dependent on the button
		//Control state determines which midi file to
		//read
		//*******************************************
		switch (State)
		{
			//***********************************************************
			//Message queue sends the midi input file to read tracks from
			//***********************************************************
		case CONTROL:

		{
			i = 0;
			std::cout << "Control State" << std::endl;
			std::string file_name = MQ->peek();
			if (file_name == "Done")
			{
				_visible = false;
				MQ->wait_and_pop();
				break;
			}

			else if (file_name == "Stop")
			{
				State = enum_to_string(file_name, MQ, State);
			}
			else if (file_name == "Play") {
					_track = tracklist.front();
					Instrument = new DefaultInstrument(_track);
					State = enum_to_string(file_name, MQ, State);

			}
			else {
				tracklist = readMIDIFromFile(file_name);	
				MQ->wait_and_pop();
				_track = tracklist.front();
				Instrument = new DefaultInstrument(_track);
				State = enum_to_string(MQ->peek(), MQ, State);
			}


		};
		break;
		//**************************************************************
		//Play the track: can be aborted if some other button is pressed
		// or instrument is done playing
		//***************************************************************
		case Play:
		{
			std::cout << "Play State" << std::endl;
			while ((MQ->size() == 0) && (Instrument->halted() == false))
			{
				sample = Instrument->sample(static_cast<double>(i) / sampleRate);
				DB->push(sample);
				i++;
			}
			std::cout << "Finished pushing the track samples" << std::endl;
			while (MQ->size() == 0)
			{
				double s = 0;
				DB->push(s);
			}
			if (MQ->size() > 0) {
				State = enum_to_string(MQ->peek(), MQ, State);
			}
		}
			break;

			//**************************************************************
			//Stop the track: can be aborted if some other button is pressed
			//***************************************************************
		case Stop:
		{
			std::cout << "Stop state" << std::endl;
			i = 0;	
			
			State = CONTROL;
			if (Instrument->halted() == true)
				std::cout << "Done playing track." << std::endl;
		}
			break;

		}
	}
	std::cout << "QT app closed" << std::endl;
}




