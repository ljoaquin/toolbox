#ifndef __PACKET_H__
#define __PACKET_H__

namespace toolbox
{
    // 'len' means data len, len <= capacity

	static const int PACKET_DEFAULT_BUF_CAPACITY = 1024;

	typedef struct packet_t
	{
        unsigned int capacity;
		unsigned char* buf;
		unsigned int len; // data len, not buf size
		unsigned int index;
	}packet_t;

	void packet_init(packet_t* packet, unsigned int capacity = PACKET_DEFAULT_BUF_CAPACITY);

	void packet_destory(packet_t* packet);

//	void packet_set(packet_t* packet, int index, void* value, int value_size);
//
//	void packet_get(packet_t* packet, int index, void* value, int size);

	void packet_set(packet_t* packet, void* value, int size);

	void packet_get(packet_t* packet, void* value, int size);

}

#endif // __PACKET_H__