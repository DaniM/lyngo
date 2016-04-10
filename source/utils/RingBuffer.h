#pragma once

#define DEBUG_RINGBUFFER_ENABLED 0

class RingBuffer
{

public:

	// Constructor for class
	RingBuffer(int elementSizeInBytes
			   , int elementCount);

	// Destructor for class
	~RingBuffer();

	// Flush buffer
	void Flush();

	// Writes data to the RingBuffer
	int Write(const void* data, int elementCount);

	// Reads data from the RingBuffer
	int Read(void* data, int elementCount);

	// Return number of elements available for reading
	int GetAvailableElementsForReading() const;

	// Return number of elements available for writing
	int GetAvailableElementsForWriting() const;

	// Sets the read position
	void SetReadPosition(int readPos);

	// Sets the write position
	void SetWritePosition(int writePos);

private:

	unsigned char* m_data;
	int m_writePos;				// Index of next writable element
	int m_readPos;				// Index of next readable element
	int m_bigMask;    			// Used for wrapping indices with extra bit to distinguish full / empty
	int m_smallMask;  			// Used for fitting indices to buffer
	int m_capacity; 			// Number of elements in FIFO. Power of 2
	int m_elementSizeInBytes;	// Number of bytes per element

								// Get address of region(s) to which we can write data.
								// If the region is contiguous, size2 will be zero.
								// If non-contiguous, size2 will be the size of second region.
								// Returns room available to be written or elementCount, whichever is smaller.
	int GetWriteRegions(int elementCount, void **dataPtr1, int *sizePtr1, void **dataPtr2, int *sizePtr2);

	// Advance the write index to the next location to be written
	int AdvanceWriteIndex(int elementCount);

	// Get address of region(s) from which we can read data.
	// If the region is contiguous, size2 will be zero.
	// If non-contiguous, size2 will be the size of second region.
	// Returns room available to be read or elementCount, whichever is smaller.
	int GetReadRegions(int elementCount, void **dataPtr1, int *sizePtr1, void **dataPtr2, int *sizePtr2);

	// Advance the read index to the next location to be read
	int AdvanceReadIndex(int elementCount);

};

