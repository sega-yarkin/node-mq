/**
 * POSIX message queue bindings.
 */

#include <node.h>
#include "utils.h"
#include "queue.h"
#include <fcntl.h>

//using namespace node;
using namespace v8;



void Init( Handle<Object> exports ) {
	// Export utils functions
	NODE_SET_METHOD( exports, "get_rlimit", utils::GetMsgQueueRLimit );
	NODE_SET_METHOD( exports, "set_rlimit", utils::SetMsgQueueRLimit );
	
	// Export constants
	NODE_DEFINE_CONSTANT( exports, O_RDONLY   );
	NODE_DEFINE_CONSTANT( exports, O_WRONLY   );
	NODE_DEFINE_CONSTANT( exports, O_RDWR     );
	NODE_DEFINE_CONSTANT( exports, O_CLOEXEC  );
	NODE_DEFINE_CONSTANT( exports, O_CREAT    );
	NODE_DEFINE_CONSTANT( exports, O_EXCL     );
	NODE_DEFINE_CONSTANT( exports, O_NONBLOCK );
	
	// Export mq functions
	NODE_SET_METHOD( exports, "open"    , mq::open    );
	NODE_SET_METHOD( exports, "get_attr", mq::getattr );
	NODE_SET_METHOD( exports, "send"    , mq::send    );
	NODE_SET_METHOD( exports, "receive" , mq::receive );
	NODE_SET_METHOD( exports, "close"   , mq::close   );
	NODE_SET_METHOD( exports, "unlink"  , mq::unlink  );
	
}

NODE_MODULE( mq, Init )

