def generate_topology(num_forwarders, num_aggregators):
    """
    Generate a topology file for ndnSIM with the specified number of forwarders per aggregator block
    and the specified number of aggregators.
    
    The chain will look like this (for num_forwarders=3, num_aggregators=2):
    pro0 -- f0 -- f1 -- f2 -- agg0 -- f3 -- f4 -- f5 -- agg1 -- f6 -- f7 -- f8 -- con0
    
    :param num_forwarders: Number of forwarders attached to each aggregator block.
    :param num_aggregators: Number of aggregators to insert.
    """
    # Calculate total number of forwarders
    # Each aggregator block has 'num_forwarders' forwarders, we have (num_aggregators + 1) blocks.
    total_forwarders = num_forwarders * (num_aggregators + 1)
    
    # Prepare the list of all nodes in the chain
    all_nodes = []
    
    # Start with the producer node
    all_nodes.append("pro0")
    
    forwarder_index = 0
    
    # Build forwarders and aggregators in a sequence
    for agg_id in range(num_aggregators + 1):
        # Add 'num_forwarders' forwarders
        for _ in range(num_forwarders):
            all_nodes.append(f"forwarder{forwarder_index}")
            forwarder_index += 1
        
        # Add aggregator if agg_id is not the last section
        if agg_id < num_aggregators:
            all_nodes.append(f"agg{agg_id}")
    
    # End with the consumer node
    all_nodes.append("con0")
    
    # File name for the output topology
    output_file = f"{num_forwarders}-forwarders-{num_aggregators}-aggregators.txt"
    
    # Create the router section
    router_section = ["router\n", "\n"]
    for node in all_nodes:
        router_section.append(f"{node}\n")
    
    # Create the link section
    link_section = ["\nlink\n"]
    
    # Link each adjacent pair
    for i in range(len(all_nodes) - 1):
        link_section.append(f"{all_nodes[i]}    {all_nodes[i+1]}    50Mbps    1    1ms    5000\n")
    
    # Combine sections
    topology_content = "".join(router_section) + "".join(link_section)
    
    # Write the topology to the output file
    with open(output_file, "w") as f:
        f.write(topology_content)
    
    print(f"Topology file '{output_file}' generated successfully!")

# Example usage:
if __name__ == "__main__":
    # Example input:
    # num_forwarders: number of forwarders per aggregator block
    # num_aggregators: number of aggregators in the chain
    fwd_input = int(input("Enter the number of forwarders per aggregator block: "))
    agg_input = int(input("Enter the number of aggregators: "))
    generate_topology(fwd_input, agg_input)