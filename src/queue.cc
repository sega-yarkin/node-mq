/**
 */

#include <errno.h>
#include <assert.h>
#include "queue.h"

namespace mq {

using node::ErrnoException;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::HandleScope;
using v8::Exception;
using v8::Local;
using v8::Value;


#define TYPE_ERROR( isolate, str ) \
	isolate->ThrowException( Exception::TypeError( String::NewFromUtf8( isolate, str ) ))
#define RANGE_ERROR( isolate, str ) \
	isolate->ThrowException( Exception::RangeError( String::NewFromUtf8( isolate, str ) ))
#define ERRNO_EXCPT( isolate, errno, fn_name ) \
	isolate->ThrowException( ErrnoException(errno, fn_name) )



void open( const FunctionCallbackInfo<Value>& args ) {
	// args[0] - name
	// args[1] - flags
	// args[2] - mode
	// args[3] - attr
	// return mqd
	
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope( isolate );
	
	char*  name;
	int    flags;
	mode_t mode;
	struct mq_attr attr = { 0, 0, 0, 0 };
	mqd_t  mqd;
	
	Local<Value> val;
	String::AsciiValue asciiname;
	mq_attr* pattr = NULL;
	
	// Check input arguments
	if( args.Length() < 2 ) {
		TYPE_ERROR( isolate, "Required at least two argument" );
		return;
	}
	
	if( ! args[0]->IsString() ) {
		TYPE_ERROR( isolate, "First argument should be a string" );
		return;
	}
	else {
		asciiname = args[0]->ToString();
		if( asciiname->length() > NAME_MAX ) {
			RANGE_ERROR( isolate, "Name should be shorter than NAME_MAX" );
			return;
		}
		name = (char*)(*asciiname);
	}
	
	if( ! args[1]->IsUint32() ) {
		TYPE_ERROR( isolate, "flags should be an integer" );
		return;
	}
	flags = args[1]->Uint32Value();
	
	if( args.Length() > 2 ) {
		if( ! args[2]->IsUint32() ) {
			TYPE_ERROR( isolate, "mode should be an integer" );
			return;
		}
		mode = args[2]->Uint32Value();
	}
	if( args.Length() > 3 ) {
		if( ! args[3]->IsObject() ) {
			TYPE_ERROR( isolate, "attr should be an integer" );
			return;
		}
		Local<Object>attr_obj = args[3]->ToObject();
		// maxmsg
		val = attr_obj->Get( String::NewFromUtf8(isolate, "maxmsg") );
		if( val->IsUint32() ) {
			attr.mq_maxmsg = val->Uint32Value();
		}
		// msgsize
		val = attr_obj->Get( String::NewFromUtf8(isolate, "msgsize") );
		if( val->IsUint32() ) {
			attr.mq_msgsize = val->Uint32Value();
		}
		
		pattr = *attr;
	}
	
}

void getattr( const FunctionCallbackInfo<Value>& args ) {
	//
}

void send( const FunctionCallbackInfo<Value>& args ) {
	//
}

void receive( const FunctionCallbackInfo<Value>& args ) {
	//
}

void close( const FunctionCallbackInfo<Value>& args ) {
	//
}

void unlink( const FunctionCallbackInfo<Value>& args ) {
	//
}


} // namespace mq {

