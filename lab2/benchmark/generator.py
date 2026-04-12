import sys
import random
import string

def get_random_word(length=None):
    if length is None:
        length = random.randint(1, 10) # Большинство слов короткие
    return ''.join(random.choices(string.ascii_letters, k=length))

def main():
    if len(sys.argv) != 2:
        print("Usage: python generator.py [num_commands]")
        sys.exit(1)

    num_commands = int(sys.argv[1])
    words_in_tree = set()
    
    with open("input.txt", "w", encoding="UTF-8") as file:
        for i in range(num_commands):
            # Выбираем тип операции: 
            # 0: Вставка, 1: Удаление, 2: Поиск, 3: Файлы
            op_type = random.random()
            
            if op_type < 0.33: # 40% - Вставка
                word = get_random_word()
                value = random.randint(0, 2**64 - 1)
                file.write(f"+ {word} {value}\n")
                words_in_tree.add(word.lower())
                
            elif op_type < 0.66: # 20% - Удаление
                if words_in_tree and random.random() > 0.3:
                    word = random.choice(list(words_in_tree))
                else:
                    word = get_random_word()
                file.write(f"- {word}\n")
                words_in_tree.discard(word.lower())
                
            elif op_type < 1: # 35% - Поиск
                if words_in_tree and random.random() > 0.2:
                    word = random.choice(list(words_in_tree))
                else:
                    word = get_random_word()
                # Иногда меняем регистр для проверки регистронезависимости
                if random.random() > 0.5:
                    word = word.upper()
                file.write(f"{word}\n")
                
            # else: # 5% - Команды сохранения/загрузки
            #     action = random.choice(["Save", "Load"])
            #     file.write(f"! {action} dict.bin\n")

    print(f"Generated {num_commands} commands in input.txt")

if __name__ == "__main__":
    main()