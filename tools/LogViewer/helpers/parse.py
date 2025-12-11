import os, timeit

#cache_file_path = '/home/debiprasanna/ucc/src/L1DCache0'
cache_file_path = '/home/debiprasanna/ucc/src/L20'

def load_data():

    f =  open(cache_file_path,"r") 

    cyc_data = []
    print_=True

    line_num = 0

    for line in f:
        line = line.strip()
        cyc_num_start_index = line.find('Cycle$') + 6
        cyc_num_end_index = line.find('\t',cyc_num_start_index)
    
        cyc_num = int(line[cyc_num_start_index:cyc_num_end_index])

        print("Loading data for cycle number : ",cyc_num, " ,Line number: ", line_num)
        if cyc_num != line_num:
            print("Cycle Number is not equal to Line number at ", line_num)
            break

        line_num +=1

        curr_cyc_data = {}
        
        line = line[cyc_num_end_index:].strip()

        while len(line)>0:
            curr_dollar = line.find('$')
            token = line[:curr_dollar].strip()
            next_dollar = line.find('$', curr_dollar+1)
            if next_dollar!=-1:
                last_space = line[:next_dollar].rfind('\t')
                curr_line = line[curr_dollar+1:last_space].strip()
                line = line[last_space:].strip()
            else:
                curr_line = line[curr_dollar+1:].strip()
                line = ""

            all_queues = {}
            
            while len(curr_line)>0:
                queue_data = {}
                name_end = curr_line.find('(')
                name = curr_line[:name_end]
                curr_line = curr_line[name_end:]

                m = int(curr_line[curr_line.find('M')+2:curr_line.find('~')].strip())
                curr_line = curr_line[curr_line.find('~')+1:]

                a = int(curr_line[curr_line.find('A')+2:curr_line.find('~')].strip())
                curr_line = curr_line[curr_line.find('~')+1:]

                c = int(curr_line[curr_line.find('C')+2:curr_line.find(')')].strip())
                curr_line = curr_line[curr_line.find(':')+1:]

                queue_data["M"] = m
                queue_data["A"] = a
                queue_data["C"] = c

                packets=[]

                for i in range(m):
                    packID = curr_line[:curr_line.find(',')].strip()
                    curr_line = curr_line[curr_line.find(',')+1:].strip()
                    packets.append(packID)

                    if len(curr_line)>0  and curr_line[0]==';':
                        if len(curr_line) > 1:
                            curr_line = curr_line[1:]
                        else:
                            curr_line= ""

                queue_data["Packets"] = packets
                all_queues[name] = queue_data

            if token in curr_cyc_data.keys():
                for queue in all_queues:
                    curr_cyc_data[token][queue] = all_queues[queue]
            else:
                curr_cyc_data[token]= all_queues
            
        cyc_data.append(curr_cyc_data)

        os.system('clear')

    return cyc_data

def parse_data():
    start_t = timeit.default_timer()
    data = load_data()
    end_t = timeit.default_timer()
    print("Time Required to load data: ", (end_t-start_t))
    return data

if __name__ == "__main__":
    data = parse_data()
