// datainnetwork.cc  
#include "datainnetwork.h"  
  
DataInNetwork::DataInNetwork() : data(1000, 0) {}  
  
// 如果析构函数、拷贝构造函数和赋值运算符是必需的，并且编译器生成的版本不适合你的需求，  
// 那么你需要在这里提供它们的实现。但在这个例子中，我们可以省略它们，因为默认的实现是足够的。  

// calculate the sum of 2 vectors  
void DataInNetwork::sum(std::vector<int64_t> &datasum, std::vector<int64_t> &vec1, std::vector<int64_t> &vec2) const {  
    for (uint8_t i = 0; i < uint8_t(datasum. size ()); ++i) 
        datasum[i] = vec1[i] + vec2[i];  
}  

// calculate the avg of count vectors' sum
double DataInNetwork::avg(std::vector<int64_t> &dataavg, int8_t count) const {  
    for (uint8_t i = 0; i < dataavg. size (); ++i)
        dataavg[i] /= count; 
}  

// generate vector with random values
void DataInNetwork::generate(std::vector<int64_t> &vec) {  
    // 获取当前时间作为种子  
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();  
  
    // 使用种子初始化随机数生成器  
    std::mt19937 rng(seed);  
  
    // 定义一个分布，用于生成int64_t范围内的随机数  
    // 这里我们简单地使用std::uniform_int_distribution生成一个尽可能大的随机范围  
    //std::uniform_int_distribution<int64_t> dist(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());  
    std::uniform_int_distribution<int64_t> dist(0, 1000000000000LL); // 生成0到1000000000000之间的随机数
    
    // 遍历vec向量，为每个元素生成一个随机数并赋值  
    for (auto& value : vec) {  
        value = dist(rng);  
    }  
}  
  
int64_t& DataInNetwork::operator[](size_t index) {  
    return data[index];  
}  
  
const int64_t& DataInNetwork::operator[](size_t index) const {  
    return data[index];  
}