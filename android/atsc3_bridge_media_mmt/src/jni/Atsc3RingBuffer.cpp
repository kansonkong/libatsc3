#include "Atsc3RingBuffer.h"

#pragma pack(push, 1)
typedef struct {
    int8_t  page_lock;
    int32_t page_num;
    int32_t segment_num;
    int32_t payload_length;
    int8_t  page_type;
    int32_t header_length;
} RingBufferPageHeader;
#pragma pack(pop)

mutex Atsc3RingBuffer::CS_global_mutex;

Atsc3RingBuffer::Atsc3RingBuffer(uint8_t* buffer_ptr, uint32_t buffer_size, uint32_t page_size) {
    this->buffer_ptr = buffer_ptr;
    this->buffer_size = buffer_size;
    this->buffer_page_size = page_size;
}

void Atsc3RingBuffer::write(int8_t type, uint16_t service_id, uint16_t packet_id, uint32_t sequence_number, uint32_t sample_number, uint64_t presentationUs, uint8_t *buffer, uint32_t bufferLen) {
    pb::MmtFragmentHeader fragmentHeader;
    fragmentHeader.set_service_id(service_id);
    fragmentHeader.set_packet_id(packet_id);
    fragmentHeader.set_sequence_number(sequence_number);
    fragmentHeader.set_sample_number(sample_number);
    fragmentHeader.set_presentationus(presentationUs);

    write(type, fragmentHeader, buffer, bufferLen);
}

void Atsc3RingBuffer::write(int8_t type, const pb::MmtFragmentHeader& fragmentHeader, uint8_t *buffer, uint32_t bufferLen) {
    lock_guard<mutex> atsc3_ring_buffer_cctor_mutex_local(Atsc3RingBuffer::CS_global_mutex);

    uint32_t remaining = bufferLen;
    int32_t page_num = ++buffer_page_number;
    int32_t page_segment_number = 0;

    size_t fragmentHeaderSize = fragmentHeader.ByteSizeLong();

    while (remaining > 0) {
        if (buffer_position >= buffer_size) {
            buffer_position = 0;
        }

        uint8_t *fragmentBuffer = buffer_ptr + buffer_position;

        RingBufferPageHeader *header = ((RingBufferPageHeader *) fragmentBuffer);
        header->page_lock = 1; // lock package
        header->page_num = page_num;
        header->segment_num = page_segment_number++;
        header->payload_length = bufferLen;
        header->page_type = type;
        header->header_length = fragmentHeaderSize;

        uint8_t *fragmentHeaderBuffer = fragmentBuffer + sizeof(RingBufferPageHeader);
        if (fragmentHeaderSize > 0) {
            fragmentHeader.SerializeToArray(fragmentHeaderBuffer, fragmentHeaderSize);
        }

        uint8_t *fragmentBufferData = fragmentHeaderBuffer + fragmentHeaderSize;
        uint32_t bytes_to_read = min(remaining, (uint32_t) (buffer_page_size - sizeof(RingBufferPageHeader) - fragmentHeaderSize));
        memcpy(fragmentBufferData, buffer, bytes_to_read);

        buffer += bytes_to_read;
        remaining -= bytes_to_read;
        buffer_position += buffer_page_size;

        header->page_lock = 0; // unlock package

        // skip fragment header for subsequent pages
        fragmentHeaderSize = 0;
    }
}

void Atsc3RingBuffer::reset() {
    lock_guard<mutex> atsc3_ring_buffer_cctor_mutex_local(Atsc3RingBuffer::CS_global_mutex);

    buffer_page_number = 0;
    buffer_position = 0;
    memset(buffer_ptr, 0, buffer_size);
}
