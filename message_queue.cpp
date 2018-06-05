// Implement your message queue here
#include "message_queue.hpp"

template<typename T>
bool MessageQueue<T>::empty() const {
	std::unique_lock<std::mutex> lock(the_mutex);
	return the_queue.empty();
}
template<typename T>
size_t MessageQueue<T>::size() const
{
	std::unique_lock<std::mutex> lock(the_mutex);
	return the_queue.size();
}

template<typename T>
T & MessageQueue<T>::peek()
{
	std::unique_lock<std::mutex> lock(the_mutex);
	while (the_queue.empty()) {
		my_condition_variable.wait(lock);
	}
	return the_queue.back();
}

template<typename T>
void MessageQueue<T>::push(const T & value)
{
	std::unique_lock<std::mutex> lock(the_mutex);
	the_queue.push(value);
	lock.unlock();
	my_condition_variable.notify_all ();
}

template<typename T>
void MessageQueue<T>::wait_and_pop()
{
	std::unique_lock<std::mutex> lock(the_mutex);
	while (the_queue.empty()) {
		my_condition_variable.wait(lock);
	}
	the_queue.pop();
}

template class MessageQueue<std::string>;