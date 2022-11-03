from cs50 import get_int

checksum = 0

# First get an int to check it's a positive number.
while True:
    cc = get_int("Number: ")
    if cc > 0:
        break

# Then I'll use map(function, iterable) to apply the integer type-cast to each character of the integer (converted to str)
# And that way I can use list() to create a list of digits, easier to manipulate.
cc = list(map(int, str(cc)))

# Then I'll apply the Luhn's algorithm:
# 1.Every other number from the second to last using range(start, stop, step):
for number in range((len(cc) - 2), -1, -2):
    result = cc[number] * 2
    # Same transformation as before to check the two digits of the result:
    result = list(map(int, str(result)))
    if len(result) == 2:
        checksum += result[0] + result[1]
    else:
        checksum += result[0]

# 2. Every other number from the last, add with no multiplication.
for number in range((len(cc) - 1), -1, -2):
    checksum += cc[number]

# If checksum valid:
if checksum % 10 == 0:
    if len(cc) == 15 and cc[0] == 3 and (cc[1] == 4 or cc[1] == 7):
        print("AMEX")
    elif len(cc) == 16 and cc[0] == 5 and (cc[1] in range(1, 6)):
        print("MASTERCARD")
    elif (len(cc) == 13 or len(cc) == 16) and cc[0] == 4:
        print("VISA")
    else:
        print("INVALID")
else:
    print("INVALID")
