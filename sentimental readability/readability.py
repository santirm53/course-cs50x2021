from cs50 import get_string

# 1. Get text and set variables.
text = get_string("Text: ")
letters = 0
words = 0
sentences = 0

# 2. Loop through the text.
for c in range(len(text)):
    # 3. Add letters, words and sentences through the text.
    if text[c].isalpha():
        letters += 1
    elif text[c] == "." or text[c] == "?" or text[c] == "!":
        words += 1
        sentences += 1
    elif text[c - 1].isalpha() and (text[c] == " " or text[c + 1] == " "):
        words += 1

# DEBUG print(f"Letters: {letters}, Words: {words}, Sentences: {sentences}")

# 4. Calculate "l" and "s", and grade:
l = (letters * 100) / words
s = (sentences * 100) / words
grade = (0.0588 * l) - (0.296 * s) - 15.8

# 5. Output:
if round(grade) < 1:
    print("Before Grade 1")
elif round(grade) >= 16:
    print("Grade 16+")
else:
    print(f"Grade {round(grade)}")