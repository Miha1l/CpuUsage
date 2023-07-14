#pragma once

template <typename T>
class GraphVector {
private:
    T* buffer;
    int bufferSize;
    int elemCount;

public:
    GraphVector() {
        bufferSize = 50;
        buffer = new T[bufferSize];
        elemCount = 0;
    }

    GraphVector(int bufferSize) : bufferSize(bufferSize) {
        buffer = new T[bufferSize];
        elemCount = 0;
    }

    ~GraphVector() {
        delete[] buffer;
    }

    void pushBack(T value) {
        if (elemCount == bufferSize) {
            elemCount = 0;
        }
        buffer[elemCount] = value;
        elemCount++;
    }

    unsigned int getBufferSize() {
        return bufferSize * sizeof(T);
    }

    int getElementsCount() {
        return elemCount;
    }

    T* getBufferPointer() {
        return buffer;
    }

    T& operator [] (int index) {
        if (index >= elemCount) {
            return buffer[elemCount - 1];
        }
        else if (index < 0) {
            return buffer[0];
        }
        return buffer[index];
    }
};