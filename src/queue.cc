/**
 */

#include "queue.h"

namespace mq {

using node::ErrnoException;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::HandleScope;
using v8::Exception;
using v8::Local;
using v8::Value;
using v8::String;
using v8::Object;
using v8::Number;
using v8::Function;
using v8::Undefined;


#define TYPE_ERROR( isolate, str ) \
	isolate->ThrowException( Exception::TypeError( String::NewFromUtf8( isolate, str ) ))
#define RANGE_ERROR( isolate, str ) \
	isolate->ThrowException( Exception::RangeError( String::NewFromUtf8( isolate, str ) ))
#define ERRNO_EXCPT( isolate, errno, fn_name ) \
	isolate->ThrowException( ErrnoException(isolate, errno, fn_name) )

#define SET_PROP_NUMB( isolate, object, name, value ) \
	object->Set( \
		String::NewFromUtf8( isolate, name ), \
		Number::New( isolate, value ) \
	)


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
	struct mq_attr attr;
	mqd_t  mqd;
	
	Local<Value> val;
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
		Local<String>str = args[0]->ToString();
		int len = str->Length();
		if( len > NAME_MAX ) {
			RANGE_ERROR( isolate, "Name should be shorter than NAME_MAX" );
			return;
		}
		name = new char[len + 1];
		str->WriteOneByte( reinterpret_cast<uint8_t*>(name) );
		name[ len ] = (char) 0;
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
		if( attr.mq_msgsize > 65536 ) {
			RANGE_ERROR( isolate, "msgsize shouldn't be greater than 65536" );
			return;
		}
		
		pattr = &attr;
	}
	
	if( args.Length() > 2 ) {
		mqd = mq_open( name, flags, mode, pattr );
	}
	else {
		mqd = mq_open( name, flags );
	}
	delete[] name;
	
	if( mqd == (mqd_t)-1 ) {
		ERRNO_EXCPT( isolate, errno, "mq_open" );
		return;
	}
	
	args.GetReturnValue().Set( mqd );
}

void getattr( const FunctionCallbackInfo<Value>& args ) {
	// args[0] - mqd
	// return object
	
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope( isolate );
	
	struct mq_attr attr;
	mqd_t  mqd;
	
	if( ! args[0]->IsUint32() ) {
		TYPE_ERROR( isolate, "First argument should be an integer" );
		return;
	}
	mqd = args[0]->Uint32Value();
	if( mqd < 1 ) {
		RANGE_ERROR( isolate, "Invalid queue descriptor" );
		return;
	}
	
	int ret = mq_getattr( mqd, &attr );
	if( ret != 0 ) {
		ERRNO_EXCPT( isolate, errno, "mq_getattr" );
		return;
	}
	
	Local<Object> result = Object::New( isolate );
	SET_PROP_NUMB( isolate, result, "flags"  , attr.mq_flags   );
	SET_PROP_NUMB( isolate, result, "maxmsg" , attr.mq_maxmsg  );
	SET_PROP_NUMB( isolate, result, "msgsize", attr.mq_msgsize );
	SET_PROP_NUMB( isolate, result, "curmsgs", attr.mq_curmsgs );
	
	args.GetReturnValue().Set( result );
}

void send( const FunctionCallbackInfo<Value>& args ) {
	// args[0] - mqd
	// args[1] - buff
	// args[2] - len
	// args[3] - prio
	// return void
	
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope( isolate );
	
	mqd_t    mqd;
	char*    msg_ptr;
	size_t   msg_len;
	uint32_t msg_prio = 0;
	
	uint8_t* tmp_buff = NULL;
	size_t   src_len;
	
	// Check input arguments
	if( args.Length() < 2 ) {
		TYPE_ERROR( isolate, "Required at least two argument" );
		return;
	}
	
	if( ! args[0]->IsUint32() ) {
		TYPE_ERROR( isolate, "First argument should be an integer" );
		return;
	}
	mqd = args[0]->Uint32Value();
	if( mqd < 1 ) {
		RANGE_ERROR( isolate, "Invalid queue descriptor" );
		return;
	}
	
	if( node::Buffer::HasInstance(args[1]) ) {
		Local<Object> val = args[1]->ToObject();
		msg_ptr = node::Buffer::Data( val );
		src_len = node::Buffer::Length( val );
	}
	else if( args[1]->IsString() ) {
		Local<String> str = args[1]->ToString();
		src_len = str->Utf8Length();
		tmp_buff = new uint8_t[ src_len + 1 ];
		str->WriteOneByte( tmp_buff, 0, src_len+1 );
		msg_ptr = (char*) tmp_buff;
	}
	else {
		TYPE_ERROR( isolate, "Second argument should be a buffer or a string" );
		return;
	}
	
	if( args[2]->IsUint32() ) {
		msg_len = args[2]->Uint32Value();
		if( msg_len > src_len ) {
			msg_len = src_len;
		}
	}
	else {
		msg_len = src_len;
	}
	
	if( args[3]->IsUint32() ) {
		msg_prio = args[3]->Uint32Value();
	}
	
	int ret = mq_send( mqd, msg_ptr, msg_len, msg_prio );
	
	if( tmp_buff != NULL ) {
		delete[] tmp_buff;
	}
	if( ret != 0 ) {
		ERRNO_EXCPT( isolate, errno, "mq_send" );
		return;
	}
}

