#include "stdafx.h"

Napi::Object Replay::Init(Napi::Env env, Napi::Object exports) {
	Napi::Function func = DefineClass(env, "Replay", {
		InstanceMethod("getHeaderInformation", &Replay::getHeaderInformation),
		InstanceMethod("getPlayerNames", &Replay::getPlayerNames),
	});
	Napi::FunctionReference* constructor = new Napi::FunctionReference();
	*constructor = Napi::Persistent(func);
	env.SetInstanceData(constructor);

	exports.Set("Replay", func);
	return exports;
}
Napi::Object Replay::NewInstance(Napi::Env env, Napi::Value arg) {
	Napi::EscapableHandleScope scope(env);
	Napi::Object obj = env.GetInstanceData<Napi::FunctionReference>()->New({ arg });
	return scope.Escape(napi_value(obj)).ToObject();
}

Replay::Replay(const Napi::CallbackInfo& info): 
	Napi::ObjectWrap<Replay>(info), buffer(nullptr), header(nullptr), decompressedBuffer(nullptr) {
	auto env = info.Env();
	if (info.Length() != 1) {
		Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
	}

	if (!info[0].IsBuffer()) {
		Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
	}

	auto buffer = info[0].As<Napi::Buffer<std::uint8_t>>();
	auto bufferData = buffer.Data();

	this->buffer = new Buffer(bufferData, buffer.ByteLength());
	this->parseHeaderInformation(env);
	this->decompressData(env);
	this->parsePlayerNames(env);
}
Replay::~Replay() {
	if (this->buffer) {
		delete this->buffer;
	}

	if (this->decompressedBuffer) {
		delete this->decompressedBuffer;
	}

	if (this->header) {
		delete this->header;
	}
}

Napi::Value Replay::getHeaderInformation(const Napi::CallbackInfo& info) {
	auto env = info.Env();
	auto headerObject = Napi::Object::New(env);

	headerObject.Set(Napi::String::New(env, "id"), Napi::Number::New(env, this->header->id));
	headerObject.Set(Napi::String::New(env, "version"), Napi::Number::New(env, this->header->version));
	headerObject.Set(Napi::String::New(env, "flag"), Napi::Number::New(env, this->header->flag));
	headerObject.Set(Napi::String::New(env, "seed"), Napi::Number::New(env, this->header->seed));
	headerObject.Set(Napi::String::New(env, "dataSize"), Napi::Number::New(env, this->header->datasize));
	headerObject.Set(Napi::String::New(env, "hash"), Napi::Number::New(env, this->header->hash));
	
	auto buffer = Napi::Buffer<uint8_t>::New(env, this->header->props, sizeof(this->header->props) / sizeof(uint8_t));
	headerObject.Set(Napi::String::New(env, "props"), buffer);

	return headerObject;
}
Napi::Value Replay::getPlayerNames(const Napi::CallbackInfo& info) {
	auto env = info.Env();
	Napi::Array playerNames = Napi::Array::New(env, this->playerNames.size());
	for (std::size_t i = 0, j = this->playerNames.size(); i < j; ++i) {
		playerNames.Set(i, Napi::String::New(env, this->playerNames[i].c_str()));
	}

	return playerNames;
}

void Replay::parseHeaderInformation(Napi::Env& env) {
	this->header = new ReplayHeader();
	if (!this->buffer->readItem(this->header, 1)) {
		Napi::TypeError::New(env, "Unexpected end of file").ThrowAsJavaScriptException();
	}
}
void Replay::parsePlayerNames(Napi::Env& env) {
	uint32_t playerCount = 0;
	if (this->header->flag & REPLAY_SINGLE_MODE) {
		playerCount = 2;
	} else if (this->header->flag & REPLAY_NEWREPLAY) {
		this->buffer->readItem(&playerCount, 1);
	} else if (this->header->flag & REPLAY_TAG) {
		playerCount = 4;
	} else {
		playerCount = 2;
	}

	char16_t nameBuffer[20];
	for (uint32_t i = 0; i < playerCount; ++i) {
		this->getName(nameBuffer);
		this->playerNames.emplace_back(nameBuffer);
	}
}
void Replay::decompressData(Napi::Env& env) {
	std::vector<std::uint8_t> replayData;
	size_t replaySize = this->header->datasize;
	auto compressedSize = this->buffer->size() - sizeof(ReplayHeader);
	replayData.resize(this->header->datasize);
	if (LzmaUncompress(replayData.data(), &replaySize, this->buffer->data() + sizeof(ReplayHeader), &compressedSize, this->header->props, 5) != SZ_OK) {
		Napi::TypeError::New(env, "Failed to decompress replay data").ThrowAsJavaScriptException();
	}

	this->decompressedBuffer = new Buffer(replayData.data(), replayData.size());
}
void Replay::getName(char16_t* data) {
	this->decompressedBuffer->readItem(data, 20);
	data[19] = 0;
}
