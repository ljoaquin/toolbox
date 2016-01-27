#include "packet.h"
#include <memory.h>

namespace toolbox
{

	void packet_init(packet_t* packet, unsigned int capacity)
	{
        packet->capacity = capacity;
		packet->buf = new unsigned char[PACKET_DEFAULT_BUF_CAPACITY];
		memset(packet->buf, 0, PACKET_DEFAULT_BUF_CAPACITY);
		packet->len = 0;
		packet->index = 0;
	}

	void packet_destory(packet_t* packet)
	{
		delete [] packet->buf;
	}

	static void packet_set(packet_t* packet, int index, void* value, int value_size)
	{
		memcpy(packet->buf + index, value, value_size);
		packet->len = index + value_size;
		packet->index = packet->len;
	}

	static void packet_get(packet_t* packet, int index, void* buf, int buf_len)
	{
		memcpy(buf, packet->buf + index, buf_len);
		packet->index = index + buf_len;
	}

	void packet_set(packet_t* packet, void* value, int value_size)
	{
		packet_set(packet, packet->index, value, value_size);
	}

	void packet_get(packet_t* packet, void* buf, int buf_len)
	{
		packet_get(packet, packet->index, buf, buf_len);
	}
	
	void packet_resize(packet_t* packet)
	{
		unsigned char* new_buf = new unsigned char[packet->len];
		packet_get(packet, 0, new_buf, packet->len);
		delete [] packet->buf;
		packet->buf = new_buf;
		packet->capacity = packet->len;
	}

}