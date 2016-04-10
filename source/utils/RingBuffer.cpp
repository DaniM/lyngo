#include "utils/RingBuffer.h"

#include <cassert>
#include <stdlib.h>
#include <string.h>

RingBuffer::RingBuffer(int elementSizeInBytes
					   , int elementCount)
	: m_data(nullptr)
	, m_writePos(0)
	, m_readPos(0)
	, m_bigMask(0)
	, m_smallMask(0)
	, m_capacity(0)
	, m_elementSizeInBytes(0)
{
	// ensure element count is power of two
	assert(((elementCount - 1) & elementCount) == 0);

	// Allocate buffers to hold data
	m_data = reinterpret_cast<unsigned char*>(malloc(elementCount * elementSizeInBytes));
	assert(m_data);

	// inisialise internal data
	m_capacity = elementCount;

	Flush();

	m_bigMask = (elementCount * 2) - 1;
	m_smallMask = elementCount - 1;
	m_elementSizeInBytes = elementSizeInBytes;
}

//  --------------------------------------------------------------------------------
/*
Destructor for class
*/
RingBuffer::~RingBuffer()
{
	free(m_data);
	m_data = nullptr;
}

//  --------------------------------------------------------------------------------
/*
Writes data to the ring buffer
*/
int RingBuffer::Write
(
	const void* data,
	int elementCount
	)
{
	//VOXAUDIO_LOG_VERBOSE("elementCount %d", elementCount);
	assert(elementCount <= GetAvailableElementsForWriting());

	int size1 = 0;
	int size2 = 0;
	int numWritten = 0;
	void* data1 = nullptr;
	void* data2 = nullptr;
	numWritten = GetWriteRegions(elementCount, &data1, &size1, &data2, &size2);
	if (size2 > 0)
	{

		memcpy(data1, data, size1* m_elementSizeInBytes);
		data = (reinterpret_cast<const unsigned char*>(data)) + size1 * m_elementSizeInBytes;
		memcpy(data2, data, size2 * m_elementSizeInBytes);
	}
	else
	{
		memcpy(data1, data, size1 * m_elementSizeInBytes);
	}
	AdvanceWriteIndex(numWritten);
	return numWritten;
}

//  --------------------------------------------------------------------------------
/*
Reads data from the ring buffer
*/
int RingBuffer::Read
(
	void* data,
	int elementCount
	)
{
	assert(elementCount <= GetAvailableElementsForReading());

	int size1 = 0;
	int size2 = 0;
	int numRead = 0;
	void* data1 = nullptr;
	void* data2 = nullptr;

	numRead = GetReadRegions(elementCount, &data1, &size1, &data2, &size2);
	if (size2 > 0)
	{
		memcpy(data, data1, size1 * m_elementSizeInBytes);
		data = (reinterpret_cast<unsigned char*>(data)) + size1 * m_elementSizeInBytes;
		memcpy(data, data2, size2 * m_elementSizeInBytes);
	}
	else
	{
		memcpy(data, data1, size1 * m_elementSizeInBytes);
	}
	AdvanceReadIndex(numRead);
	return numRead;
}

//  --------------------------------------------------------------------------------
/*
Return number of elements available for reading
*/
int RingBuffer::GetAvailableElementsForReading() const
{
	return ((m_writePos - m_readPos) & m_bigMask);
}

//  --------------------------------------------------------------------------------
/*
Return number of elements available for writing
*/
int RingBuffer::GetAvailableElementsForWriting() const
{
	return (m_capacity - GetAvailableElementsForReading());
}

//  --------------------------------------------------------------------------------
/*
Flush buffer
*/
void RingBuffer::Flush()
{
	m_writePos = 0;
	m_readPos = 0;
}

//  --------------------------------------------------------------------------------
/*
Sets read position
*/
void RingBuffer::SetReadPosition(int readPos)
{
	m_readPos = readPos;
}

//  --------------------------------------------------------------------------------
/*
Sets write position
*/
void RingBuffer::SetWritePosition(int writePos)
{
	m_writePos = writePos;
}

//  --------------------------------------------------------------------------------
/*
Get address of region(s) to which we can write data.
If the region is contiguous, size2 will be zero.
If non-contiguous, size2 will be the size of second region.
Returns room available to be written or elementCount, whichever is smaller.
*/
int RingBuffer::GetWriteRegions
(
	int elementCount,
	void** dataPtr1,
	int* sizePtr1,
	void** dataPtr2,
	int* sizePtr2
	)
{
	int index;
	int available = GetAvailableElementsForWriting();
	if (elementCount > available)
	{
		elementCount = available;
	}

	// Check to see if write is not contiguous
	index = m_writePos & m_smallMask;
	if ((index + elementCount) > m_capacity)
	{
		// Write data in two blocks that wrap the buffer
		int firstHalf = m_capacity - index;
		*dataPtr1 = &m_data[index * m_elementSizeInBytes];
		*sizePtr1 = firstHalf;
		*dataPtr2 = &m_data[0];
		*sizePtr2 = elementCount - firstHalf;
	}
	else
	{
		*dataPtr1 = &m_data[index * m_elementSizeInBytes];
		*sizePtr1 = elementCount;
		*dataPtr2 = nullptr;
		*sizePtr2 = 0;
	}

	return elementCount;
}

//  --------------------------------------------------------------------------------
/*
Advance the write index to the next location to be written
*/
int RingBuffer::AdvanceWriteIndex
(
	int elementCount
	)
{
	return m_writePos = (m_writePos + elementCount) & m_bigMask;
}

//  --------------------------------------------------------------------------------
/*
Get address of region(s) from which we can read data.
If the region is contiguous, size2 will be zero.
If non-contiguous, size2 will be the size of second region.
Returns room available to be read or elementCount, whichever is smaller.
*/
int RingBuffer::GetReadRegions
(
	int elementCount,
	void** dataPtr1,
	int* sizePtr1,
	void** dataPtr2,
	int* sizePtr2
	)
{
	int index;
	int available = GetAvailableElementsForReading();
	if (elementCount > available)
	{
		elementCount = available;
	}

	// Check to see if read is not contiguous
	index = m_readPos & m_smallMask;
	if ((index + elementCount) > m_capacity)
	{
		// Write data in two blocks that wrap the buffer
		int firstHalf = m_capacity - index;
		*dataPtr1 = &m_data[index * m_elementSizeInBytes];
		*sizePtr1 = firstHalf;
		*dataPtr2 = &m_data[0];
		*sizePtr2 = elementCount - firstHalf;
	}
	else
	{
		*dataPtr1 = &m_data[index * m_elementSizeInBytes];
		*sizePtr1 = elementCount;
		*dataPtr2 = nullptr;
		*sizePtr2 = 0;
	}

	return elementCount;
}

//  --------------------------------------------------------------------------------
/*
Advance the read index to the next location to be read
*/
int RingBuffer::AdvanceReadIndex(int elementCount)
{
	return m_readPos = (m_readPos + elementCount) & m_bigMask;
}
