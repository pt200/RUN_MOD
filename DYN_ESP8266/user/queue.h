// Bondarenko Andrey
#ifndef __QUEUE_H__
#define __QUEUE_H__

ICACHE_FLASH_ATTR void* q32create( int len);

// NOT SAFE
int q32push_overwrite( void* q32, int32_t v);
int q32pop( void* q32, int32_t* v);

#endif
