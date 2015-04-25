/**
 */

#include <errno.h>
#include <assert.h>
#include "utils.h"

namespace utils {

using node::ErrnoException;

using v8::Handle;
using v8::FunctionCallbackInfo;
using v8::String;
using v8::Number;
using v8::Object;
using v8::Value;
using v8::Local;
using v8::Isolate;
using v8::HandleScope;
using v8::Exception;

void
GetMsgQueueRLimit( const FunctionCallbackInfo<Value>& args ) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope( isolate );
	
	rlim_t resource = RLIMIT_MSGQUEUE;
	struct rlimit limit;
	
	if( getrlimit(resource, &limit) ) {
		isolate->ThrowException(
			ErrnoException( errno, "getrlimit" )
		);
		return;
	}
	Local<Object> result = Object::New( isolate );
	result->Set(
		String::NewFromUtf8( isolate, "soft" ),
		Number::New( isolate, limit.rlim_cur )
	);
	result->Set(
		String::NewFromUtf8( isolate, "hard" ),
		Number::New( isolate, limit.rlim_max )
	);
	
	args.GetReturnValue().Set( result );
}

void
SetMsgQueueRLimit( const FunctionCallbackInfo<Value>& args ) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope( isolate );
	
	// Check input arguments
	if( args.Length() < 1 ) {
		isolate->ThrowException( Exception::TypeError(
			String::NewFromUtf8( isolate, "Required one argument" )
		));
		return;
	}
	
	if( !args[0]->IsObject() && !args[0]->IsUint32() ) {
		isolate->ThrowException( Exception::TypeError(
			String::NewFromUtf8( isolate, "First argument should be an object or an integer" )
		));
		return;
	}
	
	rlim_t resource = RLIMIT_MSGQUEUE;
	struct rlimit limit;
	
	// Get current limits
	if( getrlimit(resource, &limit) ) {
		isolate->ThrowException(
			ErrnoException( errno, "getrlimit" )
		);
		return;
	}
	
	if( args[0]->IsObject() ) {
		Local<Object> limit_obj = args[0]->ToObject();
		Local<Value> soft = limit_obj->Get( String::NewFromUtf8(isolate, "soft") );
		Local<Value> hard = limit_obj->Get( String::NewFromUtf8(isolate, "hard") );
		
		if( soft->IsUint32() ) limit.rlim_cur = soft->Uint32Value();
		if( hard->IsUint32() ) limit.rlim_max = hard->Uint32Value();
		
		if( limit.rlim_cur > limit.rlim_max ) {
			isolate->ThrowException( Exception::RangeError(
				String::NewFromUtf8( isolate, "soft limit should be less or equal to hard limit" )
			));
			return;
		}
	}
	else {
		limit.rlim_cur = args[0]->Uint32Value();
		if( limit.rlim_max < limit.rlim_cur ) {
			limit.rlim_max = limit.rlim_cur;
		}
	}
	
	if( setrlimit(resource, &limit) ) {
		isolate->ThrowException(
			ErrnoException( errno, "getrlimit" )
		);
		return;
	}
}

} // namespace utils
