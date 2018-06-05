#include <QTest>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>

#include "midiplayer.hpp"
#include "test_config.hpp"
#include "test_modules.hpp"
class TestMIDIPlayer : public QObject{
  Q_OBJECT

private slots:

  void messageQueueTest();
  void doubleBufferTest();
  void doubleBufferTest_threaded();

  void initTestCase();
  //****************
  //test0.mid case
  //***************
  void get_midi_file_test0();
  void play_test0();
  void pause_test0();
  void play_after_pause_test0();
  void stop_test0();

  //****************
  //test1.mid case
  //***************
  void get_midi_file_test1();
  void play_test1();
  void stop_test1();

  //****************
  //test2.mid case
  //***************
  void get_midi_file_test2();
  void play_test2();
  void stop_test2();

  //****************
  //test3.mid case
  //***************
  void get_midi_file_test3();
  void play_test3();
  void pause_test3();
  void play_after_pause_test3();
  void stop_test3();

  //****************
  //test4.mid case
  //***************
  void get_midi_file_test4();
  void play_test4();
  void pause_test4();
  void play_after_pause_test4();
  void stop_test4();

  //****************
  //test5.mid case
  //***************
  void get_midi_file_test5();
  void play_test5();
  void stop_test5();

  //****************
  //test6.mid case
  //***************
  void get_midi_file_test6();
  void play_test6();
  void pause_test6();
  void play_after_pause_test6();
  void stop_test6();
  
private:
  MIDIPlayer widget;
  std::vector<std::thread> threadpool;
  MessageQueue<std::string> * MQ,*MQ3;
  MessageQueue<std::string> *MQ2;
  DoubleBuffer<double> * DB;
  std::vector<double> test_buffer;
  Test_Modules *TM;
};

void TestMIDIPlayer::messageQueueTest()
{
	widget.show();
	MQ = new MessageQueue<std::string>;
	MQ2 = new MessageQueue<std::string>;
	MQ3 = new MessageQueue<std::string>;
	DB = new DoubleBuffer<double>;
	MQ->push("Test");
	QVERIFY2(MQ->peek() == "Test", "Message didn't go through properly in the message queue");
	MQ->wait_and_pop();
	QVERIFY2(MQ->size() == 0, "Message queue is not empty. Pop method didn't work");
}

void TestMIDIPlayer::doubleBufferTest()
{
	for (auto i = 0; i < MAXBUFFERSIZE-2; i++)
	{
		double s = 200.0;
		DB->push(s);
	}
	QVERIFY2(DB->size() == MAXBUFFERSIZE - 2, "Double buffer didn't get filled up properly");
	DB->peek();
	QVERIFY2(DB->size() == 0, "Write buffer wasn't swapped");
	QVERIFY2(DB->readbuffer_size()!=0, "Read buffer wasn't swapped");
	DB->clear();
	QVERIFY2(DB->size() == 0, "Write buffer wasn't cleared");
	QVERIFY2(DB->readbuffer_size() == 0, "Read buffer wasn't cleared");
}

void TestMIDIPlayer::doubleBufferTest_threaded()
{
	TM = new Test_Modules;
	//create the threads to test double buffer and message queue
	threadpool.emplace_back(std::thread(&Test_Modules::pushvalues, TM,MQ,MQ3,DB));
	//threadpool.emplace_back(std::thread(&Test_Modules::readbuffer, TM,MQ,DB));
	threadpool.emplace_back(std::thread(&Test_Modules::popmessages, TM, MQ2));
	
	//push arbitrary messages in the queue
	MQ2->push("Test");
	MQ2->push("Test2");
	for (auto i = 0; i < MAXBUFFERSIZE * 8; i++)
	{
		 test_buffer.push_back(DB->peek());
	}
	QTest::qWait(1000);
	QVERIFY2(MQ2->size() == 0, "Message queue #2 is not empty. Popmessages not working");

	//send quit messages in the queues
	MQ2->push("End");
	MQ->push("End");
	DB->clear(); //clear double buffer

	//join threads
	for (auto &t : threadpool)
		t.join();
	MQ->wait_and_pop(); //clear out message queue #1
	QVERIFY2(test_buffer.size() == (MAXBUFFERSIZE * 8), "The test buffer wasn't filled up properly by shared double buffer. Check pushvalues thread.");
	QVERIFY2(MQ->size() == 0, "Message wasn't popped properly in message queue #1");
	QVERIFY2(MQ3->peek() == "Success", "pushvalues thread didn't send back the success message");
	MQ3->wait_and_pop();
	QVERIFY2(MQ2->peek() == "Success", "Success message wasn't sent by popmessages thread before joining");
	MQ2->wait_and_pop(); //clear out message queue #2
	delete TM;
	delete MQ;
	delete DB;
	delete MQ2;
	delete MQ3;
}

