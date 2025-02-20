# Sample Python program

import math

class Circle:
    def __init__(self, radius):
        self.radius = radius
    
    def area(self):
        return math.pi * self.radius * self.radius
    
    def perimeter(self):
        return 2 * math.pi * self.radius

circle = Circle(5)
print("Area:", circle.area())
print("Perimeter:", circle.perimeter())
