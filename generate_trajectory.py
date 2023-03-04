import csv
import numpy as np
import math

with open('traj.csv', 'w', newline='') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=';', quotechar='|', quoting=csv.QUOTE_MINIMAL)
    
    angle = 0.0
    spamwriter.writerow(['X', 'Y', 'Z'])
    while(angle < 6.28):
        x = 0.5 * math.sin(angle) + 1
        y = 0.5 * math.cos(angle) + 1
        z = 1 #+ angle / 6.28
        spamwriter.writerow([x, y, z])
        angle = angle + 0.05