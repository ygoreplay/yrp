#include <iostream>
#include <vector>
#include <napi.h>

#include "lzma/LzmaLib.h"

struct ReplayHeader {
    uint32_t id;
    uint32_t version;
    uint32_t flag;
    uint32_t seed;
    uint32_t datasize;
    uint32_t hash;
    uint8_t props[8];
};

Napi::Value uncompressReplayData(const Napi::CallbackInfo& info) {
    // info[0] - original replay file data
    Napi::Env env = info.Env();
    
    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsBuffer()) {
        Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto bufferObject = info[0].As<Napi::Buffer<uint8_t>>();
    auto buffer = bufferObject.Data();
    auto bufferLength = bufferObject.ByteLength();

    size_t compressedSize = bufferLength - sizeof(ReplayHeader);
    uint8_t* dataBuffer = new uint8_t[bufferLength];
    memcpy(dataBuffer, buffer, bufferLength);

    ReplayHeader header;
    memcpy(&header, dataBuffer, sizeof(ReplayHeader));

    std::vector<uint8_t> replayData;
    replayData.resize(header.datasize);
    size_t dataSize = header.datasize;

    if (LzmaUncompress(replayData.data(), &dataSize, dataBuffer + sizeof(ReplayHeader), &compressedSize, header.props, 5) != SZ_OK) {
        delete[] dataBuffer;
        Napi::TypeError::New(env, "Failed to uncompress replay data").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    delete[] dataBuffer;
    return Napi::Buffer<uint8_t>::New(env, replayData.data(), dataSize);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "uncompressReplayData"), Napi::Function::New(env, uncompressReplayData));
    return exports;
}

NODE_API_MODULE(addon, Init)