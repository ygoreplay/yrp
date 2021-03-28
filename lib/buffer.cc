#include "stdafx.h"

Buffer::Buffer(std::uint8_t* data, std::size_t byteLength) : _data(nullptr), cursor(0), byteLength(byteLength) {
	this->_data = new std::uint8_t[byteLength];
	memcpy(this->_data, data, byteLength);
}
Buffer::~Buffer(void) {
	delete[] this->_data;
}

bool Buffer::checkIfCursorMovable(size_t byteLength) {
	if (this->cursor + byteLength >= this->byteLength) {
		return false;
	}

	return true;
}
std::size_t Buffer::moveCursorForward(size_t byteLength) {
	if (this->cursor + byteLength >= this->byteLength) {
		throw std::invalid_argument("Tried to read out of data length.");
	}

	std::size_t currentCursor = this->cursor;
	this->cursor += byteLength;

	return currentCursor;
}

std::uint8_t* Buffer::data(void) {
	return this->_data;
}
std::size_t Buffer::size(void) const {
	return this->byteLength;
}