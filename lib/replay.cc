#include "stdafx.h"

Napi::Object Replay::Init(Napi::Env env, Napi::Object exports) {
	Napi::Function func = DefineClass(env, "Replay", {
		InstanceMethod("getHeaderInformation", &Replay::getHeaderInformation),
        InstanceMethod("getPlayerNames", &Replay::getPlayerNames),
        InstanceMethod("getScriptName", &Replay::getScriptName),
        InstanceMethod("getParameters", &Replay::getParameters),
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

	this->parameters = { 0 };
	this->buffer = new Buffer(bufferData, buffer.ByteLength());
	this->parseHeaderInformation(env);
	this->decompressData(env);
	this->parsePlayerNames(env);
	this->parseParams(env);
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
Napi::Value Replay::getParameters(const Napi::CallbackInfo& info) {
    auto env = info.Env();
    auto parameters = Napi::Object::New(env);

	parameters.Set(Napi::String::New(env, "startLP"), Napi::Number::New(env, this->parameters.startLP));
	parameters.Set(Napi::String::New(env, "startHand"), Napi::Number::New(env, this->parameters.startHand));
    parameters.Set(Napi::String::New(env, "drawCount"), Napi::Number::New(env, this->parameters.drawCount));
    parameters.Set(Napi::String::New(env, "duelFlags"), Napi::Number::New(env, this->parameters.duelFlags));

	return parameters;
}

Napi::Value Replay::getScriptName(const Napi::CallbackInfo& info) {
    auto env = info.Env();
	return Napi::String::New(env, this->scriptName.c_str());
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
void Replay::parseParams(Napi::Env& env) {
//     params = { 0 };
//     if (pheader.id == REPLAY_YRP1) {
//         params.start_lp = Read<uint32_t>();
//         params.start_hand = Read<uint32_t>();
//         params.draw_count = Read<uint32_t>();
//     }
//     if (pheader.flag & REPLAY_64BIT_DUELFLAG)
//         params.duel_flags = Read<uint64_t>();
//     else
//         params.duel_flags = Read<uint32_t>();
//     if (pheader.flag & REPLAY_SINGLE_MODE && pheader.id == REPLAY_YRP1) {
//         size_t slen = Read<uint16_t>();
//         scriptname.resize(slen);
//         ReadData(&scriptname[0], slen);
//     }

	if (this->header->id == REPLAY_YRP1) {
        this->parameters.startLP = this->decompressedBuffer->read<uint32_t>();
        this->parameters.startHand = this->decompressedBuffer->read<uint32_t>();
        this->parameters.drawCount = this->decompressedBuffer->read<uint32_t>();
    }

	if (this->header->flag & REPLAY_64BIT_DUELFLAG) {
        this->parameters.duelFlags = this->decompressedBuffer->read<uint64_t>();
	} else {
        this->parameters.duelFlags = this->decompressedBuffer->read<uint32_t>();
	}

	if (this->header->flag & REPLAY_SINGLE_MODE && this->header->id == REPLAY_YRP1) {
		std::size_t len = this->decompressedBuffer->read<uint16_t>();
		this->scriptName.resize(len);
		this->decompressedBuffer->readItem(&scriptName[0], len);
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
