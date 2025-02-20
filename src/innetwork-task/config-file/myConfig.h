#ifndef MYCONFIG_H
#define MYCONFIG_H

#include <string>
#include <vector>

class MyConfig {
public:
  static void Load(const std::string &filename);

  // Existing getters
  static const std::vector<uint16_t>& GetIterationNumbers();
  static const std::vector<uint32_t>& GetBufferSizes();
  static double GetLossRate();
  static std::string GetCongestionControl();

  // New getter for vector size.
  static uint32_t GetVectorSize();

  // New getters for topology file prefix generation:
  static const std::vector<std::string>& GetTopoTypes();
  static const std::vector<uint16_t>& GetTopoScales();
  // Generate final file name prefixes based on topo types and scales.
  static std::vector<std::string> GetFileNames();

private:
  // Existing members
  static std::vector<uint16_t> m_iterationNumbers;
  static std::vector<uint32_t> m_bufferSizes;
  static double m_lossRate;
  static std::string m_congestionControl;

  // New member for vector size.
  static uint32_t m_vectorSize;

  // New members for topology parameters
  static std::vector<std::string> m_topoTypes;
  static std::vector<uint16_t> m_topoScales;
};

#endif // MYCONFIG_H