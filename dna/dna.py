import sys
import csv

# Check correct usage.
if len(sys.argv) != 3:
    print("Usage: dna.py file.csv file.txt")
    sys.exit(1)

# Open CSV file and read to memory.
database = []
srt = []

csv_file = sys.argv[1]
with open(csv_file, "r") as file:
    reader = csv.reader(file)
    # srt gives me the fieldname reference starting from srt[1], so after that I pop the first element
    srt = next(reader)
    srt.pop(0)

with open(csv_file, "r") as file:
    reader = csv.DictReader(file)
    for row in reader:
        # Store everything in database as a list of dictionaries
        database.append(row)

# Open DNA sequence and read to memory.
txt_file = sys.argv[2]
with open(txt_file, "r") as file:
    reader = csv.reader(file)
    # I guess that reader outputs lists, so I had to think of this workaround
    # I came back and I suppose I could've done sequence = file.read()
    sequence_list = next(reader)
sequence = sequence_list[0]

# For each STR, compute the longest run of consecutive repeats.
# Populate a list with the matches
matches = []
count = 0

# Loop through the list of SRT's in database:
for i in range(len(srt)):
    # Populate matches list by setting to 0 matches for our compared srt (srt[i]).
    matches.append(0)
    for j in range(len(sequence)):
        # Loop through the sequence.
        if sequence[j: j + len(srt[i])] == srt[i]:
            # If it matches, start looking for repeats:
            position_shift = 0
            count = 0
            # Check for repeats:
            while sequence[j + position_shift: j + position_shift + len(srt[i])] == srt[i]:
                count += 1
                position_shift += len(srt[i])
            # Register matches:
            if count > matches[i]:
                matches[i] = count

# Look for matches
for i in range(len(database)):
    # Loop through the list of matches and through the database
    coincidences = 0
    for j in range(len(srt)):
        # Typecast to int and compare the value for each srt to the corresponding matches.
        # For each person (i) check the srt coincidences
        if matches[j] == int(database[i][srt[j]]):
            coincidences += 1
    # Finally check if all SRT are a coincidence
    if coincidences == len(srt):
        print(database[i]["name"])
        sys.exit(0)

print("No match.")