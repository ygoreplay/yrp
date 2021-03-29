#ifndef _REPLAY_H_
#define _REPLAY_H_

class Replay : public Napi::ObjectWrap<Replay> {
public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	static Napi::Object NewInstance(Napi::Env env, Napi::Value arg);

public:
	Replay(const Napi::CallbackInfo& info);
	~Replay(void);

// -> NodeJS
private:
	Napi::Value getHeaderInformation(const Napi::CallbackInfo& info);
	Napi::Value getPlayerNames(const Napi::CallbackInfo& info);
	Napi::Value getParameters(const Napi::CallbackInfo& info);
	Napi::Value getScriptName(const Napi::CallbackInfo& info);
	Napi::Value getDecks(const Napi::CallbackInfo& info);
	
// -> C++
private:
	void parseHeaderInformation(Napi::Env& env);
	void parsePlayerNames(Napi::Env& env);
	void parseParams(Napi::Env& env);
	void parseDecks(Napi::Env& env);
	void decompressData(Napi::Env& env);
	void getName(char16_t* data);

private:
	Buffer* buffer;
	Buffer* decompressedBuffer;
	ReplayHeader* header;

	ReplayParameters parameters;
	std::vector<std::u16string> playerNames;
    std::vector<Deck> decks;
	std::vector<uint32_t> customRuleCards;
    std::string scriptName;

	uint32_t homePlayerCount;
	uint32_t awayPlayerCount;
};

#endif
