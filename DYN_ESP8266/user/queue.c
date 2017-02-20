// Bondarenko Andrey

#include "osapi.h"
#include "user_interface.h"
#include "mem.h"

//#include <assert.h>

#define Q32_MAGIC	0x423654FD

struct _QUEUE32
{
	uint32_t MAGIC; // = Q32_MAGIC
	uint32_t size;
	int32_t p_write;
//	uint32_t p_read; // Calk ( "p_write" - "len")
	int32_t len;
	uint32_t data[ 0];
};

ICACHE_FLASH_ATTR void* q32create( int len)
{
	struct _QUEUE32* q = NULL;

	q = ( struct _QUEUE32*)os_zalloc( sizeof( struct _QUEUE32) + len * 4);
	if( q == NULL)
		return NULL; // Can't alloc mem

	// Clear
	os_memset( ( void*)q, 0, ( sizeof( struct _QUEUE32) + len * 4));

	// Init
	q->MAGIC = Q32_MAGIC;
	q->size = len;

	return q;
}

// NOT SAFE
int q32push_overwrite( void* q32, int32_t v)
{
	struct _QUEUE32* q = ( struct _QUEUE32*)q32;

	// Check
	if( q == NULL)
		return -1;
	if( q->MAGIC != Q32_MAGIC)
		return -2;

	// Push
	if( q->p_write >= q->size)
		q->p_write = 0;
	q->data[ q->p_write++] = v;

	// Update len
	if( q->len >= q->size)
		q->len = q->size;
	else
		q->len++;

	return 0;
}

// NOT SAFE
int q32pop( void* q32, int32_t* v)
{
	struct _QUEUE32* q = ( struct _QUEUE32*)q32;
	int32_t p_read = 0;

	// Check
	if( q == NULL)
		return -1;
	if( q->MAGIC != Q32_MAGIC)
		return -2;

	// Check size
	if( q->len == 0)
		return 1;

	// Pop
	p_read = q->p_write - q->len;
	if( p_read < 0)
		p_read += q->size;
	*v = q->data[ p_read];

	// Update len
	q->len--;

	return 0;
}
