import sys
import matplotlib.pyplot as plt
import csv

with open('putty.log', 'r') as f:
  reader = csv.reader(f)
  your_list = list( reader)

l = []
for v in your_list:
  try:
    l.append( int( v[ 0]) * 51 / 167000)
  except:
    print( sys.exc_info())
    

plt.plot(l)
plt.show()