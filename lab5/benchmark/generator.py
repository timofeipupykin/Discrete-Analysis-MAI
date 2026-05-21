import sys
import random
import string

def generate_tests():
    if len(sys.argv) != 3:
        print("Использование: python generator.py <длина_текста> <количество_паттернов>")
        sys.exit(1)

    try:
        text_length = int(sys.argv[1])
        num_patterns = int(sys.argv[2])
    except ValueError:
        print("Ошибка: Аргументы должны быть целыми числами.")
        sys.exit(1)

    if text_length <= 0 or num_patterns <= 0:
        print("Ошибка: Размеры должны быть больше нуля.")
        sys.exit(1)

    alphabet = string.ascii_lowercase

    # Генерируем текст. Он остается в памяти, так как из него нужно брать подстроки.
    text = ''.join(random.choices(alphabet, k=text_length))

    # Открываем файл и пишем данные потоково
    with open("input.txt", "w", encoding="utf-8") as f:
        f.write(text + "\n")
        
        for _ in range(num_patterns):
            pattern_length = random.randint(1, text_length)
            
            if random.random() < 0.7:
                # Вместо создания новой строки-подстроки (которая выделяет память),
                # берем срез "на лету" прямо внутри f.write()
                start_idx = random.randint(0, text_length - pattern_length)
                f.write(text[start_idx:start_idx + pattern_length] + "\n")
            else:
                # Генерируем случайный паттерн и сразу пишем в файл
                pattern = ''.join(random.choices(alphabet, k=pattern_length))
                f.write(pattern + "\n")

    print(f"Успешно сгенерировано! Файл input.txt создан.")
    print(f"Длина текста: {text_length}, Количество паттернов: {num_patterns}")

if __name__ == "__main__":
    generate_tests()