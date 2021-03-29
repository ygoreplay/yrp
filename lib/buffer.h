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
    std::size_t size(void) const;

public:
    template<typename T>
    bool readItem(T* dest, std::size_t count) {
        std::size_t moveLength = sizeof(T) * count;
        if (!this->checkIfCursorMovable(moveLength)) {
            return false;
        }

        memcpy(dest, this->_data + this->moveCursorForward(moveLength), moveLength);
        return true;
    }

    template<typename T>
    T read() {
        T result;
        std::size_t moveLength = sizeof(T);
        if (!this->checkIfCursorMovable(moveLength)) {
            throw std::exception("Unexpected end of buffer");
        }

        memcpy(&result, this->_data + this->moveCursorForward(moveLength), moveLength);
        return result;
    }

private:
    std::uint8_t* _data;
    std::size_t byteLength;
    std::size_t cursor;
};


#endif