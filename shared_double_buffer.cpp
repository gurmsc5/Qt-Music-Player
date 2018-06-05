#include "shared_double_buffer.hpp"

// Implement your shared double buffer here
template<typename T>
DoubleBuffer<T>::DoubleBuffer()
{
	readbuffer = new std::list<T>;
	writebuffer = new std::list<T>;
}

template<typename T>
DoubleBuffer<T>::~DoubleBuffer()
{
	delete readbuffer;
	delete writebuffer;
}


template<typename T>
void DoubleBuffer<T>::clear()
{
	std::unique_lock<std::mutex> lock(my_mutex);
	readbuffer->clear();
	writebuffer->clear();
	the_condition_variable.notify_all();
}

template<typename T>
size_t DoubleBuffer<T>::size()
{
	std::unique_lock<std::mutex> lock(my_mutex);
	return writebuffer->size();
}

template<typename T>
size_t DoubleBuffer<T>::readbuffer_size()
{
	std::unique_lock<std::mutex> lock(my_mutex);
	return readbuffer->size();
}

template<typename T>
void DoubleBuffer<T>::push(T & item)
{	
	std::unique_lock<std::mutex> lock(my_mutex);
	if (writebuffer->size() < MAXBUFFERSIZE) {
		writebuffer->push_back(item);
		
	}
	
	the_condition_variable.notify_all();
	while (writebuffer->size() == MAXBUFFERSIZE)
	{
		the_condition_variable.wait(lock);
	}
	
}

template<typename T>
void DoubleBuffer<T>::pop()
{
	if (!readbuffer->empty())
		readbuffer->pop_front();
}

template<typename T>
T DoubleBuffer<T>::peek()
{

	if (readbuffer->size() == 0)
		swap_buffer();
	//std::unique_lock<std::mutex> lock(my_mutex);
	sample = readbuffer->front();
	pop();
	return sample;
}

template<typename T>
void DoubleBuffer<T>::swap_buffer()
{
	std::unique_lock<std::mutex> lock(my_mutex);
	while (writebuffer->size() == 0)
	{
		the_condition_variable.wait(lock);
	}
	std::swap(readbuffer, writebuffer);
	the_condition_variable.notify_all();

}

template class DoubleBuffer<double>;