void receive( const FunctionCallbackInfo<Value>& args ) {
	// args[0] - mqd
	// args[1] - buff
	// args[2] - len
	// args[3] - callback
	// return void
	
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope( isolate );
	
	mqd_t    mqd;
	char*    msg_ptr = NULL;
	size_t   msg_len = 0;
	uint32_t msg_prio = 0;
	Local<Function> cb;
	
	Local<Object> buff;
	//bool buff_created = false;
	
	// Check input arguments
	if( args.Length() < 2 ) {
		TYPE_ERROR( isolate, "Required at least two argument" );
		return;
	}
	
	if( ! args[0]->IsUint32() ) {
		TYPE_ERROR( isolate, "First argument should be an integer" );
		return;
	}
	mqd = args[0]->Uint32Value();
	if( mqd < 1 ) {
		RANGE_ERROR( isolate, "Invalid queue descriptor" );
		return;
	}
	
	if( args.Length() == 2 ) {
		if( ! args[1]->IsFunction() ) {
			TYPE_ERROR( isolate, "Last argument should be a function" );
			return;
		}
		cb = Local<Function>::Cast( args[1] );
		struct mq_attr attr;
		int ret = mq_getattr( mqd, &attr );
		if( ret != 0 ) {
			ERRNO_EXCPT( isolate, errno, "mq_getattr" );
			return;
		}
		msg_len = attr.mq_msgsize;
		buff = node::Buffer::New( isolate, msg_len );
		msg_ptr = node::Buffer::Data( buff );
		//buff_created = true;
	}
	else if( args.Length() == 3 ) {
		if( ! node::Buffer::HasInstance(args[1]) ) {
			TYPE_ERROR( isolate, "Second argument should be a buffer" );
			return;
		}
		if( ! args[2]->IsFunction() ) {
			TYPE_ERROR( isolate, "Last argument should be a function" );
			return;
		}
		buff = args[1]->ToObject();
		msg_ptr = node::Buffer::Data( buff );
		msg_len = node::Buffer::Length( buff );
		cb = Local<Function>::Cast( args[2] );
	}
	else if( args.Length() > 3 ) {
		if( ! node::Buffer::HasInstance(args[1]) ) {
			TYPE_ERROR( isolate, "Second argument should be a buffer" );
			return;
		}
		if( ! args[2]->IsUint32() ) {
			TYPE_ERROR( isolate, "Third argument should be an integer" );
			return;
		}
		if( ! args[3]->IsFunction() ) {
			TYPE_ERROR( isolate, "Fourth argument should be a function" );
			return;
		}
		buff = args[1]->ToObject();
		msg_ptr = node::Buffer::Data( buff );
		msg_len = args[2]->Uint32Value();
		size_t buff_len = node::Buffer::Length( buff );
		if( msg_len > buff_len ) {
			RANGE_ERROR( isolate, "Buffer size is less than specified" );
			return;
		}
		cb = Local<Function>::Cast( args[3] );
	}
	
	ssize_t ret = mq_receive( mqd, msg_ptr, msg_len, &msg_prio );
	if( ret < 0 ) {
		Local<Value> err = ErrnoException( isolate, errno, "mq_receive" );
		Local<Value> argv[1] = { err };
		cb->Call( isolate->GetCurrentContext()->Global(), 1, argv );
	}
	else {
		Local<Value> argv[4] = {
			Undefined(isolate ),
			buff,
			Number::New( isolate, ret ),
			Number::New( isolate, msg_prio )
		};
		cb->Call( isolate->GetCurrentContext()->Global(), 4, argv );
	}
}

void close( const FunctionCallbackInfo<Value>& args ) {
	// args[0] - mqd
	// return void
	
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope( isolate );
	
	mqd_t  mqd;
	
	if( ! args[0]->IsUint32() ) {
		TYPE_ERROR( isolate, "First argument should be an integer" );
		return;
	}
	mqd = args[0]->Uint32Value();
	if( mqd < 1 ) {
		RANGE_ERROR( isolate, "Invalid queue descriptor" );
		return;
	}
	
	int ret = mq_close( mqd );
	if( ret != 0 ) {
		ERRNO_EXCPT( isolate, errno, "mq_close" );
		return;
	}
}

void unlink( const FunctionCallbackInfo<Value>& args ) {
	// args[0] - name
	// return void
	
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope( isolate );
	
	char*  name;
	
	if( ! args[0]->IsString() ) {
		TYPE_ERROR( isolate, "First argument should be a string" );
		return;
	}
	else {
		Local<String> str = args[0]->ToString();
		int len = str->Length();
		if( len > NAME_MAX ) {
			RANGE_ERROR( isolate, "Name should be shorter than NAME_MAX" );
			return;
		}
		name = new char[len + 1];
		str->WriteOneByte( reinterpret_cast<unsigned char*>(name) );
		name[ len ] = (char) 0;
	}
	int ret = mq_unlink( name );
	delete[] name;
	
	if( ret != 0 ) {
		ERRNO_EXCPT( isolate, errno, "mq_unlink" );
		return;
	}
}


} // namespace mq