void TestMIDIPlayer::initTestCase(){
	
  {
    auto w = widget.findChild<QLineEdit *>("inputpath");
    QVERIFY2(w, "Could not find a QLineEdit widget");
  }

  {
    auto w = widget.findChild<QPushButton *>("play");
    QVERIFY2(w, "Could not find play button");
  }

  {
    auto w = widget.findChild<QPushButton *>("pause");
    QVERIFY2(w, "Could not find pause button");
  }
  
  {
    auto w = widget.findChild<QPushButton *>("stop");
    QVERIFY2(w, "Could not find stop button");
  }

  {
    auto w = widget.findChild<QSlider *>("volume");
    QVERIFY2(w, "Could not find volume slider");
  }
  
  {
    auto w = widget.findChild<QPushButton *>("mute");
    QVERIFY2(w, "Could not find mute button");
  }
}

void TestMIDIPlayer::get_midi_file_test0()
{
	widget.show();
	auto m = widget.findChild<QLineEdit *>("inputpath");
	QVERIFY2(m, "Could not find inputpath QlineEdit");

	QString in_file = QString::fromStdString(TEST_FILE_DIR + "/test0.mid");
	m->setText(in_file);
	QTest::keyPress(m, Qt::Key_Enter);
}



//*****************************************
//test sending "Play" in the message queue
//*****************************************
void TestMIDIPlayer::play_test0()
{
	auto m = widget.findChild<QPushButton *>("browse_button");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	
	
	
	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(play_button, Qt::LeftButton);
	QTest::qWait(1000);

}

void TestMIDIPlayer::pause_test0()
{
	auto pause_button = widget.findChild<QPushButton *>("pause");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	

	
	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(pause_button, Qt::LeftButton);
	QTest::qWait(1000);	
}

void TestMIDIPlayer::play_after_pause_test0()
{

	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(play_button, Qt::LeftButton);
	QTest::qWait(2000);

}
void TestMIDIPlayer::stop_test0()
{
	auto m = widget.findChild<QLineEdit *>("inputpath");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(stop_button, Qt::LeftButton);
	QTest::qWait(1000);
	
}

void TestMIDIPlayer::get_midi_file_test1()
{
	auto m = widget.findChild<QLineEdit *>("inputpath");
	QVERIFY2(m, "Could not find inputpath QlineEdit");

	QString in_file = QString::fromStdString(TEST_FILE_DIR + "/test1.mid");
	m->setText(in_file);
	QTest::keyPress(m, Qt::Key_Enter);
}

void TestMIDIPlayer::play_test1()
{
	auto m = widget.findChild<QPushButton *>("browse_button");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	
	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(play_button, Qt::LeftButton);
	QTest::qWait(1000);
}

void TestMIDIPlayer::stop_test1()
{
	auto m = widget.findChild<QLineEdit *>("inputpath");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(stop_button, Qt::LeftButton);
	QTest::qWait(1000);
}

void TestMIDIPlayer::get_midi_file_test2()
{
	auto m = widget.findChild<QLineEdit *>("inputpath");
	QVERIFY2(m, "Could not find inputpath QlineEdit");

	QString in_file = QString::fromStdString(TEST_FILE_DIR + "/test2.mid");
	m->setText(in_file);
	QTest::keyPress(m, Qt::Key_Enter);
}

void TestMIDIPlayer::play_test2()
{
	auto m = widget.findChild<QPushButton *>("browse_button");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	
	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(play_button, Qt::LeftButton);
	QTest::qWait(1000);
}

void TestMIDIPlayer::stop_test2()
{
	auto m = widget.findChild<QLineEdit *>("inputpath");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");
	QTest::mouseClick(stop_button, Qt::LeftButton);
	QTest::qWait(1000);
}

void TestMIDIPlayer::get_midi_file_test3()
{
	auto m = widget.findChild<QLineEdit *>("inputpath");
	QVERIFY2(m, "Could not find inputpath QlineEdit");

	QString in_file = QString::fromStdString(TEST_FILE_DIR + "/test3.mid");
	m->setText(in_file);
	QTest::keyPress(m, Qt::Key_Enter);
}

