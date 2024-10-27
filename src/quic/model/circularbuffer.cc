#include "circularbuffer.h"

namespace ns3 {

    NS_LOG_COMPONENT_DEFINE ("CircularBuffer");
    NS_OBJECT_ENSURE_REGISTERED (CircularBuffer);

    TypeId 
    CircularBuffer::GetTypeId(void) {
        static TypeId tid = TypeId ("ns3::CircularBuffer")
            .SetParent<Object> ()
            .SetGroupName("quic")
            .AddConstructor<CircularBuffer> ()
        ;
        return tid;
    }

    // 构造函数
    CircularBuffer::CircularBuffer() {
        size_t capacity = pktlen * 20;
        init(capacity);
    }

    // 初始化缓冲区
    void CircularBuffer::init(size_t capacity) {
        this->capacity = capacity;
        this->size = 0;
        this->read_pos = 0;
        this->write_pos = 0;
        this->buffer = (uint8_t*)malloc(capacity);
        buffer[0] = 12;
        //std::cout<<"CircularBuffer::init(size_t capacity) done ----"<<buffer[0]<<std::endl;
    }


    void CircularBuffer::print(){
        uint8_t zero=0;
        std::cout<<"CircularBuffer::print--with--content-size---"<<this->size<<"-from-"<<this->read_pos<<"|---";
        for (size_t i=0;i<this->size;i++){
            if( i%(pktlen) == 0&& i!=0 )
                std::cout<<"--new-packet-"<<std::endl;
            std::cout<<this->buffer[(this->read_pos+i)%this->capacity]-zero<<"|";
        }
        std::cout<<"|"<<std::endl;    
    }

    // 析构函数
    CircularBuffer::~CircularBuffer() {
        free(this->buffer);
    }

    // 写入数据到缓冲区
    size_t CircularBuffer::write(uint8_t *data, size_t bytes) {
        // if (bytes > (this->capacity - this->size)) {
        //     bytes = this->capacity - this->size; // 防止溢出
        // }
        //NS_ASSERT (bytes <= (this->capacity - this->size));
        //NS_LOG_INFO ("write buffer");
        //bytes <= (this->capacity - this->size);
        size_t first_part = this->capacity - this->write_pos;
        if (bytes > first_part) {
            std::copy(data , data+first_part,this->buffer + this->write_pos);
            std::copy(data+first_part , data+bytes - first_part,this->buffer);
            //memcpy(this->buffer + this->write_pos, data, first_part);
            //memcpy(this->buffer, data + first_part, bytes - first_part);
            //this->write_pos = bytes - first_part;
            //NS_LOG_INFO ("memcpy 1 error");
        } else {
            std::copy(data , data+bytes,this->buffer+ this->write_pos);
            //memcpy(this->buffer + this->write_pos, data, bytes);
            //NS_LOG_INFO ("memcpy 2 error");
        }
        this->write_pos = (this->write_pos + bytes) % this->capacity;
        this->size += bytes;
        //std::cout<<"circularbuffer----writting------on---"<<this->write_pos<<std::endl;
        NS_LOG_INFO ("[CircularBuffer:write] buffer write end"<<size);
        return bytes;
    }

    // 从缓冲区读取数据
    size_t 
    CircularBuffer::read(uint8_t *dest, size_t bytes) {
        if (bytes > this->size) {
            bytes = this->size; // 防止读取超过现有数据
        }

        size_t first_part = this->capacity - this->read_pos;
        if (bytes > first_part) {
            std::copy(this->buffer + this->read_pos,this->buffer + this->capacity, dest);
            std::copy(this->buffer,this->buffer+bytes-first_part,dest+first_part);

            //memcpy(dest, this->buffer + this->read_pos, first_part);
            //memcpy(dest + first_part, this->buffer, bytes - first_part);
            this->read_pos = bytes - first_part;
        } else {
            std::copy(this->buffer + this->read_pos,this->buffer + this->read_pos+bytes,dest);
            //memcpy(dest, this->buffer + this->read_pos, bytes);
            this->read_pos = (this->read_pos + bytes) % this->capacity;
        }
        this->size -= bytes;
        //std::cout<<"circularbuffer----reading------on---"<<this->read_pos%(BASESIZE+3)<<std::endl;
        return bytes;
    }

    size_t 
    CircularBuffer::getSize () {
        return this->size;
    }

    uint8_t *
    CircularBuffer::getData () {
        return this->buffer;
    }

    uint16_t
    CircularBuffer::getHeadUint16 () {
        uint16_t dataLen = 0;
        std::memcpy (&dataLen, this->buffer + this->read_pos, sizeof (uint16_t));
        //NS_LOG_INFO ("[CircularBuffer:getHeadUint16] datalen = " << dataLen << "read_pos = " << this->read_pos);
        NS_ASSERT (dataLen <= this->capacity);
        return dataLen;
    }

    uint8_t CircularBuffer::getCapacity(){
        return buffer[10];
    }

    uint8_t CircularBuffer::getNextToRead(){
        return buffer[this->read_pos];
    }

    uint8_t* CircularBuffer::getNextToReadBuffer(){
        return buffer+(this->read_pos);
    }


}; /*namespace ns3*/