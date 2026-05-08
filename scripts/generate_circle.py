
import numpy as np

rad = -1

while rad < 0:
    try:
        rad = float(input("Enter radius: "))
    except ValueError:
        print("Please enter valid radius.")
        rad = -1

count = -1

while count < 0:
    try:
        count = int(input("Enter node count: "))
    except ValueError:
        print("Please enter valid count.")
        count = -1

count_prefix = [f"Count: { count }\n"]
coords = [None] * count

ainc = 2 * np.pi / count
for i in range(count):
    a = ainc * i
    x = rad * np.cos(a)
    y = rad * np.sin(a)

    coords[i] = "{} {:.4f} {:.4f}\n".format(i + 1, x, y)

path = "./circle.txt"
with open(path, "w") as file:
    file.writelines(count_prefix + coords)
    print(f"Circle generated in { path }.")
