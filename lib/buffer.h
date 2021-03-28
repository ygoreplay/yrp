#ifndef _BUFFER_H_
#define _BUFFER_H_

class Buffer {
public:
	Buffer(std::uint8_t* data, std::size_t byteLength);
	~Buffer(void);

private:
	bool checkIfCursorMovable(size_t byteLength);
	std::size_t moveCursorForward(size_t byteLength);

public:
	std::uint8_t* data(void);

public:
	template<typename T>
	std::uint8_t readItem(T* dest, std::size_t count) {
		std::size_t moveLength = sizeof(T) * count;
		if (!this->checkIfCursorMovable(moveLength)) {
			return false;
		}

		memcpy(dest, this->_data + this->moveCursorForward(moveLength), moveLength);
		return true;
	}

private:
	std::uint8_t* _data;
	std::size_t byteLength;
	std::size_t cursor;
};


#endif