#include "MemoryStream.h"
#include <cstring>
#include <stdexcept>

#ifndef _MSC_VER
#define memcpy_s(dest, destSize, source, sourceSize) memcpy(dest, source, destSize)
#endif

LUS::MemoryStream::MemoryStream() {
    mBuffer = std::vector<char>();
    // mBuffer.reserve(1024 * 16);
    mExternalBuffer = nullptr;
    mBufferSize = 0;
    mBaseAddress = 0;
}

LUS::MemoryStream::MemoryStream(char* nBuffer, size_t nBufferSize) : MemoryStream() {
    mBuffer = std::vector<char>(nBuffer, nBuffer + nBufferSize);
    mBufferSize = nBufferSize;
}

LUS::MemoryStream::MemoryStream(const char* nBuffer, size_t nBufferSize, bool copy) : MemoryStream() {
    if (!copy) {
        mExternalBuffer = nBuffer;
        mBufferSize = nBufferSize;
        return;
    }

    mBuffer = std::vector<char>(nBuffer, nBuffer + nBufferSize);
    mBufferSize = nBufferSize;
}

LUS::MemoryStream::~MemoryStream() {
}

uint64_t LUS::MemoryStream::GetLength() {
    return mBufferSize;
}

void LUS::MemoryStream::Seek(int32_t offset, SeekOffsetType seekType) {
    if (seekType == SeekOffsetType::Start) {
        mBaseAddress = offset;
    } else if (seekType == SeekOffsetType::Current) {
        mBaseAddress += offset;
    } else if (seekType == SeekOffsetType::End) {
        mBaseAddress = mBufferSize - 1 - offset;
    }
}

std::unique_ptr<char[]> LUS::MemoryStream::Read(size_t length) {
    std::unique_ptr<char[]> result = std::make_unique<char[]>(length);

    memcpy_s(result.get(), length, ReadBuffer() + mBaseAddress, length);
    mBaseAddress += length;

    return result;
}

void LUS::MemoryStream::Read(const char* dest, size_t length) {
    memcpy_s((void*)dest, length, ReadBuffer() + mBaseAddress, length);
    mBaseAddress += length;
}

int8_t LUS::MemoryStream::ReadByte() {
    return ReadBuffer()[mBaseAddress++];
}

void LUS::MemoryStream::Write(char* srcBuffer, size_t length) {
    if (mExternalBuffer != nullptr) {
        throw std::runtime_error("Cannot write to a borrowed MemoryStream");
    }

    if (mBaseAddress + length >= mBuffer.size()) {
        mBuffer.resize(mBaseAddress + length);
        mBufferSize += length;
    }

    memcpy_s(&mBuffer[mBaseAddress], length, srcBuffer, length);
    mBaseAddress += length;
}

void LUS::MemoryStream::WriteByte(int8_t value) {
    if (mExternalBuffer != nullptr) {
        throw std::runtime_error("Cannot write to a borrowed MemoryStream");
    }

    if (mBaseAddress >= mBuffer.size()) {
        mBuffer.resize(mBaseAddress + 1);
        mBufferSize = mBaseAddress;
    }

    mBuffer[mBaseAddress++] = value;
}

std::vector<char> LUS::MemoryStream::ToVector() {
    if (mExternalBuffer != nullptr) {
        return std::vector<char>(mExternalBuffer, mExternalBuffer + mBufferSize);
    }

    return mBuffer;
}

void LUS::MemoryStream::Flush() {
}

void LUS::MemoryStream::Close() {
}

const char* LUS::MemoryStream::ReadBuffer() const {
    return mExternalBuffer != nullptr ? mExternalBuffer : mBuffer.data();
}
