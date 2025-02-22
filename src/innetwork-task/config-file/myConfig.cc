#include "myConfig.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <sstream>
#include <vector>
#include <algorithm>

using boost::property_tree::ptree;

// Definitions of static members
std::vector<uint16_t> MyConfig::m_iterationNumbers;
std::vector<uint32_t> MyConfig::m_bufferSizes;
std::vector<double> MyConfig::m_lossRates;
double MyConfig::m_currentLossRate = 0.0;

std::vector<std::string> MyConfig::m_congestionControls;
std::string MyConfig::m_congestionControl;

uint32_t MyConfig::m_vectorSize = 150;

std::vector<std::string> MyConfig::m_topoTypes;
std::vector<uint16_t> MyConfig::m_topoScales;
std::vector<uint16_t> MyConfig::m_constraints;

std::vector<uint16_t> MyConfig::m_appTbSizes;
uint16_t MyConfig::m_currentAppTbSize = 30;  // default if not specified

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
    
    // Load lossRates as a vector (comma separated)
    std::string lossRateStr = pt.get<std::string>("Simulation.lossRate");
    std::istringstream ssLoss(lossRateStr);
    double lr;
    while (ssLoss >> lr) {
      m_lossRates.push_back(lr);
      if (ssLoss.peek() == ',')
          ssLoss.ignore();
    }
    if (!m_lossRates.empty())
      m_currentLossRate = m_lossRates[0];

    // Load congestion controls (comma separated)
    std::string ccStr = pt.get<std::string>("Simulation.congestionControl", "ns3::TcpAIMD");
    m_congestionControls.clear();
    std::istringstream ssCC(ccStr);
    std::string token;
    while (std::getline(ssCC, token, ',')) {
        token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
        if (!token.empty())
            m_congestionControls.push_back(token);
    }
    // Set current cc from first element.
    if (!m_congestionControls.empty())
        m_congestionControl = m_congestionControls[0];

    // Load vector size
    m_vectorSize = pt.get<uint32_t>("Simulation.vectorSize", 150);

    // Load appTbSize (iteration threshold) as a vector.
    std::string appTbSizeStr = pt.get<std::string>("Simulation.appTbSize", "30");
    std::istringstream ssAppTb(appTbSizeStr);
    uint16_t tb;
    while (ssAppTb >> tb) {
        m_appTbSizes.push_back(tb);
        if (ssAppTb.peek() == ',')
            ssAppTb.ignore();
    }
    if (!m_appTbSizes.empty())
        m_currentAppTbSize = m_appTbSizes[0];

    // Load topology parameters from [Topology] section
    std::string topoTypesStr = pt.get<std::string>("Topology.topoTypes", "");
    std::istringstream ssTypes(topoTypesStr);
    std::string typeToken;
    while (std::getline(ssTypes, typeToken, ',')) {
        typeToken.erase(remove_if(typeToken.begin(), typeToken.end(), ::isspace), typeToken.end());
        std::transform(typeToken.begin(), typeToken.end(), typeToken.begin(), ::tolower);
        if (!typeToken.empty())
            m_topoTypes.push_back(typeToken);
    }
    
    std::string topoScalesStr = pt.get<std::string>("Topology.topoScales", "");
    std::istringstream ssScales(topoScalesStr);
    uint16_t scale;
    while (ssScales >> scale) {
      m_topoScales.push_back(scale);
      if (ssScales.peek() == ',')
          ssScales.ignore();
    }
    
    // Load optional constrain values from Topology section.
    std::string constrainStr = pt.get<std::string>("Topology.constrain", "");
    if (!constrainStr.empty()) {
        std::istringstream ssConstrain(constrainStr);
        while (ssConstrain.good()) {
            uint16_t c;
            ssConstrain >> c;
            if (!ssConstrain.fail())
                m_constraints.push_back(c);
            if (ssConstrain.peek() == ',')
                ssConstrain.ignore();
        }
    }
}

const std::vector<uint16_t>& MyConfig::GetIterationNumbers() {
    return m_iterationNumbers;
}

const std::vector<uint32_t>& MyConfig::GetBufferSizes() {
    return m_bufferSizes;
}

const std::vector<double>& MyConfig::GetLossRates() {
    return m_lossRates;
}

void MyConfig::SetCurrentLossRate(double lr) {
    m_currentLossRate = lr;
}

double MyConfig::GetLossRate() {
    return m_currentLossRate;
}

const std::vector<std::string>& MyConfig::GetCongestionControls() {
    return m_congestionControls;
}

void MyConfig::SetCongestionControl(const std::string &cc) {
    m_congestionControl = cc;
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
            std::string base = type + "-" + std::to_string(scale);
            prefixes.push_back(base); // basic filename, e.g., "isp-50"
            // If constraints exist, generate additional prefixes with "-c<value>" suffix.
            for (auto constr : m_constraints) {
                prefixes.push_back(base + "-c" + std::to_string(constr));
            }
        }
    }
    return prefixes;
}

const std::vector<uint16_t>& MyConfig::GetAppTbSizes() {
    return m_appTbSizes;
}

void MyConfig::SetCurrentAppTbSize(uint16_t size) {
    m_currentAppTbSize = size;
}

uint16_t MyConfig::GetAppTbSize() {
    return m_currentAppTbSize;
}