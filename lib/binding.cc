#include "stdafx.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
	std::cout << 1 << std::endl;

	return Replay::Init(env, exports);
}

NODE_API_MODULE(addon, InitAll)