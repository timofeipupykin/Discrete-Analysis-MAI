import random
import string
import sys

def generate_random_word(length_range=(3, 8)):
    """Генерирует случайное слово из латинских букв разного регистра."""
    length = random.randint(*length_range)
    return ''.join(random.choice(string.ascii_letters) for _ in range(length))

def generate_test(num_words, pattern_len=10):
    # 1. Генерируем паттерн (первая строка)
    pattern = [generate_random_word() for _ in range(pattern_len)]
    pattern_str = " ".join(pattern)
    
    # 2. Генерируем основной текст
    text_words = []
    i = 0
    while i < num_words:
        # С вероятностью 5% вставляем паттерн целиком, чтобы были совпадения
        if random.random() < 0.05:
            # Случайно меняем регистр букв в паттерне для проверки регистронезависимости
            for p_word in pattern:
                mixed_case = "".join(c.upper() if random.random() > 0.5 else c.lower() for c in p_word)
                text_words.append(mixed_case)
                i += 1
        else:
            text_words.append(generate_random_word())
            i += 1
            
    # 3. Формируем структуру строк (разбиваем слова по \n)
    lines = []
    current_line = []
    words_in_current_line = 0
    target_words_per_line = random.randint(3, 10)

    for word in text_words:
        current_line.append(word)
        words_in_current_line += 1
        if words_in_current_line >= target_words_per_line:
            lines.append(" ".join(current_line))
            current_line = []
            words_in_current_line = 0
            target_words_per_line = random.randint(3, 10)
    
    if current_line:
        lines.append(" ".join(current_line))

    # 4. Сохраняем в файл
    with open("input.txt", "w", encoding="utf-8") as f:
        f.write(pattern_str + "\n")
        f.write("\n".join(lines) + "\n")

if __name__ == "__main__":
    # Можно передать количество слов аргументом командной строки
    # Например: python generator.py 100
    if len(sys.argv) > 1:
        try:
            count = int(sys.argv[1])
        except ValueError:
            count = 50
    else:
        count = 50 # По умолчанию 50 слов

    generate_test(count)
    print(f"Тест сгенерирован в файл input.txt. Всего слов в тексте: ~{count}")