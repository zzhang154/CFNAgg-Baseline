#include "vectorop.h"

namespace ns3 {

void SumVector (uint64_t * sum, uint64_t * vec1, 
                    std::vector<uint64_t> &vec2){
    for (uint8_t i = 0; i < uint8_t(vec2. size ()); ++i) 
        sum[i] = vec1[i] + vec2[i];
                    }

void 
SumVector(std::vector<uint64_t> &sum, std::vector<uint64_t> &vec1, 
                std::vector<uint64_t> &vec2) {
    for (uint8_t i = 0; i < uint8_t(sum. size ()); ++i) 
        sum[i] = vec1[i] + vec2[i];

}


void AvgVector (uint64_t * sum, uint8_t count){
    for (uint16_t i = 0; i < static_cast<uint16_t>(BASESIZE / 8); ++i)
        sum[i] /= count;
}


void 
AvgVector (std::vector<uint64_t> &sum, uint8_t count) {
    for (uint8_t i = 0; i < sum. size (); ++i)
        sum[i] /= count;
}

void
GenetrateRandomVector (std::vector<uint64_t> &vec, uint8_t rank) {
    //uint16_t size = static_cast<uint16_t> (vec.size());
    // seed the random number generator
    std::srand(std::time (nullptr) + rank);
    
    // generate vSize random uint64_t integers
    for (int i = 0; i < static_cast<int> (vec.size()); ++i) 
        vec[i] = (static_cast<uint64_t> (std::rand ()) << 32) | std::rand ();

}

void SerializeVector (const std::vector<uint64_t> &vec, uint8_t *buffer){
    //int size = static_cast<int>(size);
    for (int i = 0; i < static_cast<int>(vec.size()); ++i) {
        for (int j = 0; j <8; ++j) {
            buffer[i*8+j] = static_cast<uint8_t>((vec[i] >> (8 * j)) & 0xff);
        }
    }
}


void DeserializeVector (std::vector<uint64_t> &vec, uint8_t *buffer, uint16_t bufferSize){
    int elementsCount = bufferSize/8;
    uint8_t *ptr = buffer;
    for (int i = 0; i < elementsCount; ++i) {
        uint64_t value = 0;
        for (int j = 0; j <8; ++j) {
            value |= static_cast<uint64_t>(*ptr++) << (8 * j);
        }
        vec[i]=value;
    }
}


void 
DeepVectorCopy(std::vector<Address> &vec1, 
                    const std::vector<Address> &vec2) {
    for (uint16_t i = 0; i < uint16_t (vec2. size ()); ++i) {
        vec1. push_back (vec2[i]);
    }
}

} /*namespace ns3*/