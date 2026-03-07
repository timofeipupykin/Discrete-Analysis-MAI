import sys
import random
import string


def main():
    if len(sys.argv) != 2:
        print("Usage: python generator.py [num_tests]")
        sys.exit(1)

    print("Generating tests...")
    
    num_tests = int(sys.argv[1])
    
    tests = (
        (
            random.randint(1, 999),
            random.randint(100, 999),
            random.randint(100000, 999999),
            ''.join(random.choices(string.ascii_letters, k=random.randint(1, 2048)))
        )
        for i in range(num_tests)
    )

    file = open("tests.txt", "w", encoding="UTF-8")
    
    for country, city, phone, value in tests:
        file.write(f"+{country}-{city}-{phone}\t{value}\n")
    
    file.close()

if __name__ == "__main__":
    main()