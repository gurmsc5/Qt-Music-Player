#include "midiplayer.hpp"

//****************************************
//Get the midi file from the qlineEdit
//****************************************
std::string MIDIPlayer::getmidiFile()
{
	return midi_input->text().toStdString();
}


//*****************************
//SLOTS SECTION
//*****************************
//*****************************
//Play button
//*****************************
void MIDIPlayer::pressPlay()
{
	if (audio->state() == QAudio::SuspendedState) {
		playing_audio = true;
		pause_button->setEnabled(true);
		play_button->setEnabled(false);
		message_q->push("Play");
		int set_volume = vol_slider->value();
		audio->setVolume(set_volume);
		audio->resume();

	}

	else if(playing_audio==false) {
		
		buffer.clear();
		audio->reset();
		message_q->push("Play");

		midi_browse_button->setEnabled(false);
		play_button->setEnabled(false);
		pause_button->setEnabled(true);
		stop_button->setEnabled(true);

		playing_audio = true;
		audio->setBufferSize(MAXBUFFERSIZE * 8);
		int volume_sliderval = vol_slider->value();

		qreal linearVolume = static_cast<double>(volume_sliderval) / 100.0;
		audio->setVolume(linearVolume);

		device = audio->start();
		handleNotify();
	}
}

//*****************************
//Pause button
//*****************************

void MIDIPlayer::pressPause()
{
	if (audio->state() == QAudio::ActiveState && playing_audio==true) {
		play_button->setEnabled(true);
		pause_button->setEnabled(false);
		playing_audio = false;
		
		audio->suspend();
	}
}

//*****************************
//Stop button
//*****************************

void MIDIPlayer::pressStop()
{
	if (playing_audio == true || QAudio::SuspendedState) {

		midi_browse_button->setEnabled(true);
		pause_button->setEnabled(false);
		play_button->setEnabled(true);
		stop_button->setEnabled(false);

		message_q->push("Stop");

		playing_audio = false;
		double_buffer->clear();
		audio->stop();
	}
}

//*****************************
//Mute button
//*****************************

void MIDIPlayer::pressMute()
{
	vol_slider->setValue(0);
	audio->setVolume(0.0);
}

//******************************************
//Browse for midi file input
//also instantiates a new audio QAudioOutput
//otherwise it gets buggy when playing multiple
//files consecutively
//*******************************************
void MIDIPlayer::browse_midi_clicked()
{
	if (playing_audio == false) {

		play_button->setEnabled(true);
		pause_button->setEnabled(false);
		stop_button->setEnabled(false);

		//audio->stop();
		audio->reset();
		audio->setBufferSize(MAXBUFFERSIZE * 8);
		QString midi_file = QFileDialog::getOpenFileName(this, tr("Open File"),
			QDir::currentPath(), "MIDI Files (*.mid)");
		midi_input->setText(midi_file);
		std::string temp = getmidiFile();
		_tracklist = readMIDIFromFile(temp);

		if (_tracklist.size()!=0)
			message_q->push(temp);
		else if (_tracklist.empty())
		{
			auto mbox = new QMessageBox;
			mbox->critical(this, "Error", "Improper MIDI file input. Please provide a proper MIDI file");
		}
	}
	else
		qDebug() << "Press stop button before loading in a new midi file";
}

void MIDIPlayer::browse_midi_enter()
{
	if (playing_audio == false) {

		play_button->setEnabled(true);
		pause_button->setEnabled(false);
		stop_button->setEnabled(false);

		audio->reset();
		audio->setBufferSize(MAXBUFFERSIZE * 8);

		std::string temp = getmidiFile();
		_tracklist = readMIDIFromFile(temp);
		if(_tracklist.size()>0)
			message_q->push(temp);
		else if(_tracklist.empty())
		{
			auto mbox = new QMessageBox;
			mbox->critical(this, "Error", "Improper MIDI file input. Please provide a proper MIDI file");
		}
	}
	else
		qDebug() << "Press stop button before loading in a new midi file";
}

