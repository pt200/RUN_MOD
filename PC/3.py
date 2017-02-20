from socket import *


s=socket(AF_INET, SOCK_DGRAM)
s.bind(('',49087))
while( 1):
  m=s.recv(1024)
  print( m)