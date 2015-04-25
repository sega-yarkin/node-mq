{
	'targets': [
		{
			'target_name': 'mq_helper',
			'sources': [
				'src/mq_helper.cc',
				'src/queue.cc',
				'src/utils.cc'
			],
			'cflags': [ '-O3' ],
			'ldflags': [ '-lrt' ],
		}
	]
}