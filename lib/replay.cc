#include "stdafx.h"

Napi::Object Replay::Init(Napi::Env env, Napi::Object exports) {
	Napi::Function func = DefineClass(env, "Replay", {
		InstanceMethod("getHeaderInformation", &Replay::getHeaderInformation),
        InstanceMethod("getPlayerNames", &Replay::getPlayerNames),
        InstanceMethod("getScriptName", &Replay::getScriptName),
        InstanceMethod("getParameters", &Replay::getParameters),
        InstanceMethod("getDecks", &Replay::getDecks),
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
	Napi::ObjectWrap<Replay>(info), buffer(nullptr), header(nullptr), decompressedBuffer(nullptr), homePlayerCount(0), awayPlayerCount(0) {
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

	if (this->header->id == REPLAY_YRP1) {
		this->parseDecks(env);
	}
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
Napi::Value Replay::getDecks(const Napi::CallbackInfo& info) {
    auto env = info.Env();
	auto generateArray = [&env](const std::vector<uint32_t>& cards) {
		auto cardArray = Napi::Array::New(env, cards.size());
		for (size_t i = 0, j = cards.size(); i < j; ++i) {
			cardArray.Set(i, cards[i]);
		}

		return cardArray;
	};

	auto deckObjectArray = Napi::Array::New(env, this->decks.size());
	for (size_t i = 0; i < this->decks.size(); ++i) {
		const auto& main = this->decks[i].main;
		const auto& extra = this->decks[i].extra;

		auto deckObject = Napi::Object::New(env);
        deckObject.Set(Napi::String::New(env, "main"), generateArray(main));
        deckObject.Set(Napi::String::New(env, "extra"), generateArray(extra));

		deckObjectArray.Set(i, deckObject);
    }

	return deckObjectArray;
}

void Replay::parseHeaderInformation(Napi::Env& env) {
	this->header = new ReplayHeader();
	if (!this->buffer->readItem(this->header, 1)) {
		Napi::TypeError::New(env, "Unexpected end of file").ThrowAsJavaScriptException();
	}
}
void Replay::parsePlayerNames(Napi::Env& env) {
    uint32_t playerCount = 0;
    char16_t nameBuffer[20];
	if (this->header->flag & REPLAY_SINGLE_MODE) {
		for (size_t i = 0; i < 2; ++i) {
            this->getName(nameBuffer);
            this->playerNames.emplace_back(nameBuffer);
		}

		this->homePlayerCount = 1;
		this->awayPlayerCount = 1;
	}

    auto parse = [this, &env](uint32_t& count) {
        if (this->header->flag & REPLAY_NEWREPLAY)
            count = this->decompressedBuffer->read<uint32_t>();
        else if (this->header->flag & REPLAY_TAG)
            count = 2;
        else
            count = 1;

        for (uint32_t i = 0; i < count; i++) {
			char16_t namebuf[20];
            this->getName(namebuf);
            this->playerNames.emplace_back(namebuf);
        }
    };

	parse(this->homePlayerCount);
	parse(this->awayPlayerCount);
}
void Replay::parseParams(Napi::Env& env) {
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
void Replay::parseDecks(Napi::Env& env) {
    if (this->header->id != REPLAY_YRP1 || (this->header->flag & REPLAY_SINGLE_MODE && !(this->header->flag & REPLAY_HAND_TEST)))
        return;

	auto readCards = [this](std::vector<uint32_t>& cards, uint32_t count) {
		uint32_t cardId = 0;
		for (uint32_t i = 0; i < count; ++i) {
            cards.push_back(this->decompressedBuffer->read<uint32_t>());
		}
	};

    for (uint32_t i = 0, j = this->homePlayerCount + this->awayPlayerCount; i < j; ++i) {
        Deck deck;

		uint32_t mainCount = this->decompressedBuffer->read<uint32_t>();
        readCards(deck.main, mainCount);

		uint32_t extraCount = this->decompressedBuffer->read<uint32_t>();
        readCards(deck.extra, extraCount);

        this->decks.push_back(std::move(deck));
    }

    this->customRuleCards.clear();
    if (this->header->flag & REPLAY_NEWREPLAY && !(this->header->flag & REPLAY_HAND_TEST)) {
        uint32_t rules = this->decompressedBuffer->read<uint32_t>();
        for (uint32_t i = 0; i < rules; ++i)
            this->customRuleCards.push_back(this->decompressedBuffer->read<uint32_t>());
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
