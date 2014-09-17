#!/usr/bin/env python
import sys
import re

class Polygon(object):
  def __init__(self, points):
    self.points = points
    print "in init"
  def hit_test(self, point):
    return True

def load_point(str):
  r = re.compile('[\-0-9]+')
  return [int(s) for s in r.findall(str)]

def main():
  n = int(sys.stdin.readline())
  points = [load_point(sys.stdin.readline()) for x in range(n)]
  print points
  polygon = Polygon(points)
  #next - test polygon against set of m points:
  m = int(sys.stdin.readline())

  hit_test_results = [
    ["no", "yes"][polygon.hit_test(load_point(sys.stdin.readline()))] for x in range(m)
  ]
  for answer in hit_test_results:
    print answer
  return 0

if __name__ == "__main__": main()
