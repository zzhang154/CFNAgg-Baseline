# generate aggtree without aggregator

import os

def generate_aggtree(num):
    line = "con0: " + " ".join([f"pro{i}" for i in range(num)])
    return [line]

def save_to_file(topo, num, lines):
    directory = "./data/aggtreenoagg"
    if not os.path.exists(directory):
        os.makedirs(directory)
    filename = f"aggtree-{topo}-{num}.txt"
    filepath = os.path.join(directory, filename)
    with open(filepath, 'w') as f:
        for line in lines:
            f.write(line + "\n")

def main():
    topos = ["dcn", "isp"]
    nums = [10, 50, 100, 200, 500, 1000]
    
    for topo in topos:
        for num in nums:
            lines = generate_aggtree(num)
            save_to_file(topo, num, lines)

if __name__ == "__main__":
    main()