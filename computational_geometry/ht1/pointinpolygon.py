#!/usr/bin/env python
import sys
import re

class Polygon(object):
  OFFSET = 1
  def __init__(self, points):
    self.points = points
    self.prepare_ray_outer_point()
    #print self.bounding_box
  def crosses(self, ray1, ray2):
    test_result = self.intersection_test(ray1[0], ray1[1], ray2[0], ray2[1])
    if not test_result:
      return [test_result, []]
    return [True, []]
    print "check1"
  def prepare_ray_outer_point(self):
    self.bounding_box = [[f(x[i] for x in self.points) for i in [0, 1]] for f in [min, max]]
    self.ray_outer_point = [x - self.OFFSET for x in self.bounding_box[0]]
    return self.ray_outer_point
  #returns True if point is inside of given polynom
  #returns False otherwise
  def hit_test(self, point):
    if point[0] < self.bounding_box[0][0] or point[0] > self.bounding_box[1][0] or point[1] < self.bounding_box[0][1] or point[1] > self.bounding_box[1][1]:
      return False
    test_ray = [self.ray_outer_point, point]
    odd_no_of_intersections = False
    for i in range(len(self.points)):
      line_segment = [self.points[i - 1], self.points[i]]
      [cross_result, intersection] = self.crosses(test_ray, line_segment)
      if cross_result:
        odd_no_of_intersections = not odd_no_of_intersections
    return odd_no_of_intersections
  def dot_product(self, p1, p2, p3, p4):
    return [(p2[i] - p1[i])*(p4[i] - p3[i]) for i in [0, 1]].sum
  def intersection_test2(self, box1, box2):
    for i in [0, 1]:
      if box1[0][i] < box2[0][i]:
        if box1[1][i] < box2[0][i]:
          return False
      else:
        if box2[1][i] < box1[0][i]:
          return False
    return True
  #fast intersection test for two given pairs of points
  def intersection_test(self, p1, p2, p3, p4):
    boxes = [
      [[f(x1[i], x2[i]) for i in [0, 1]] for f in [min, max]]
      for x1, x2 in [[p1, p2], [p3, p4]]
    ]
    return self.intersection_test2(boxes[0], boxes[1])

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
