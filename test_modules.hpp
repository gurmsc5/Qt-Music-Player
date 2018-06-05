#ifndef TEST_MODULES_HPP
#define TEST_MODULES_HPP

#include "message_queue.hpp"
#include "shared_double_buffer.hpp"
class Test_Modules {
private:

public:
	void popmessages(MessageQueue<std::string> *MQ2);
	void pushvalues(MessageQueue<std::string> * MQ, MessageQueue<std::string> * MQ3, DoubleBuffer<double> * DB); //push the sample
	//void readbuffer(MessageQueue<std::string> * MQ, DoubleBuffer<double> * DB); //read from double
};



#endif