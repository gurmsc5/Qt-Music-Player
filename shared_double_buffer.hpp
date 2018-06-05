// Define your shared double buffer here
#ifndef SHARED_DOUBLE_BUFFER_HPP
#define SHARED_DOUBLE_BUFFER_HPP

#include <list>
#include <mutex>
#include <condition_variable>
#define MAXBUFFERSIZE 15000

template<typename T>
class DoubleBuffer {
private:
	std::list<T> * readbuffer;
	std::list<T> * writebuffer;
	std::condition_variable the_condition_variable;
	mutable std::mutex my_mutex;
	void pop(); //pops the items from readbuffer
	void swap_buffer(); //swap the buffers when readbuffer.size() = 0 and writebuffer.size()!=0
	T sample = 0; //the sample to be returned
public:
	DoubleBuffer();
	~DoubleBuffer();
	void clear();
	size_t size(); //gives writebuffer size
	size_t readbuffer_size(); //gives readbuffer size
	void push(T &item); //pushes items in writebuffer until buffer is full
	
	T peek(); //get the item at the top of the buffer
	
	
};

#endif