void MIDIPlayer::setvolume(int value)
{
	//vol_slider->setValue(value);
	//int volume_sliderval = vol_slider->value();
	qreal linearVolume = static_cast<double>(value) / 100.0;
	audio->setVolume(linearVolume);
}

void MIDIPlayer::checkerror()
{
	if (audio->error() == QAudio::UnderrunError)
	{

		qDebug() << "Buffer underflow. Suspending and waiting for it to be filled up";
		audio->suspend();
		while (double_buffer->size() < (MAXBUFFERSIZE / 8) && double_buffer->readbuffer_size() > 1) {
			if (double_buffer->size() == 0) {
				break;
			}
		}
		audio->resume();
		
	}
	
}

//********************************************
//Handles the notify() signal which is sent out
//when25ms of audio data is processed
//this is where the samples are read from 
//our DoubleBuffer 
//************************************************
void MIDIPlayer::handleNotify()
{
	int bytesFree = audio->bytesFree();
	int bytesToWrite = bytesFree > static_cast<int>(double_buffer->size()) ? static_cast<int>(double_buffer->size()) : bytesFree;
	if (double_buffer->size() == 0)
	{
		bytesToWrite = static_cast<int>(double_buffer->readbuffer_size());
	}
	int samplesToWrite = bytesToWrite;
	bytesToWrite = 2 * samplesToWrite;
	buffer.reserve(bytesToWrite);

	for (int i = 0; i < samplesToWrite; i++)
	{
		sample = double_buffer->peek();
		normalize();
		char *byte1 = reinterpret_cast<char *>(&normalized_sample);
		char *byte2 = ++byte1;
		buffer.push_back(*byte1);
		buffer.push_back(*byte2);
		
	}
	if (bytesToWrite > 0)
	{
		device->write(buffer.data(), bytesToWrite);
	}
	buffer.clear();
}
//********************************
//END OF SLOTS SECTION
//**********************************

// Implement class MIDIPlayer here
//***************************************
//Return the MIDI input form layout with
//the browse button
//*****************************************
QFormLayout * MIDIPlayer::midi_input_form()
{
	midi_browse_button = new QPushButton("Browse");
	connect(midi_browse_button, SIGNAL(clicked()), this, SLOT(browse_midi_clicked()));
	midi_browse_button->setObjectName("browse_button");

	midi_input = new QLineEdit();
	midi_input->setFixedWidth(450);
	midi_input->setObjectName("inputpath");
	connect(midi_input, SIGNAL(returnPressed()), this, SLOT(browse_midi_enter()));

	auto midi_form = new QFormLayout();
	midi_form->addRow(midi_input, midi_browse_button);

	return midi_form;
}

//******************************************************
//Return the form layout of play,pause and stop button
//******************************************************
QFormLayout * MIDIPlayer::control_buttons()
{
	play_button = new QPushButton("Play");
	QObject::connect(play_button, SIGNAL(clicked()), this, SLOT(pressPlay()));
	
	pause_button = new QPushButton("Pause");
	QObject::connect(pause_button, SIGNAL(clicked()), this, SLOT(pressPause()));
	stop_button = new QPushButton("Stop");
	QObject::connect(stop_button, SIGNAL(clicked()), this, SLOT(pressStop()));

	play_button->setObjectName("play");
	pause_button->setObjectName("pause");
	stop_button->setObjectName("stop");

	play_button->setEnabled(false);
	pause_button->setEnabled(false);
	stop_button->setEnabled(false);

	auto control_button_form = new QFormLayout();
	auto hlay = new QHBoxLayout;
	hlay->addWidget(play_button);
	hlay->addWidget(pause_button);
	hlay->addWidget(stop_button);

	control_button_form->addRow(hlay);

	return control_button_form;
}


