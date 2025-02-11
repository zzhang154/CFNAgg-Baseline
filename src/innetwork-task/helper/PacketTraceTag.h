#ifndef PACKET_TRACE_TAG_H
#define PACKET_TRACE_TAG_H

// Include the NS-3 core module header which brings in core definitions including time.
#include <ns3/core-module.h>
#include <ns3/tag.h>
#include <ns3/packet.h>
#include <ns3/ipv4-address.h>

// Include helper headers for node name conversion functions.
#include "parameter.h" // Provides GetNodeNameFromIp()
#include "utils.h"     // Provides Ipv4AddressToString()

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <iomanip>

namespace ns3 {

/**
 * \brief A custom packet tag for tracking packet traversal.
 *
 * This tag attaches an iteration number and a trace of entries, where each entry
 * records:
 *   - the node's IPv4 address,
 *   - the time when the packet passed that node, and
 *   - the node's name.
 *
 * This is useful for tracking the complete path of a packet with both timing
 * information and human-readable node identifiers.
 *
 * The entire implementation is contained in this header file.
 */
class PacketTraceTag : public Tag
{
public:
  /**
   * \brief Structure representing one trace entry.
   */
  struct TraceEntry {
    Ipv4Address addr;     ///< The IPv4 address of the node.
    Time t;               ///< The time when the packet passed the node.
    std::string nodeName; ///< The node's name obtained from its IP.
  };

  /**
   * \brief Constructor initializes the iteration counter to zero.
   */
  PacketTraceTag () : m_iteration (0) {}

  /**
   * \brief Get the TypeId for this class.
   * \return the TypeId.
   */
  static TypeId GetTypeId (void)
  {
    static TypeId tid = TypeId ("ns3::PacketTraceTag")
      .SetParent<Tag> ()
      .AddConstructor<PacketTraceTag> ();
    return tid;
  }
  
  /**
   * \brief Get the instance TypeId.
   * \return the instance's TypeId.
   */
  virtual TypeId GetInstanceTypeId (void) const { return GetTypeId (); }
  
  /**
   * \brief Get the serialized size in bytes of the tag.
   *
   * The serialized format consists of:
   * - iteration number: 4 bytes
   * - number of trace entries: 4 bytes
   * - for each trace entry:
   *     - IPv4 address: 4 bytes
   *     - timestamp: 8 bytes
   *     - string length: 4 bytes
   *     - node name string: (string length) bytes
   *
   * \return the total size in bytes.
   */
  virtual uint32_t GetSerializedSize (void) const
  {
    uint32_t size = 4 /* iteration */ + 4 /* number of entries */;
    for (std::vector<TraceEntry>::const_iterator it = m_trace.begin (); it != m_trace.end (); ++it)
      {
        size += 4;  // IPv4 address (uint32_t)
        size += 8;  // timestamp (double)
        size += 4;  // length of node name string
        size += it->nodeName.size (); // node name string bytes
      }
    return size;
  }
  
  /**
   * \brief Serialize the tag onto a buffer.
   *
   * \param i The TagBuffer to serialize into.
   */
  virtual void Serialize (TagBuffer i) const
  {
    // Write the iteration counter.
    i.WriteU32 (m_iteration);

    // Write the number of trace entries.
    uint32_t numEntries = m_trace.size ();
    i.WriteU32 (numEntries);

    // Write each trace entry.
    for (std::vector<TraceEntry>::const_iterator it = m_trace.begin (); it != m_trace.end (); ++it)
      {
        // Write the IPv4 address as a uint32_t.
        i.WriteU32 (it->addr.Get ());
        // Write the timestamp in seconds as a double.
        i.WriteDouble (it->t.GetSeconds ());
        // Write the length of the node name string.
        uint32_t strLen = it->nodeName.size ();
        i.WriteU32 (strLen);
        // Write the string bytes (if any).
        if (strLen > 0)
          {
            i.Write ((uint8_t*) it->nodeName.data (), strLen);
          }
      }
  }
  
  /**
   * \brief Deserialize the tag from a buffer.
   *
   * \param i The TagBuffer to deserialize from.
   */
  virtual void Deserialize (TagBuffer i)
  {
    // Read the iteration counter.
    m_iteration = i.ReadU32 ();

    // Read the number of trace entries.
    uint32_t numEntries = i.ReadU32 ();
    m_trace.clear ();
    for (uint32_t j = 0; j < numEntries; ++j)
      {
        TraceEntry entry;
        // Read the IPv4 address.
        uint32_t ip = i.ReadU32 ();
        entry.addr = Ipv4Address (ip);
        // Read the timestamp.
        double seconds = i.ReadDouble ();
        entry.t = Seconds (seconds);
        // Read the string length for the node name.
        uint32_t strLen = i.ReadU32 ();
        // Read the node name string if available.
        if (strLen > 0)
          {
            char* buffer = new char[strLen];
            i.Read ((uint8_t*) buffer, strLen);
            entry.nodeName = std::string (buffer, strLen);
            delete[] buffer;
          }
        else
          {
            entry.nodeName = "";
          }
        m_trace.push_back (entry);
      }
  }
  
  /**
   * \brief Print the tag information.
   *
   * \param os The output stream.
   */
  virtual void Print (std::ostream &os) const
  {
    os << "\nIteration: " << m_iteration << " Trace:\n";
    for (std::vector<TraceEntry>::const_iterator it = m_trace.begin (); it != m_trace.end (); ++it)
      {
        os << it->nodeName << "(" << it->addr << ")@"
          << std::fixed << std::setprecision(6) << it->t.GetSeconds () << "s\n";
      }
  }
  
  /**
   * \brief Set the iteration number.
   *
   * \param iteration The iteration number to set.
   */
  void SetIteration (uint16_t iteration)
  {
    m_iteration = iteration;
  }
  
  /**
   * \brief Get the current iteration number.
   *
   * \return The iteration number.
   */
  uint32_t GetIteration () const
  {
    return m_iteration;
  }
  
  /**
   * \brief Add an entry to the trace.
   *
   * This function automatically computes the node name from the IP address using
   * the helper functions GetNodeNameFromIp (from parameter.h) and Ipv4AddressToString (from utils.h).
   *
   * \param addr The IPv4 address of the current node.
   * \param t The time when the packet passed the node.
   */
  void AddEntry (Ipv4Address addr, Time t)
  {
    TraceEntry entry;
    entry.addr = addr;
    entry.t = t;
    // Convert the IPv4 address to a string.
    std::string addrStr = Ipv4AddressToString (addr);
    // Retrieve the node name using the helper function.
    entry.nodeName = GetNodeNameFromIp (addrStr);
    m_trace.push_back (entry);
  }
  
  /**
   * \brief Get a constant reference to the trace vector.
   *
   * \return A constant reference to the vector containing trace entries.
   */
  const std::vector<TraceEntry> &GetTrace () const
  {
    return m_trace;
  }


private:
  uint16_t m_iteration;               ///< Iteration number for grouping packets.
  std::vector<TraceEntry> m_trace;     ///< Vector of trace entries.
}; // class PacketTraceTag

/**
 * \brief Overload of operator<< to support streaming a PacketTraceTag object.
 *
 * This allows you to use NS_LOG_INFO() or std::clog << tag, where the tag is an instance of PacketTraceTag.
 *
 * \param os The output stream.
 * \param tag The PacketTraceTag object.
 * \return The output stream.
 */
inline std::ostream& operator<<(std::ostream& os, const PacketTraceTag& tag)
{
  tag.Print(os);
  return os;
}

} // namespace ns3

#endif // PACKET_TRACE_TAG_H