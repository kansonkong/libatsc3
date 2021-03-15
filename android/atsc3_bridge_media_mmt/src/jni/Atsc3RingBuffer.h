
#include <string.h>
#include <sys/types.h>
#include <vector>
#include <list>

using namespace std;

#ifndef LIBATSC3_RING_BUFFER_H
#define LIBATSC3_RING_BUFFER_H

class Atsc3RingBuffer
{
public:
    static const int8_t RING_BUFFER_PAGE_INIT = 1;
    static const int8_t RING_BUFFER_PAGE_FRAGMENT = 2;

private:
    uint8_t*    buffer_ptr = nullptr;
    uint32_t    buffer_size = 0;
    uint32_t    buffer_page_size = 0;

    uint32_t    buffer_page_number = 0;
    uint32_t    buffer_position = 0;

public:
    Atsc3RingBuffer(uint8_t* buffer_ptr, uint32_t buffer_size, uint32_t page_size);

    void write(int8_t type, uint16_t service_id, uint16_t packet_id, uint32_t sample_number, uint64_t presentationUs, uint8_t* buffer, uint32_t bufferLen);

    uint32_t getCurrentPosition();
    uint32_t getCurrentPageNumber();
};

#endif //LIBATSC3_RING_BUFFER_H