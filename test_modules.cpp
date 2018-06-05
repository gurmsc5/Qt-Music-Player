#include "test_modules.hpp"



void Test_Modules::popmessages(MessageQueue<std::string>* MQ2)
{
	while (MQ2->peek() != "End")
	{
		MQ2->wait_and_pop();
	}
	MQ2->wait_and_pop();
	MQ2->push("Success");
}

void Test_Modules::pushvalues(MessageQueue<std::string> * MQ, MessageQueue<std::string> * MQ3, DoubleBuffer<double> * DB)
{
	while (MQ->size()==0)
	{
		double s = 100.0;
		DB->push(s);
	}
	MQ3->push("Success");
}
/*
void Test_Modules::readbuffer(MessageQueue<std::string> * MQ, DoubleBuffer<double> * DB)
{
	while (MQ->size()==0)
	{
		double s = DB->peek();
	}
}*/
