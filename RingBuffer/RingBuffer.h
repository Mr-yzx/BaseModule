#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define MIN(x, y) ((x) < (y)) ? (x) : (y)

#define DEFAULT_ALIGN_SIZE 8

#define ALIGN(size, align)       (((size) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(size, align)  ((size) & ~((align) - 1))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ringbuffer_status
{
    RINGBUFFER_EMPTY,
    RINGBUFFER_FULL,
    RINGBUFFER_NOT_FULL_AND_NOT_EMPTY

} ringbuffer_status;

typedef struct ringbuffer_t
{
    uint32_t read_mirror : 1;
    uint32_t read_index : 31;
    uint32_t write_mirror : 1;
    uint32_t write_index : 31;
    uint32_t buffer_size;
    uint8_t *buffer;
} ringbuffer;

/**
/*@brief 创建ringbuffer
/*
/*@param length 缓存区大小
/*@return ringbuffer* 
*/
ringbuffer* ringbuffer_create(uint32_t length);

/**
/*@brief 销毁ringbuffer
/*
/*@param rb ringbuffer指针
 */
void ringbuffer_destroy(ringbuffer *rb);

/**
/*@brief 初始化ringbuffer
/*
/*@param rb ringbuffer指针
/*@param buffer 缓存区指针
/*@param length 缓存区大小
 */
void ringbuffer_init(ringbuffer *rb, uint8_t *buffer, uint32_t length);

/**
/*@brief 重置ringbuffer
/*
/*@param rb ringbuffer指针
 */
void ringbuffer_reset(ringbuffer *rb);


/**
/*@brief 向ringbuffer写入数据
/*
/*@param rb ringbuffer指针
/*@param data 数据指针
/*@param length 数据长度
/*@return uint32_t 实际写入长度
 */
uint32_t ringbuffer_put(ringbuffer *rb, uint8_t *data, uint32_t length);

/**
/*@brief 强制向ringbuffer写入数据
/*
/*@param rb ringbuffer指针
/*@param data 数据指针
/*@param length 数据长度
/*@return uint32_t 实际写入长度
 */
uint32_t ringbuffer_put_force(ringbuffer *rb, uint8_t *data, uint32_t length);

/**
/*@brief 从ringbuffer读取数据
/*
/*@param rb ringbuffer指针
/*@param data 数据指针
/*@param length 数据长度
/*@return uint32_t 实际读取长度
 */
uint32_t ringbuffer_get(ringbuffer *rb, uint8_t *data, uint32_t length);

/**
/*@brief 获取 ringbuffer 可读位置
/*
/*@param rb ringbuffer指针
/*@param ptr 第一个可读数据指针
/*@return uint32_t 数据长度
 */
uint32_t ringbuffer_peek(ringbuffer *rb, uint8_t **ptr);

/**
/*@brief 获取ringbuffer数据长度
/*
/*@param rb ringbuffer指针
/*@return uint32_t 数据长度
 */
uint32_t ringbuffer_data_len(ringbuffer *rb);

/**
/*@brief 获取ringbuffer大小
/*
/*@param rb ringbuffer指针
/*@return uint32_t buffer长度
 */
uint32_t ringbuffer_get_size(ringbuffer *rb);


#define ringbuffer_available_len(rb) ((rb)->buffer_size - ringbuffer_data_len(rb))
#ifdef __cplusplus

}
#endif


#endif /* __RINGBUFFER_H__ */