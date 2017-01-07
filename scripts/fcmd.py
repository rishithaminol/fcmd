#!/usr/bin/python

# find commands or keywords in the shell environment "$PATH"

from os import environ
from os import listdir
from os import pathsep
from sys import argv

def fcmd(cmd_name, path_list):
   real_cmds = []
   related_cmds = []

   while len(path_list) != 0:
      path = path_list.pop() + '/'
      try:
         cmds = listdir(path)
      except:
         continue

      for cmd in cmds:
         testing = cmd.find(cmd_name,0,len(cmd))
         if cmd ==  cmd_name:
            real_cmds.append(path + cmd_name)
            continue
         if testing != -1:
            related_cmds.append(path + cmd)

   return (real_cmds, related_cmds)

if __name__ == '__main__':
   if len(argv[1:]) == 0:
      print "Usage: %s Command(s)" % argv[0]
      exit(1)

   PATH = environ['PATH']

   for command in argv[1:]:
      reslt = fcmd(command ,PATH.split(pathsep,len(PATH)))

      print "\n".join(reslt[0])

      if len(reslt[1]) != 0:
         print "\nRelated commands:"
         print "\n".join(reslt[1])