void TestMIDIPlayer::play_test3()
{
	auto m = widget.findChild<QPushButton *>("browse_button");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	
	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(play_button, Qt::LeftButton);
	QTest::qWait(1200);
}

void TestMIDIPlayer::pause_test3()
{
	auto pause_button = widget.findChild<QPushButton *>("pause");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	

	
	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(pause_button, Qt::LeftButton);
	QTest::qWait(1000);
}
void TestMIDIPlayer::play_after_pause_test3()
{
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(play_button, Qt::LeftButton);
	QTest::qWait(2000);
}

void TestMIDIPlayer::stop_test3()
{
	auto m = widget.findChild<QLineEdit *>("inputpath");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");
	QTest::mouseClick(stop_button, Qt::LeftButton);
	QTest::qWait(1000);
}

void TestMIDIPlayer::get_midi_file_test4()
{
	auto m = widget.findChild<QLineEdit *>("inputpath");
	QVERIFY2(m, "Could not find inputpath QlineEdit");

	QString in_file = QString::fromStdString(TEST_FILE_DIR + "/test4.mid");
	m->setText(in_file);
	QTest::keyPress(m, Qt::Key_Enter);
}

void TestMIDIPlayer::play_test4()
{
	auto m = widget.findChild<QPushButton *>("browse_button");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	
	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(play_button, Qt::LeftButton);
	QTest::qWait(1000);
}

void TestMIDIPlayer::pause_test4()
{
	auto pause_button = widget.findChild<QPushButton *>("pause");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	

	
	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(pause_button, Qt::LeftButton);
	QTest::qWait(1000);
}

void TestMIDIPlayer::play_after_pause_test4()
{
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(play_button, Qt::LeftButton);
	QTest::qWait(2000);

}

void TestMIDIPlayer::stop_test4()
{
	auto m = widget.findChild<QLineEdit *>("inputpath");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");
	QTest::mouseClick(stop_button, Qt::LeftButton);
	QTest::qWait(1000);

}
void TestMIDIPlayer::get_midi_file_test5()
{
	auto m = widget.findChild<QLineEdit *>("inputpath");
	QVERIFY2(m, "Could not find inputpath QlineEdit");

	QString in_file = QString::fromStdString(TEST_FILE_DIR + "/test5.mid");
	m->setText(in_file);
	QTest::keyPress(m, Qt::Key_Enter);
}

void TestMIDIPlayer::play_test5()
{
	auto m = widget.findChild<QPushButton *>("browse_button");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	
	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(play_button, Qt::LeftButton);
	QTest::qWait(1000);
}

void TestMIDIPlayer::stop_test5()
{
	auto m = widget.findChild<QLineEdit *>("inputpath");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");
	QTest::mouseClick(stop_button, Qt::LeftButton);
	QTest::qWait(1000);
}

void TestMIDIPlayer::get_midi_file_test6()
{
	auto m = widget.findChild<QLineEdit *>("inputpath");
	QVERIFY2(m, "Could not find inputpath QlineEdit");

	QString in_file = QString::fromStdString(TEST_FILE_DIR + "/test6.mid");
	m->setText(in_file);
	QTest::keyPress(m, Qt::Key_Enter);
}

void TestMIDIPlayer::play_test6()
{
	auto m = widget.findChild<QPushButton *>("browse_button");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	
	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(play_button, Qt::LeftButton);
	QTest::qWait(1500);
}

void TestMIDIPlayer::pause_test6()
{
	auto pause_button = widget.findChild<QPushButton *>("pause");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	

	
	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(pause_button, Qt::LeftButton);
	QTest::qWait(1000);
}

void TestMIDIPlayer::play_after_pause_test6()
{
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");

	QTest::mouseClick(play_button, Qt::LeftButton);
	QTest::qWait(2300);
}

void TestMIDIPlayer::stop_test6()
{
	auto m = widget.findChild<QLineEdit *>("inputpath");
	auto play_button = widget.findChild<QPushButton *>("play");
	auto stop_button = widget.findChild<QPushButton *>("stop");
	auto pause_button = widget.findChild<QPushButton *>("pause");
	

	QVERIFY2(play_button, "Could not find play button");
	QVERIFY2(pause_button, "Could not find pause button");
	QVERIFY2(stop_button, "Could not find stop button");
	QTest::mouseClick(stop_button, Qt::LeftButton);
	QTest::qWait(1000);
}
QTEST_MAIN(TestMIDIPlayer)

#include "test_midiplayer.moc"