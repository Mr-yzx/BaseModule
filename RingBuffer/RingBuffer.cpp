#include "RingBuffer.h"

static ringbuffer_status ringbuffer_status_check(ringbuffer *rb)
{
    if (rb->read_index == rb->write_index)
    {
        if (rb->read_mirror == rb->write_mirror)
        {
            return RINGBUFFER_EMPTY;
        }
        else
        {
            return RINGBUFFER_FULL;
        }
    }
    return RINGBUFFER_NOT_FULL_AND_NOT_EMPTY;
}

ringbuffer *ringbuffer_create(uint32_t length)
{
    ringbuffer *rb = nullptr;
    uint8_t *pool = nullptr;

    assert(length > 0);

    length = ALIGN_DOWN(length, DEFAULT_ALIGN_SIZE);

    rb = (ringbuffer *)malloc(sizeof(ringbuffer));
    if (rb == nullptr)
    {
        goto exit;
    }

    pool = (uint8_t *)malloc(length);
    if (pool == nullptr)
    {
        free(rb);
        rb = nullptr;
        goto exit;
    }

    ringbuffer_init(rb, pool, length);


exit:
    return rb;
}

void ringbuffer_destroy(ringbuffer *rb)
{
    assert(rb != nullptr);

    free(rb->buffer);
    free(rb);
}

void ringbuffer_init(ringbuffer *rb, uint8_t *buffer, uint32_t length)
{
    assert(rb != nullptr);
    assert(length > 0);

    rb->read_mirror = rb->read_index = 0;
    rb->write_mirror = rb->write_index = 0;

    rb->buffer = buffer;
    rb->buffer_size = ALIGN_DOWN(length, DEFAULT_ALIGN_SIZE);

}

void ringbuffer_reset(ringbuffer *rb)
{
    assert(rb != nullptr);

    rb->read_mirror = 0;
    rb->read_index = 0;
    rb->write_mirror = 0;
    rb->write_index = 0;
}

uint32_t ringbuffer_put(ringbuffer *rb, uint8_t *data, uint32_t length)
{
    assert(rb != nullptr);

    uint32_t available_len = ringbuffer_available_len(rb);

    if (available_len == 0)
    {
        return 0;
    }

    if (available_len < length)
    {
        length = available_len;
    }

    if (rb->buffer_size - rb->write_index > length)
    {
        memcpy(&rb->buffer[rb->write_index], data, length);
        rb->write_index += length;
        return length;
    }

    memcpy(&rb->buffer[rb->write_index], data, rb->buffer_size - rb->write_index);
    memcpy(&rb->buffer[0], &data[rb->buffer_size - rb->write_index], length - (rb->buffer_size - rb->write_index));

    rb->write_mirror = ~rb->write_mirror;

    rb->write_index = length - (rb->buffer_size - rb->write_index);

    return length;
}

uint32_t ringbuffer_put_force(ringbuffer *rb, uint8_t *data, uint32_t length)
{
    assert(rb != nullptr);

    uint32_t space_len = ringbuffer_available_len(rb);

    if (length > rb->buffer_size)
    {
        data = &data[length - rb->buffer_size];
        length = rb->buffer_size;
    }

    if (rb->buffer_size - rb->write_index > length)
    {
        memcpy(&rb->buffer[rb->write_index], data, length);

        rb->write_index += length;

        if (length > space_len)
        {
            rb->read_index = rb->write_index;
        }
        return length;
    }

    memcpy(&rb->buffer[rb->write_index], data, rb->buffer_size - rb->write_index);

    memcpy(&rb->buffer[0], &data[rb->buffer_size - rb->write_index], length - (rb->buffer_size - rb->write_index));

    rb->write_mirror = ~rb->write_mirror;
    rb->write_index = length - (rb->buffer_size - rb->write_index);

    if (length > space_len)
    {
        if (rb->write_index <= rb->read_index)
        {
            rb->read_mirror = ~rb->read_mirror;
        }
        rb->read_index = rb->write_index;
    }
    return length;
}

uint32_t ringbuffer_get(ringbuffer *rb, uint8_t *data, uint32_t length)
{
    assert(rb != nullptr);

    uint32_t data_length = ringbuffer_data_len(rb);

    if (data_length == 0)
    {
        return 0;
    }
    if (data_length < length)
    {
        length = data_length;
    }

    if (rb->buffer_size - rb->read_index > length)
    {
        memcpy(data, &rb->buffer[rb->read_index], length);
        rb->read_index += length;
        return length;
    }

    memcpy(&data[0], &rb->buffer[rb->read_index], rb->buffer_size - rb->read_index);

    memcpy(&data[rb->buffer_size - rb->read_index], &rb->buffer[0], length - (rb->buffer_size - rb->read_index));

    rb->read_mirror = ~rb->read_mirror;
    rb->read_index = length - (rb->buffer_size - rb->read_index);

    return length;
}

uint32_t ringbuffer_peek(ringbuffer *rb, uint8_t **ptr)
{
    assert(rb != nullptr);

    *ptr = nullptr;

    uint32_t data_length = ringbuffer_data_len(rb);

    if (data_length == 0)
    {
        return 0;
    }

    *ptr = &rb->buffer[rb->read_index];

    if ((rb->buffer_size - rb->read_index) > data_length)
    {
        rb->read_index += data_length;
        return data_length;
    }

    data_length = rb->buffer_size - rb->read_index;

    rb->read_mirror = ~rb->read_mirror;
    rb->read_index = 0;

    return data_length;

}

uint32_t ringbuffer_data_len(ringbuffer *rb)
{
    switch (ringbuffer_status_check(rb))
    {
        case RINGBUFFER_EMPTY:
            return 0;
        case RINGBUFFER_FULL:
            return rb->buffer_size;
        case RINGBUFFER_NOT_FULL_AND_NOT_EMPTY:
        
        default:
        {
            uint32_t write_idx = rb->write_index, read_rdx = rb->read_index;

            if (write_idx > read_rdx)
            {
                return write_idx - read_rdx;
            }
            else
            {
                return rb->buffer_size - (read_rdx - write_idx);
            }
        }
    }
}

uint32_t ringbuffer_get_size(ringbuffer *rb)
{
    assert(rb != nullptr);
    return rb->buffer_size;
}