//*******************************************************
//Set horizontal layout for volume slider and mute button
//*******************************************************
QHBoxLayout * MIDIPlayer::vol_controls()
{
	vol_slider = new QSlider(Qt::Horizontal);
	auto mute_button = new QPushButton("Mute");
	QObject::connect(mute_button, SIGNAL(clicked()), this, SLOT(pressMute()));
	connect(vol_slider, SIGNAL(valueChanged(int)), this, SLOT(setvolume(int)));
	vol_slider->setObjectName("volume");
	mute_button->setObjectName("mute");

	vol_slider->setRange(1, 100);
	vol_slider->setSingleStep(1);
	vol_slider->setValue(50);
	auto vol_lay = new QFormLayout();
	auto vol_label = new QLabel("Volume:");
	vol_lay->addRow(vol_label, vol_slider);
	auto hlay = new QHBoxLayout;

	hlay->addLayout(vol_lay);
	hlay->addWidget(mute_button);
	return hlay;

}

//*****************************************
//Set the Audio Control layout vertically
//with play,pause,stop buttons on top and
//volume slider and mute on bottom
//*****************************************
QVBoxLayout * MIDIPlayer::Audio_controls_layout()
{
	auto audio_control_lay = new QVBoxLayout;
	auto control_button_form = control_buttons();
	auto vol_button_lay = vol_controls();
	audio_control_lay->addLayout(control_button_form);
	audio_control_lay->addLayout(vol_button_lay);
	return audio_control_lay;
}
//****************************************
//Normalize the sample
//****************************************
void MIDIPlayer::normalize()
{
	if (sample > std::numeric_limits<int16_t>::max())
		normalized_sample = std::numeric_limits<int16_t>::max();
	else if (sample < std::numeric_limits<int16_t>::min())
		normalized_sample = std::numeric_limits<int16_t>::min();
	else
		normalized_sample = int16_t(sample);
}

//*****************************************
//Setup QIOdevice
//*****************************************
void MIDIPlayer::setupdevice()
{
	QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
	auto rates = info.supportedSampleRates();
	sampleRate = rates.at(0);

	for (int i = 1; i < rates.size(); ++i) {
		if (rates.at(i) < sampleRate)
			sampleRate = rates.at(i);
	}
	format.setSampleRate(sampleRate);
	format.setChannelCount(1);
	format.setSampleSize(16);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::SignedInt);

	audio = new QAudioOutput(format);
	//audio->setParent(this);
	//audio->setObjectName("audioplayer");
	connect(audio, SIGNAL(notify()), this, SLOT(handleNotify()));
	connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(checkerror()));
	audio->setBufferSize(MAXBUFFERSIZE * 4);
	buffer_size = audio->bufferSize();
	audio->setNotifyInterval(25);

	message_q = new MessageQueue<std::string>;
	double_buffer = new DoubleBuffer<double>;
	message_q->push(std::to_string(sampleRate));
}


//*********************************************
//MIDIPlayer constructor: set the overall layout
//**********************************************
MIDIPlayer::MIDIPlayer()
{
	setupdevice();
	auto group1 = new QGroupBox("MIDI Input",this);
	auto group2 = new QGroupBox("Audio Controls",this);

	auto midi_params_lay = new QHBoxLayout;
	
	auto midi_form = midi_input_form();
	auto audio_lay = Audio_controls_layout();

	//set midi_param layout in a horiontalbox layout
	midi_params_lay->addLayout(midi_form);

	//add layouts in the appropriate groups
	group1->setLayout(midi_params_lay);
	group2->setLayout(audio_lay);

	auto vlay = new QVBoxLayout(this);
	vlay->addWidget(group1);
	vlay->addWidget(group2);

	this->setLayout(vlay);
	

	Synth = new Synthesizer;
	ThreadPool.emplace_back(std::thread(&Synthesizer::synthesize, Synth, double_buffer, message_q, true));
}

MIDIPlayer::~MIDIPlayer()
{
	double_buffer->clear();
	message_q->push("Done");
	for (auto &j : ThreadPool)
		j.join();
	delete Synth;
	delete message_q;
	delete double_buffer;
	delete audio;
	device = nullptr;
}

