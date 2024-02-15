import math 

def degrees_to_mils(degrees):
    return degrees * 18.52 

# Define the Range Table data as a list of lists
range_table = [
    [9, 100],
    [18.6, 200],
    [29.1, 300],
    [40.4, 400],
    [62.6, 500],
    [66.7, 600],
    [80.3, 700],
    [96, 800],
    [113.1, 900],
    [131.9, 1000],
    [152.5, 1100],
    [175.2, 1200],
    [200.4, 1300],
    [228.6, 1400],
    [260.3, 1500],
    [296.7, 1600],
    [339.2, 1700],
    [390.8, 1800],
    [468.3, 1900],
    [570.1, 2000],
    
    # ... (Add the remaining rows of your table here)
]

# Get angle from sensor (Placeholder until sensor reading implemented)
angle_degrees = 45  

# Convert to mils 
mils = degrees_to_mils(angle_degrees)

# Find nearest mils (Assumes "Mils" are sorted and mils is the first column)
closest_mils = min(row[0] for row in range_table) 
 # Find min based on first value in each row


# Find corresponding distance (Assuming distance is in the second column)
for row in range_table:
    if row[0] == closest_mils:
        distance = row[1]
        break

print("Distance:", distance) 
