/**
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <node.h>
#include <node_buffer.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>


namespace mq {

using v8::FunctionCallbackInfo;
using v8::Value;

/**
 * Open a message queue.
 *
 * var mqd = mq.open( name, flags[, mode, attr] );
 */
void open( const FunctionCallbackInfo<Value>& args );

/**
 * Get message queue attributes.
 *
 * var attr = mq.getattr( mqd );
 * attr -> { flags, maxmsg, msgsize, curmsgs }
 */
void getattr( const FunctionCallbackInfo<Value>& args );

/**
 * Send a message to a message queue.
 *
 * var err = mq.send( mqd, buff[, len, prio] );
 */
void send( const FunctionCallbackInfo<Value>& args );

/**
 * Receive a message from a message queue;
 *
 * mq.receive(
 *      mqd, buff, len,
 *      function callback( err, buff, size, prio ) {
 *              //
 *      }
 * )
 */
void receive( const FunctionCallbackInfo<Value>& args );

/**
 * Close a message queue.
 *
 * var err = mq.close( mqd );
 */
void close( const FunctionCallbackInfo<Value>& args );

/**
 * Remove a message queue.
 *
 * var err = mq.unlink( name );
 */
void unlink( const FunctionCallbackInfo<Value>& args );



} // namespace mq

#endif // QUEUE_H_
