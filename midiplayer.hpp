#ifndef MIDIPLAYER_HPP
#define MIDIPLAYER_HPP

#include "message_queue.hpp"
#include "shared_double_buffer.hpp"
#include "synthesizer.hpp"
#include "midi.hpp"

#include <thread>
#include <QMessageBox>
#include <QAudioOutput>
#include <QDebug>
#include <QFileDialog>
#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QLabel>
// define class MIDIPlayer here
class MIDIPlayer : public QWidget {
	Q_OBJECT

public:
	MIDIPlayer();
	~MIDIPlayer();

	//Synthesizer pointer
	Synthesizer * Synth =nullptr;

private:
	//Tracklist that will be obtained for midireader
	TrackListType _tracklist;

	//Message Queue pointer
	MessageQueue<std::string> *message_q =nullptr;
	//DoubleBuffer pointer
	DoubleBuffer<double> *double_buffer =nullptr;

	//bool for checking if playing;
	bool playing_audio = false;

	//thread pool
	std::vector<std::thread> ThreadPool;

	int sampleRate = 0;
	//qaudiodevice variables
	double sample = 0.0;
	int16_t normalized_sample = 0;
	std::size_t buffer_size =0; //buffer that holds samples that are played
	std::vector<char> buffer;

	QAudioOutput * audio;
	QIODevice *device;
	QAudioFormat format; //audio format

	//get the midi file path in string format
	std::string getmidiFile();

	//midi input
	QLineEdit * midi_input;
	//midi input form layout function
	QFormLayout * midi_input_form();

	//browse button
	QPushButton * midi_browse_button;
	//play,press,pause buttons;
	QPushButton * play_button;
	QPushButton * pause_button;
	QPushButton * stop_button;

	//volume slider
	QSlider * vol_slider;
	//play,pause,stop button form layout function
	QFormLayout * control_buttons();
	//set volume slider and mute button layout
	QHBoxLayout * vol_controls();
	//set the overall Audio Controls layout
	QVBoxLayout * Audio_controls_layout();
	void normalize(); //normalized sample from double to int16_t data type
	void setupdevice(); //setup the format for QAudioFormat in order to setup the audio device
	

protected slots:
	//slots for the buttons
	void pressPlay();
	void pressPause();
	void pressStop();
	void pressMute();
	//slot for the browse input file button
	void browse_midi_clicked();
	void browse_midi_enter();
	void setvolume(int value);
private slots:
	//slot for handling the notify() signal sent by qaudiooutput
	void handleNotify();
	void checkerror();

};
#endif