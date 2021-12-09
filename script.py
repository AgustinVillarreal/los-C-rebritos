import re
import os

ip_swap = input('Ingrese IP SWAP: ')

dir = './Memoria/config'
configs = os.listdir(dir)

for config in configs:

  regex = re.compile(r'\b[0-9]{0,3}\.[0-9]{0,3}\.[0-9]{0,3}\.[0-9]{0,3}\b')
  x = ''

  print('-> '+config)

  with open(dir+'/'+config, "rt") as file:
    x = file.read()
  
  with open(dir+'/'+config, "wt") as file:
    x = x.replace('IP_SWAP='+regex.search(x).group(0),'IP_SWAP='+ip_swap)
    file.write(x)

print('\n')
print('Ip cambiada\n\n')

ip_memoria = input('Ingrese IP MEMORIA: ')

dir = './Kernel/config'
configs = os.listdir(dir)

for config in configs:

  regex = re.compile(r'\b[0-9]{0,3}\.[0-9]{0,3}\.[0-9]{0,3}\.[0-9]{0,3}\b')
  x = ''

  print('-> '+config)

  with open(dir+'/'+config, "rt") as file:
    x = file.read()
  
  with open(dir+'/'+config, "wt") as file:
    x = x.replace('IP_MEMORIA='+regex.search(x).group(0),'IP_MEMORIA='+ip_memoria)
    file.write(x)

print('\n')
print('Ip cambiada\n')

