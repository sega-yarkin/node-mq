/**
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <node.h>
#include <sys/resource.h>

namespace utils {

using v8::FunctionCallbackInfo;
using v8::Value;


/**
 * Get current message queue resource limits (memory).
 *
 * var lim = GetMsgQueueRLimit();
 * lim -> { "soft": -1, "hard": -1 }
 */
void
GetMsgQueueRLimit( const FunctionCallbackInfo<Value>& args );

/**
 * Set message queue resource limits (memory).
 *
 * var lim = { "soft": -1, "hard": -1 };
 * SetMsgQueueRLimit( lim );
 */
void
SetMsgQueueRLimit( const FunctionCallbackInfo<Value>& args );

} // namespace utils

#endif // UTILS_H_
