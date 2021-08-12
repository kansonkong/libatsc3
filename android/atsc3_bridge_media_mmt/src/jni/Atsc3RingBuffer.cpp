#include "Atsc3RingBuffer.h"

#pragma pack(push, 1)
typedef struct {
    int8_t page_lock;
    int32_t page_num;
    int32_t length;
    int8_t  page_type;
    int32_t service_id;
    int32_t packet_id;
    int32_t sample_number;
    int64_t presentationUs;
    int8_t  reserved[7];
} RingBufferPageHeader;
#pragma pack(pop)

mutex Atsc3RingBuffer::CS_global_mutex;

Atsc3RingBuffer::Atsc3RingBuffer(uint8_t* buffer_ptr, uint32_t buffer_size, uint32_t page_size) {
    this->buffer_ptr = buffer_ptr;
    this->buffer_size = buffer_size;
    this->buffer_page_size = page_size;
}

void Atsc3RingBuffer::write(int8_t type, uint16_t service_id, uint16_t packet_id, uint32_t sample_number, uint64_t presentationUs, uint8_t *buffer, uint32_t bufferLen) {
    lock_guard<mutex> atsc3_ring_buffer_cctor_mutex_local(Atsc3RingBuffer::CS_global_mutex);

    uint32_t remaining = bufferLen;
    int32_t page_num = ++buffer_page_number;
    uint8_t page_segment_number = 0;

    while (remaining > 0) {
        if (buffer_position >= buffer_size) {
            buffer_position = 0;
        }

        uint8_t *fragmentBuffer = buffer_ptr + buffer_position;

        // lock package
        RingBufferPageHeader *header = ((RingBufferPageHeader *) fragmentBuffer);
        header->page_lock = 1;
        header->page_num = page_num;
        header->length = bufferLen;
        header->page_type = type;
        header->service_id = service_id;
        header->packet_id = packet_id;
        header->sample_number = sample_number;
        header->presentationUs = presentationUs;
        header->reserved[0] = page_segment_number++;

        uint8_t *fragmentBufferData = fragmentBuffer + sizeof(RingBufferPageHeader);
        uint32_t bytes_to_read = min(remaining, (uint32_t) (buffer_page_size - sizeof(RingBufferPageHeader)));
        memcpy(fragmentBufferData, buffer, bytes_to_read);

        buffer += bytes_to_read;
        remaining -= bytes_to_read;
        buffer_position += buffer_page_size;

        header->page_lock = 0;
    }
}

void Atsc3RingBuffer::rewind() {
    lock_guard<mutex> atsc3_ring_buffer_cctor_mutex_local(Atsc3RingBuffer::CS_global_mutex);

    buffer_position = 0;
    memset(buffer_ptr, 0, sizeof(RingBufferPageHeader));
}
