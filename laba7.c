#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#define size_work 5        
#define size_max 200

// Структура для хранения даты
struct date {
    int day;
    char* month;
    int year;
};

// Структура для хранения информации о ценном письме
struct letter {
    char* index;  // Почтовый индекс (оставлен без изменений)
    char* address; // Адрес (оставлен без изменений)
    struct date date_send; // Дата отправки
    int delivery_days; // Срок доставки в днях
    struct man* sender;  // Отправитель 
    struct man* recipient; // Получатель
};

// Структура для хранения ФИО
struct man {
    char* surname;
    char* name;
    char* father;
};

// Функция для выделения памяти под строку и копирования в нее данных
char* createAndCopyString(const char* source) {
    if (source == NULL) return NULL; 
    size_t len = strlen(source) + 1;
    char* dest = (char*)malloc(len * sizeof(char));
    if (dest == NULL) {
        perror("Ошибка выделения памяти");
        exit(EXIT_FAILURE);
    }
    strcpy(dest, source);
    return dest;
}

// Функция для ввода даты с клавиатуры
struct date inputDate() {
    struct date d;
    printf("Введите день: ");
    scanf("%d", &d.day);
    getchar(); // Очистка буфера

    char monthInput[20];
    printf("Введите месяц: ");
    fgets(monthInput, sizeof(monthInput), stdin);
    monthInput[strcspn(monthInput, "\n")] = 0; // Удаление символа новой строки
    d.month = createAndCopyString(monthInput);

    printf("Введите год: ");
    scanf("%d", &d.year);
    getchar(); 
    return d;
}

// Функция для ввода информации о человеке с клавиатуры
struct man inputMan() {
    struct man m;
    char input[80];

