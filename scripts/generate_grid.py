
import numpy as np

side = -1

while side < 0:
    try:
        side = int(input("Enter side length: "))
    except ValueError:
        print("Please enter valid side length.")
        side = -1

dist = -1

while dist < 0:
    try:
        dist = float(input("Enter dist: "))
    except ValueError:
        print("Please enter valid dist.")
        dist = -1

count = side * side

count_prefix = [f"Count: { count }\n"]
coords = [None] * count

for i in range(count):
    x = (i % side) * dist
    y = (i // side) * dist
    coords[i] = "{} {:.4f} {:.4f}\n".format(i + 1, x, y)

path = "./grid.generated"
with open(path, "w") as file:
    file.writelines(count_prefix + coords)
    print(f"Grid generated in { path }.")
