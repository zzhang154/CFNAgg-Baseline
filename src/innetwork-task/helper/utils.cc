#include "utils.h"
#include <cstring>
namespace ns3 {

NS_LOG_COMPONENT_DEFINE("TracedTimeQueue");

// Define fileMutex
std::mutex fileMutex;

void HelloUtils(){
    std::cout << "Hello from utils" << std::endl;
}

void WriteToFile(const std::string& filename, const std::string& result) {
    std::lock_guard<std::mutex> guard(fileMutex);  // Ensure thread-safe access
    std::ofstream outfile;

    // Open the file in append mode
    outfile.open(filename, std::ios_base::app);
    if (!outfile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Write the result to the file
    outfile << result << std::endl;

    // Close the file
    outfile.close();
}

void Addr2Str (Address addr, std::string &str) {
    std::stringstream ss;
    ss << Ipv4Address::ConvertFrom (addr);
    str = ss.str();
}

TracedTimeQueue::TracedTimeQueue()
{
    // Initialize the trace source
    m_tracedTimeQueue.ConnectWithoutContext(
        MakeCallback(&TracedTimeQueue::TimeQueueChanged, this));
}

void TracedTimeQueue::Push(Time t)
{
    m_times.push(t);
    m_tracedTimeQueue(m_times); // Trigger the trace source
}

void TracedTimeQueue::Pop()
{
    if (!m_times.empty())
    {
        m_times.pop();
        m_tracedTimeQueue(m_times); // Trigger the trace source
    }
}

size_t TracedTimeQueue::Size() const
{
    return m_times.size();
}

const std::queue<Time> &TracedTimeQueue::GetTimes() const
{
    return m_times;
}

void TracedTimeQueue::TimeQueueChanged(const std::queue<Time> &times)
{
    // This method will be called whenever the traced value changes
    NS_LOG_INFO( this << " Time queue changed. Current size: " << times.size());
}

void MyTimeQueueCallback(const std::queue<Time> &times)
{
    std::cout << "Time queue changed. New size: " << times.size() << std::endl;
}

void ValueChangedCallback(std::string context, uint32_t oldValue, uint32_t newValue) {
    std::cout << "Value changed from " << oldValue << " to " << newValue << " in context: " << context << std::endl;
}

void TimeChangedCallback(std::string context, Time oldValue, Time newValue) {
    std::cout << "Time changed from " << oldValue.GetSeconds() << " to " << newValue.GetSeconds() << " in context: " << context << std::endl;
}

// std::string Ipv4AddressToString(ns3::Ipv4Address ipv4Address)
// {
//     // Get the raw IPv4 address
//     uint32_t addr = ipv4Address.Get();

//     /*
//     // Convert the raw address to a string
//     std::string addressString = inet_ntoa(addr);
//     */

//     std::ostringstream oss;
//     oss << ((addr >> 24) & 0xFF) << "."
//         << ((addr >> 16) & 0xFF) << "."
//         << ((addr >> 8) & 0xFF) << "."
//         << (addr & 0xFF);
    
//     return oss.str();
// }


std::string Ipv4AddressToString(ns3::Ipv4Address ipv4Address){
    // In the relevant function:
    std::stringstream ss;
    ss << ipv4Address;
    return ss.str();
}

ns3::Ipv4Address GetIpAddrFromNode (Ptr<Node> node){
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
    Ipv4InterfaceAddress iaddr = ipv4->GetAddress (1, 0);
    Ipv4Address addr = iaddr.GetLocal ();
    return addr;
}

void EnableLoggingComponents() {
    LogComponentEnable("InnetworkAggregationInterface", LOG_LEVEL_ALL);
    LogComponentEnable("TCPclient", LOG_LEVEL_ALL);
    LogComponentEnable("TCPserver", LOG_LEVEL_ALL);
    LogComponentEnable("TcpSocketBase", LOG_LEVEL_ALL);
    LogComponentEnable("TcpRxBuffer", LOG_LEVEL_ALL);
    LogComponentEnable("TcpTxBuffer", LOG_LEVEL_ALL);
    LogComponentEnable("Packet", LOG_LEVEL_DEBUG);
}

void DisableLoggingComponents() {
    LogComponentDisable("InnetworkAggregationInterface", LOG_LEVEL_ALL);
    LogComponentDisable("TCPclient", LOG_LEVEL_ALL);
    LogComponentDisable("TCPserver", LOG_LEVEL_ALL);
    LogComponentDisable("TcpSocketBase", LOG_LEVEL_ALL);
    LogComponentDisable("TcpRxBuffer", LOG_LEVEL_ALL);
    LogComponentDisable("TcpTxBuffer", LOG_LEVEL_ALL);
    LogComponentDisable("Packet", LOG_LEVEL_DEBUG);
}

}; /* namespace ns3 */