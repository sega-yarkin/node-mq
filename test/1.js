
// MSG_LIMIT*(MSG_SIZE+96)+10

var mq = require( "./build/Release/mq_helper" );
mq.set_rlimit(543162367)
var mqd = mq.open( "/test", mq.O_RDWR | mq.O_CREAT, parseInt("722", 8), {'maxmsg': 65536, 'msgsize': 8192} )

var mqd = mq.open( "/test", mq.O_RDWR | mq.O_CREAT, parseInt("722", 8) )

var res = mq.send( mqd, "Hello World!" );

mq.receive( mqd, function(err, buff, len, pri){ console.log(buff.toString()) })
