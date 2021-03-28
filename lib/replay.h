#ifndef _REPLAY_H_
#define _REPLAY_H_

class Replay : public Napi::ObjectWrap<Replay> {
public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	static Napi::Object NewInstance(Napi::Env env, Napi::Value arg);

public:
	Replay(const Napi::CallbackInfo& info);

// -> NodeJS
private:
	Napi::Value getHeaderInformation(const Napi::CallbackInfo& info);

// -> C++
private:
	void parseHeaderInformation(Napi::Env& env);

private:
	Buffer* buffer;
	ReplayHeader* header;
};

#endif
