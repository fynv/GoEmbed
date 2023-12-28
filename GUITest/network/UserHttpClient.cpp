#include "UserHttpClient.h"
#include "HttpClient.h"

void HttpGetResult_Delete(void* ptr)
{
	delete (GetResult*)ptr;
}

int HttpGetResult_GetResult(void* ptr)
{
	auto* result = (GetResult*)ptr;
	return result->result?1:0;
}

size_t HttpGetResult_GetSize(void* ptr)
{
	auto* result = (GetResult*)ptr;
	return result->data.size();
}

void* HttpGetResult_GetData(void* ptr)
{
	auto* result = (GetResult*)ptr;
	return result->data.data();
}


void* HttpClient_New()
{
	return new HttpClient;
}

void HttpClient_Delete(void* ptr)
{
	delete (HttpClient*)ptr;
}

void* HttpClient_Get(void* ptr, const char* url)
{
	HttpClient* self = (HttpClient*)ptr;
	auto* result = new GetResult;
	self->Get(url, result->data);
	result->result = true;
	return result;
}

struct TUserData
{
	HttpGetCallback callback;
	void* userData;
};

static void _GetCallback(const GetResult& result, void* userData)
{
	TUserData* ud = (TUserData*)userData;
	GetResult* res = new GetResult;
	*res = result;
	ud->callback(res, ud->userData);
	delete ud;
}

void HttpClient_GetAsync(void* ptr, const char* url, HttpGetCallback callback, void* userData)
{
	HttpClient* self = (HttpClient*)ptr;
	TUserData* ud = new TUserData({ callback, userData });
	self->GetAsync(url, _GetCallback, ud);
}