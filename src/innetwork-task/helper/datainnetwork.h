#ifndef INNETWORK_TASK_DATAINNETWORK_H  
#define INNETWORK_TASK_DATAINNETWORK_H  
  
#include <vector>  
#include <cstdint>  
#include <random>  
#include <chrono>  
#include <ctime>
  
class DataInNetwork {  
private:  
    std::vector<int64_t> data;  
  
public:  
    // 构造函数  
    DataInNetwork();  
  
    // 析构函数（可选，如果类中没有动态分配内存，则不需要显式定义）  
    // ~DataInNetwork();  
  
    // 求和函数  
    void sum(std::vector<int64_t> &datasum, std::vector<int64_t> &vec1, std::vector<int64_t> &vec2) const;  
  
    // 求平均值函数  
    double avg(std::vector<int64_t> &dataavg, int8_t count) const;  
  
    // 生成函数  
    void generate(std::vector<int64_t> &vec);  
  
    // 拷贝构造函数（可选，现代C++编译器通常会为你生成）  
    // DataInNetwork(const DataInNetwork& other);  
  
    // 赋值运算符（可选，同上）  
    // DataInNetwork& operator=(const DataInNetwork& other);  
  
    // 下标运算符  
    int64_t& operator[](size_t index);  
    const int64_t& operator[](size_t index) const;  
};  
  
#endif // INNETWORK_TASK_DATAINNETWORK_H