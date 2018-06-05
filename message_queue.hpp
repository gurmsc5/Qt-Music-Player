// Define your message queue here
#ifndef _MESSAGE_QUEUE_HPP
#define _MESSAGE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class MessageQueue {
public:
	
	bool empty() const;
	size_t size() const;
	T &peek();
	void push(const T& value);
	void wait_and_pop();
private:
	std::queue<T> the_queue;
	mutable std::mutex the_mutex;
	std::condition_variable my_condition_variable;
};
//#include "message_queue.cpp"
#endif // !MESSAGE_QUEUE_HPP