#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Структура для хранения данных о человеке
struct Man {
    char *surname;
    char *name;
};

// Структура для хранения данных о дате
struct Date {
    int day;
    int month;
    int year;
};

// Структура для хранения данных о письме
struct Letter {
    char *adr1;    // Указатель на строку с адресом получателя
    struct Man recipient; // Получатель письма
    char *adr2;    // Указатель на строку с адресом отправителя
    struct Man sender; // Отправитель письма
    double cost;   // Стоимость письма
    struct Date sending_date; // Дата отправки
    int delivery_time; // Срок доставки в днях
};

// Функция для получения текущей даты
struct Date getCurrentDate() {
    struct Date currentDate;
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);

    currentDate.day = timeinfo->tm_mday;
    currentDate.month = timeinfo->tm_mon + 1;
    currentDate.year = timeinfo->tm_year + 1900;

    return currentDate;
}

// Функция для сравнения двух дат
int compareDates(struct Date date1, struct Date date2) {
    if (date1.year != date2.year) {
        return date1.year - date2.year;
    } else if (date1.month != date2.month) {
        return date1.month - date2.month;
    } else {
        return date1.day - date2.day;
    }
}

// Функция для добавления нового письма в базу данных
int add_letter(struct Letter **letters, int count, struct Date currentDate) {
    *letters = (struct Letter *)realloc(*letters, (count + 1) * sizeof(struct Letter));
    struct Letter *new_letter = &(*letters)[count];

    char buffer[1024];

    printf("Введите адрес получателя: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    new_letter->adr1 = strdup(buffer);

    printf("Введите фамилию получателя: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    new_letter->recipient.surname = strdup(buffer);

    printf("Введите имя получателя: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    new_letter->recipient.name = strdup(buffer);

    printf("Введите адрес отправителя: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    new_letter->adr2 = strdup(buffer);

    printf("Введите фамилию отправителя: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    new_letter->sender.surname = strdup(buffer);

    printf("Введите имя отправителя: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    new_letter->sender.name = strdup(buffer);

    printf("Введите стоимость письма: ");
    scanf("%lf", &new_letter->cost);
    getchar(); // Очищаем буфер от \n

    // Ввод даты отправки
    printf("Введите дату отправки (день месяц год): ");
    scanf("%d %d %d", &new_letter->sending_date.day, 
          &new_letter->sending_date.month, &new_letter->sending_date.year);
    getchar(); // Очищаем буфер от \n

    printf("Введите срок доставки в днях: ");
    scanf("%d", &new_letter->delivery_time);
    getchar(); // Очищаем буфер от \n

    return count + 1;
}

// Функция для печати информации о письме в виде строки таблицы
void print_letter_row(struct Letter *letter) {
    printf("| %-18s   %-2s %-25s  %-18s   %-2s %-25s   %-9.2lf  %02d.%02d.%04d  %d дней \n",
           letter->adr1, letter->recipient.surname, letter->recipient.name,
           letter->adr2, letter->sender.surname, letter->sender.name,
           letter->cost, letter->sending_date.day, letter->sending_date.month,
           letter->sending_date.year, letter->delivery_time);
}

// Функция для печати разделителя таблицы
void print_table_separator() {
    printf("+--------------------+------------------------------+--------------------+------------------------------+-----------+---------------+---------------+\n");
}

// Функция для печати заголовка таблицы
void print_table_header() {
    print_table_separator();
    printf("| Адрес получателя   | Получатель                   | Адрес отправителя  | Отправитель                  | Стоимость | Дата отправки | Срок доставки |\n");
    print_table_separator();
}

// Функция для печати всей базы данных писем
void print_letters_table(struct Letter *letters, int count) {
    if (count == 0) {
        printf("База данных пуста.\n");
        return;
    }

    print_table_header();
    for (int i = 0; i < count; i++) {
        print_letter_row(&letters[i]);
        print_table_separator();
    }
}

// Функция для поиска писем с нарушением срока доставки
void find_delayed_letters(struct Letter *letters, int count, struct Date currentDate) {
    int found = 0;
    print_table_header();
    for (int i = 0; i < count; i++) {
        // Рассчитываем ожидаемую дату доставки
        struct tm sendingTime = {0};
        sendingTime.tm_year = letters[i].sending_date.year - 1900;
        sendingTime.tm_mon = letters[i].sending_date.month - 1;
        sendingTime.tm_mday = letters[i].sending_date.day;

        time_t sendingTimeSec = mktime(&sendingTime);
        sendingTimeSec += (time_t)letters[i].delivery_time * 24 * 60 * 60;
        struct tm *expectedDeliveryTime = localtime(&sendingTimeSec);

        struct Date expectedDeliveryDate = {
            .day = expectedDeliveryTime->tm_mday,
            .month = expectedDeliveryTime->tm_mon + 1,
            .year = expectedDeliveryTime->tm_year + 1900
        };

        if (compareDates(expectedDeliveryDate, currentDate) < 0) {
            print_letter_row(&letters[i]);
            found = 1;
            print_table_separator();
        }
    }

    if (!found) {
        printf("Письма с нарушением срока доставки не найдены.\n");
    }
}

// Функция для поиска писем по получателю (man)
void find_letters_by_recipient(struct Letter *letters, int count, char *surname, char *name) {
    int found = 0;
    print_table_header();
    for (int i = 0; i < count; i++) {
        if (strcmp(letters[i].recipient.surname, surname) == 0 && 
            strcmp(letters[i].recipient.name, name) == 0) {
            print_letter_row(&letters[i]);
            found = 1;
            print_table_separator();
        }
    }
    if (!found) {
        printf("Письма, адресованные %s %s, не найдены.\n", surname, name);
    }
}

// Функция для сравнения двух писем по стоимости (для qsort)
int compare_letters_by_cost(const void *a, const void *b) {
    double cost_a = ((struct Letter *)a)->cost; 
    double cost_b = ((struct Letter *)b)->cost; 
    return (cost_b - cost_a); 
}
 
// Функция для сортировки базы данных по убыванию стоимости
void sort_by_cost(struct Letter *letters, int count) {
    qsort(letters, count, sizeof(struct Letter), compare_letters_by_cost); 
    printf("База данных отсортирована по убыванию стоимости.\n"); 
}

void save_work (work* b_work, int n) {
    work *tw;
    FILE* pf;
    // определение формата записи на диск 
    if((pf = fopen("data.bin","wb"))==NULL) {
        perror("Ошибка открытия файла: режим save_bin");
    }
    // двоичная запись массива структур в файл data.bin
    tw=b_work; 
    fwrite (tw, sizeof(work), size_work, pf);	
    fclose (pf); // закрыть бинарный файл	
    // запись динамических строк из Кучи в файл data.txt
    if((pf = fopen("data.txt","w"))==NULL) {
        perror("Ошибка открытия файла: режим save_txt");
    }
    // запись строк в текстовый файл
    for (tw=b_work; tw<b_work+n; tw++) {
        fprintf (pf, "%s\n", tw->worker.fam);
        fprintf (pf, "%s\n", tw->worker.name);
        fprintf (pf, "%s\n", tw->worker.father);
    }
    fclose (pf); // закрыть текстовый файл
};

void load_work (work* b_work) {work *tw;
    FILE* pf;
    char  buff[80]; // буфер для чтения строки с диска
// определение формата чтения с диска (бинарный файл)
    if((pf = fopen("data.bin","rb"))==NULL) {
        perror("Ошибка открытия файла: режим load_bin");
    }
// продолжать цикл пока "не конец файла"
    for (tw=b_work; !feof(pf); tw++)	
    fread (tw, sizeof(work),1, pf);	// чтение 1–ой структуры
    fclose (pf); // закрыть бинарный файл	
 // определение формата чтения с диска (текстовый файл)
    if((pf = fopen("data.txt","r"))==NULL)
{perror("Ошибка открытия файла: режим load_txt");
}
for (tw=b_work; !feof(pf); tw++)	
{ fscanf (pf,"%s\n", buff);			// fam с диска в буфер
  //	выделение памяти под строку 
  tw->worker.fam= new char(strlen(buff)+1) ;	
  strcpy (tw->worker.fam, buff); // копирование из буфера в память
  fscanf (pf,"%s\n",buff);	    		// name с диска
  tw->worker.name= new char(strlen(buff)+1) ;	
  strcpy (tw->worker.name, buff);
  fscanf (pf,"%s\n",buff);				// father с диска
  tw->worker.father= new char(strlen(buff)+1) ;	
  strcpy (tw->worker.father, buff);
}
fclose (pf); // закрыть текстовый файл
};


int main() {
    struct Letter *letters = NULL;
    int count = 0;
    int choice;

    struct Date currentDate = getCurrentDate();
    printf("Текущая дата: %d.%d.%d\n", currentDate.day, currentDate.month, currentDate.year);

    while (1) {
        printf("Меню:\n");
        printf("1. Добавить письмо\n");
        printf("2. Распечатать базу данных\n");
        printf("3. Поиск по получателю\n");
        printf("4. Поиск по стоимости\n");
        printf("5. Сортировать по стоимости\n");
        printf("6. Поиск писем с нарушением срока доставки\n");
        printf("7. Выход\n");
        printf("Выберите действие: ");
        scanf("%d", &choice);
        getchar(); // Очищаем буфер от \n

        switch (choice) {
            case 1:
                count = add_letter(&letters, count, currentDate);
                break;
            case 2:
                print_letters_table(letters, count);
                break;
            case 3: {
                char surname[1024], name[1024];
                printf("Введите фамилию получателя: ");
                fgets(surname, sizeof(surname), stdin);
                surname[strcspn(surname, "\n")] = 0;
                printf("Введите имя получателя: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                find_letters_by_recipient(letters, count, surname, name);
                break;
            }
            case 4: {
                double min_cost;
                printf("Введите минимальную стоимость: ");
                scanf("%lf", &min_cost);
                getchar(); // Очищаем буфер от \n
                // find_by_cost(letters, count, min_cost); //  -- нужно реализовать
                break;
            }
            case 5:
                sort_by_cost(letters, count);
                break;
            case 6:
                find_delayed_letters(letters, count, currentDate);
                break;
            case 7:
                // Освобождаем память перед выходом
                for (int i = 0; i < count; i++) { 
                    free(letters[i].adr1); 
                    free(letters[i].recipient.surname);
                    free(letters[i].recipient.name); 
                    free(letters[i].adr2); 
                    free(letters[i].sender.surname);
                    free(letters[i].sender.name); 
                }
                free(letters); 
                return 0; 
            default:
                printf("Неверный выбор.\n");
        }
    }
}