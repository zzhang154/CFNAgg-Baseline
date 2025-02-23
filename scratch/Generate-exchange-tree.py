import os
import random

def read_aggtree_file(filepath):
    with open(filepath, 'r') as file:
        lines = file.readlines()
    return lines

def parse_aggtree_lines(lines):
    aggs = {}
    other_lines = []
    for line in lines:
        if line.startswith('agg') and ':' in line:
            parts = line.split(':')
            agg_name = parts[0].strip()
            pros = parts[1].strip().split()
            if all(pro.startswith('pro') for pro in pros):
                aggs[agg_name] = pros
            else:
                other_lines.append(line)
        else:
            other_lines.append(line)
    return aggs, other_lines

def exchange_pros(aggs, percentage):
    agg_names = list(aggs.keys())
    pool = []
    num_pros_to_exchange = {}

    # 抽出每个 agg 的部分 pro 放入池子
    for agg_name in agg_names:
        pros = aggs[agg_name]
        num_pros = len(pros)
        num_pros_exchange = int(num_pros * percentage / 100)
        num_pros_to_exchange[agg_name] = num_pros_exchange

        if num_pros_exchange > 0:
            pros_to_exchange = [pros.pop(random.randint(0, len(pros) - 1)) for _ in range(num_pros_exchange)]
            pool.extend(pros_to_exchange)

    # 随机搅乱池子里的 pro
    random.shuffle(pool)

    # 按照原本的数量重新分配回给每个 agg
    for agg_name in agg_names:
        num_pros_exchange = num_pros_to_exchange[agg_name]
        if num_pros_exchange > 0:
            aggs[agg_name].extend(pool[:num_pros_exchange])
            pool = pool[num_pros_exchange:]

    return aggs

def write_aggtree_file(filepath, aggs, other_lines):
    with open(filepath, 'w') as file:
        for agg_name, pros in aggs.items():
            file.write(f"{agg_name}: {' '.join(pros)}\n")
        for line in other_lines:
            file.write(line)

def main():
    topo_types = ['isp', 'dcn']
    nums = [10, 50, 100, 200, 500, 1000]
    
    percentage = float(input("Enter the percentage of pros to exchange: "))
    
    for topo in topo_types:
        for num in nums:
            input_filepath = f"./data/aggtree/aggtree-{topo}-{num}.txt"
            output_filepath = f"./data/aggtree/aggtree-{topo}-exchange-{int(percentage)}percent-{num}.txt"
            
            if os.path.exists(input_filepath):
                lines = read_aggtree_file(input_filepath)
                aggs, other_lines = parse_aggtree_lines(lines)
                
                aggs = exchange_pros(aggs, percentage)
                
                write_aggtree_file(output_filepath, aggs, other_lines)
                print(f"Exchange file saved as {output_filepath}")

if __name__ == "__main__":
    main()