#include "myConfig.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <sstream>
#include <vector>
#include <algorithm>

using boost::property_tree::ptree;

std::vector<uint16_t> MyConfig::m_iterationNumbers;
std::vector<uint32_t> MyConfig::m_bufferSizes;
double MyConfig::m_lossRate = 0.0;
std::string MyConfig::m_congestionControl;

// New member for vector size.
uint32_t MyConfig::m_vectorSize = 150;

// New members for topology parameters
std::vector<std::string> MyConfig::m_topoTypes;
std::vector<uint16_t> MyConfig::m_topoScales;

void MyConfig::Load(const std::string &filename) {
    ptree pt;
    boost::property_tree::ini_parser::read_ini(filename, pt);

    // Load iterationNumbers
    std::string itrStr = pt.get<std::string>("Simulation.iterationNumbers");
    std::istringstream ssItr(itrStr);
    uint16_t val;
    while (ssItr >> val) {
      m_iterationNumbers.push_back(val);
      if (ssItr.peek() == ',')
          ssItr.ignore();
    }
    
    // Load bufferSizes
    std::string bufStr = pt.get<std::string>("Simulation.bufferSizes");
    std::istringstream ssBuf(bufStr);
    uint32_t buf;
    while (ssBuf >> buf) {
      m_bufferSizes.push_back(buf);
      if (ssBuf.peek() == ',')
          ssBuf.ignore();
    }
    
    m_lossRate = pt.get<double>("Simulation.lossRate");
    m_congestionControl = pt.get<std::string>("Simulation.congestionControl");

    // Load vector size
    m_vectorSize = pt.get<uint32_t>("Simulation.vectorSize", 150);

    // Load new topology parameters from [Topology] section
    std::string topoTypesStr = pt.get<std::string>("Topology.topoTypes", "");
    std::istringstream ssTypes(topoTypesStr);
    std::string typeToken;
    while (std::getline(ssTypes, typeToken, ',')) {
        // trim and convert to lowercase
        typeToken.erase(remove_if(typeToken.begin(), typeToken.end(), ::isspace), typeToken.end());
        std::transform(typeToken.begin(), typeToken.end(), typeToken.begin(), ::tolower);
        if (!typeToken.empty()) {
            m_topoTypes.push_back(typeToken);
        }
    }
    
    std::string topoScalesStr = pt.get<std::string>("Topology.topoScales", "");
    std::istringstream ssScales(topoScalesStr);
    uint16_t scale;
    while (ssScales >> scale) {
      m_topoScales.push_back(scale);
      if (ssScales.peek() == ',')
          ssScales.ignore();
    }
}

const std::vector<uint16_t>& MyConfig::GetIterationNumbers() {
    return m_iterationNumbers;
}

const std::vector<uint32_t>& MyConfig::GetBufferSizes() {
    return m_bufferSizes;
}

double MyConfig::GetLossRate() {
    return m_lossRate;
}

std::string MyConfig::GetCongestionControl() {
    return m_congestionControl;
}

uint32_t MyConfig::GetVectorSize() {
    return m_vectorSize;
}

const std::vector<std::string>& MyConfig::GetTopoTypes() {
    return m_topoTypes;
}

const std::vector<uint16_t>& MyConfig::GetTopoScales() {
    return m_topoScales;
}

std::vector<std::string> MyConfig::GetFileNames() {
    std::vector<std::string> prefixes;
    // Generate cross product of topo types and scales.
    for (const auto &type : m_topoTypes) {
        for (auto scale : m_topoScales) {
            prefixes.push_back(type + "-" + std::to_string(scale));
        }
    }
    return prefixes;
}