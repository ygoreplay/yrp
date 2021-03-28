#include "stdafx.h"

Napi::Object Replay::Init(Napi::Env env, Napi::Object exports) {
	Napi::Function func = DefineClass(env, "Replay", { InstanceMethod("getHeaderInformation", &Replay::getHeaderInformation) });
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
	Napi::ObjectWrap<Replay>(info), buffer(nullptr), header(nullptr) {
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
}

Napi::Value Replay::getHeaderInformation(const Napi::CallbackInfo& info) {
	auto env = info.Env();
	if (this->header == nullptr) {
		this->parseHeaderInformation(env);
	}

	Napi::Object headerObject = Napi::Object::New(env);
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

void Replay::parseHeaderInformation(Napi::Env& env) {
	if (this->header != nullptr) {
		return;
	}

	this->header = new ReplayHeader();
	if (!this->buffer->readItem(this->header, 1)) {
		Napi::TypeError::New(env, "Unexpected end of file").ThrowAsJavaScriptException();
	}
}

