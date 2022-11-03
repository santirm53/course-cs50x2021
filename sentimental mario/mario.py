from cs50 import get_int

# get the height
while True:
    height = get_int("Height:")
    if (height >= 1) and (height <= 8):
        break

# same logic but instead of nested for loops, just manipulate the strings.
for row in range(height):
    print((" " * (height - 1 - row)) + ("#" * (row + 1)) + "  " + ("#" * (row + 1)))