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
	
// -> C++
private:
	void parseHeaderInformation(Napi::Env& env);
	void parsePlayerNames(Napi::Env& env);
	void decompressData(Napi::Env& env);
	void getName(char16_t* data);

private:
	Buffer* buffer;
	Buffer* decompressedBuffer;
	ReplayHeader* header;

	std::vector<std::u16string> playerNames;
};

#endif
