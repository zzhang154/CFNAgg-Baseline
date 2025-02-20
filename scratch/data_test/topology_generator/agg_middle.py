def generate_topology(num_forwarders_procon, num_aggregators):
    """
    Generate a topology file for ndnSIM with the specified number of forwarders
    directly connected to pro0 and con0 (equal on both sides), and a specified
    number of aggregators (all in the middle).
    
    Example (num_forwarders_procon=2, num_aggregators=6):
    pro0 -- f0 -- f1 -- agg0 -- agg1 -- agg2 -- agg3 -- agg4 -- agg5 -- f2 -- f3 -- con0
    
    :param num_forwarders_procon: Number of forwarders attached to pro0 AND con0 (both sides).
    :param num_aggregators: Number of aggregators to place in the middle.
    """
    # Prepare the node list starting with pro0
    all_nodes = ["pro0"]

    # Add forwarders for the pro0 side
    for i in range(num_forwarders_procon):
        all_nodes.append(f"f{i}")

    # Add aggregators
    for j in range(num_aggregators):
        all_nodes.append(f"agg{j}")

    # Add forwarders for the con0 side
    # Continue forwarder numbering for clarity
    for i in range(num_forwarders_procon):
        all_nodes.append(f"f{num_forwarders_procon + i}")

    # Finally, add con0
    all_nodes.append("con0")

    # Name the output file
    output_file = f"{num_forwarders_procon}-forwarders-each-side-{num_aggregators}-aggregators.txt"

    # Create the router section
    router_section = ["router\n\n"]
    for node in all_nodes:
        router_section.append(f"{node}\n")

    # Create the link section
    link_section = ["\nlink\n"]
    for i in range(len(all_nodes) - 1):
        # Link each adjacent pair in the chain
        link_section.append(f"{all_nodes[i]}    {all_nodes[i+1]}    50Mbps    1    1ms    5000\n")

    # Combine sections
    topology_content = "".join(router_section) + "".join(link_section)

    # Write to file
    with open(output_file, "w") as f:
        f.write(topology_content)

    print(f"Topology file '{output_file}' generated successfully!")

# Example usage:
if __name__ == "__main__":
    # Here, you can enter the number of forwarders on each side and the number of aggregators:
    fwd_procon_input = int(input("Enter the number of forwarders connected to pro0 and con0: "))
    agg_input = int(input("Enter the number of aggregators: "))
    generate_topology(fwd_procon_input, agg_input)