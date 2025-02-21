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

  // Updated loss rate: now a vector, with a current value.
  static const std::vector<double>& GetLossRates();
  static void SetCurrentLossRate(double lr);
  static double GetLossRate(); // returns the current loss rate

  // Change: new getter returning vector of congestion control strings.
  static const std::vector<std::string>& GetCongestionControls();
  static void SetCongestionControl(const std::string &cc);
  static std::string GetCongestionControl();

  // New getter for vector size.
  static uint32_t GetVectorSize();

  // New getters for topology file prefix generation:
  static const std::vector<std::string>& GetTopoTypes();
  static const std::vector<uint16_t>& GetTopoScales();
  // Generate final file name prefixes based on topo types, scales, and constraints.
  static std::vector<std::string> GetFileNames();

private:
  // Existing members
  static std::vector<uint16_t> m_iterationNumbers;
  static std::vector<uint32_t> m_bufferSizes;
  static std::vector<double> m_lossRates;
  static double m_currentLossRate; // holds the current loss rate in simulation

  // Remove single cc and replace with a vector.
  static std::vector<std::string> m_congestionControls;
  static std::string m_congestionControl;  // can serve as current cc

  // New member for vector size.
  static uint32_t m_vectorSize;

  // New members for topology parameters
  static std::vector<std::string> m_topoTypes;
  static std::vector<uint16_t> m_topoScales;

  // New member: constraints, if provided.
  static std::vector<uint16_t> m_constraints;
};

#endif // MYCONFIG_H