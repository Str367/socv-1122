import random

# Number of lines to generate
num_lines = 2000
path = "socv-1122/design/SoCV/vending/input.pattern"

with open(path, 'w') as file:
    for _ in range(num_lines):
        # Generate five random numbers
        first_num = random.randint(0, 3)
        other_nums = [random.randint(0, 2) for _ in range(4)]
        
        # Format the line
        line = f"{first_num} {' '.join(map(str, other_nums))}\n"
        
        # Write the line to the file
        file.write(line)