    printf("Введите фамилию: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    m.surname = createAndCopyString(input);

    printf("Введите имя: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    m.name = createAndCopyString(input);

    printf("Введите отчество: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    m.father = createAndCopyString(input);

    return m;
}

// Функция для ввода информации о письме с клавиатуры
struct letter inputLetter() {
    struct letter l;
    char input[80];

    printf("Введите индекс: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    l.index = createAndCopyString(input);

    printf("Введите адрес: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    l.address = createAndCopyString(input);

    printf("Введите дату отправки:\n");
    l.date_send = inputDate();

    printf("Введите срок доставки (в днях): ");
    scanf("%d", &l.delivery_days);
    getchar(); 

    printf("Введите информацию об отправителе:\n");
    l.sender = (struct man*)malloc(sizeof(struct man)); // Выделение памяти
    if (l.sender == NULL) {
        perror("Ошибка выделения памяти");
        exit(EXIT_FAILURE);
    }
    *l.sender = inputMan(); 

    printf("Введите информацию о получателе:\n");
    l.recipient = (struct man*)malloc(sizeof(struct man));
    if (l.recipient == NULL) {
        perror("Ошибка выделения памяти");
        exit(EXIT_FAILURE);
    }
    *l.recipient = inputMan();

    return l;
}
// Функция для вывода информации о дате
void printDate(const struct date* d) {
    printf("%d %s %d", d->day, d->month, d->year);
}

// Функция для вывода информации о человеке
void printMan(const struct man* m) {
    printf("%s %s %s", m->surname, m->name, m->father);
}

// Функция для вывода информации о письме
void printLetter(const struct letter* l) {
    printf("Индекс: %s\n", l->index);
    printf("Адрес: %s\n", l->address);
    printf("Дата отправки: "); 
    printDate(&l->date_send); 
    printf("\n");
    printf("Срок доставки: %d дней\n", l->delivery_days);
    printf("Отправитель: "); 
    printMan(l->sender); 
    printf("\n");
    printf("Получатель: "); 
    printMan(l->recipient); 
    printf("\n");
    printf("---------------------------\n");
}

// Функция для инициализации базы данных с клавиатуры
void init_letters(struct letter* letters, int n) {
    for (int i = 0; i < n; ++i) {
        printf("Введите данные для письма %d:\n", i + 1);
        letters[i] = inputLetter();
        printf("---------------------------\n");
    }
}

// Функция для поиска писем с нарушением срока доставки
void findDelayedLetters(struct letter* letters, int n, const struct date* currentDate) {
    printf("Письма с нарушением срока доставки:\n");
    int found = 0; 
    for (int i = 0; i < n; ++i) {
        // Расчет предполагаемой даты доставки
        struct date expectedDeliveryDate = letters[i].date_send;
        expectedDeliveryDate.day += letters[i].delivery_days; 

        // Упрощенная проверка нарушения срока (без учета месяцев и лет)
        if (expectedDeliveryDate.day < currentDate->day && 
            strcmp(expectedDeliveryDate.month, currentDate->month) == 0 &&
            expectedDeliveryDate.year == currentDate->year) {
            printLetter(&letters[i]);
            found = 1; 
        }
    }

    if (!found) {
        printf("Писем с нарушением срока доставки не найдено.\n");
    }
    printf("---------------------------\n");
}


// Функция для поиска писем по ФИО получателя
void findLettersByRecipient(struct letter* letters, int n) {
    struct man searchRecipient;
    char input[80];

    printf("Введите фамилию получателя: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    searchRecipient.surname = createAndCopyString(input);

    printf("Введите имя получателя: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    searchRecipient.name = createAndCopyString(input);

    printf("Введите отчество получателя: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    searchRecipient.father = createAndCopyString(input);

    printf("Найденные письма:\n");
    int found = 0;
    for (int i = 0; i < n; ++i) {
        // Сравнение ФИО получателя
        if (strcmp(letters[i].recipient->surname, searchRecipient.surname) == 0 &&
            strcmp(letters[i].recipient->name, searchRecipient.name) == 0 &&
            strcmp(letters[i].recipient->father, searchRecipient.father) == 0) {

            printLetter(&letters[i]);
            found = 1; 
        }
    }

    if (!found) {
        printf("Писем от данного отправителя не найдено.\n");
    }
    printf("---------------------------\n");
}

// Функция для получения текущей даты
struct date getCurrentDate() {
    struct date currentDate;
    time_t now = time(NULL);
    struct tm *ltm = localtime(&now);

    currentDate.day = ltm->tm_mday;
    // Преобразование номера месяца (0-11) в название
    switch (ltm->tm_mon) {
        case 0: currentDate.month = "Январь"; break;
        case 1: currentDate.month = "Февраль"; break;
        case 2: currentDate.month = "Март"; break;
        case 3: currentDate.month = "Апрель"; break;
        case 4: currentDate.month = "Май"; break;
        case 5: currentDate.month = "Июнь"; break;
        case 6: currentDate.month = "Июль"; break;
        case 7: currentDate.month = "Август"; break;
        case 8: currentDate.month = "Сентябрь"; break;
        case 9: currentDate.month = "Октябрь"; break;
        case 10: currentDate.month = "Ноябрь"; break;
        case 11: currentDate.month = "Декабрь"; break;
    }
    currentDate.year = 1900 + ltm->tm_year;
    return currentDate;
}

// Функция для печати базы данных
void printDatabase(struct letter* letters, int n) {
    if (n == 0) {
        printf("База данных пуста.\n");
        return;
    }
    printf("База данных писем:\n");
    for (int i = 0; i < n; ++i) {
        printLetter(&letters[i]);
    }
}

int main() {
    setlocale(LC_ALL, "Russian"); 
    struct letter letters[size_max];
    int n = 0;
    
    // Получение текущей даты
    struct date currentDate = getCurrentDate();
    printf("Текущая дата: ");
    printDate(&currentDate);
    printf("\n");

    // Загрузка данных из файла (не реализована в этом примере)
    // ...

    int choice;
    do {
        printf("\nМеню:\n");
        printf("1. Добавить новое письмо\n");
        printf("2. Распечатать базу данных\n");
        printf("3. Найти письма с нарушением срока доставки\n");
        printf("4. Найти письма по получателю\n");
        printf("0. Выход\n");
        printf("Введите ваш выбор: ");
        scanf("%d", &choice);
        getchar(); // Очистка буфера

        switch (choice) {
            case 1: 
                if (n < size_max) {
                    letters[n] = inputLetter();
                    n++;
                } else {
                    printf("База данных заполнена.\n");
                }
                break;
            case 2: 
                printDatabase(letters, n);
                break;
            case 3: 
                findDelayedLetters(letters, n, &currentDate); 
                break;
            case 4: 
                findLettersByRecipient(letters, n);
                break;
            case 0:
                printf("Выход из программы.\n");
                break;
            default:
                printf("Неверный выбор. Попробуйте снова.\n");
        }
    } while (choice != 0);

    // Сохранение данных в файл (не реализовано в этом примере)
    // ...

    // Освобождение памяти
    for (int i = 0; i < n; ++i) {
        free(letters[i].index);
        free(letters[i].address);
        free(letters[i].date_send.month);
        free(letters[i].sender);
        free(letters[i].recipient);
    }

    return 0;
}