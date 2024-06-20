#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Letter {
    char *adr1;
    char *fam1;
    char *name1;
    char *adr2;
    char *fam2;
    char *name2;
    double cost;
};

int add_letter(struct Letter **letters, int count) {
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
    new_letter->fam1 = strdup(buffer);

    printf("Введите имя получателя: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    new_letter->name1 = strdup(buffer);

    printf("Введите адрес отправителя: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    new_letter->adr2 = strdup(buffer);

    printf("Введите фамилию отправителя: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    new_letter->fam2 = strdup(buffer);

    printf("Введите имя отправителя: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    new_letter->name2 = strdup(buffer);

    printf("Введите стоимость письма: ");
    scanf("%lf", &new_letter->cost);
    getchar();

    return count + 1;
}

void print_letter(struct Letter *letter) {
    printf("Адрес получателя: %s\n", letter->adr1);
    printf("Фамилия получателя: %s\n", letter->fam1);
    printf("Имя получателя: %s\n", letter->name1);
    printf("Адрес отправителя: %s\n", letter->adr2);
    printf("Фамилия отправителя: %s\n", letter->fam2);
    printf("Имя отправителя: %s\n", letter->name2);
    printf("Стоимость: %.2lf\n\n", letter->cost);
}

void print_letters(struct Letter *letters, int count) {
    if (count == 0) {
        printf("База данных пуста.\n");
        return;
    }

    printf("------------------------------\n");
    for (int i = 0; i < count; i++) {
        print_letter(&letters[i]);
        printf("------------------------------\n");
    }
}

void find_by_sender(struct Letter *letters, int count, char *fam2, char *name2) {
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(letters[i].fam2, fam2) == 0 && strcmp(letters[i].name2, name2) == 0) {
            print_letter(&letters[i]);
            found = 1;
        }
    }
    if (!found) {
        printf("Письма от %s %s не найдены.\n", fam2, name2);
    }
}

void find_by_cost(struct Letter *letters, int count, double min_cost) {
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (letters[i].cost > min_cost) {
            print_letter(&letters[i]);
            found = 1;
        }
    }
    if (!found) {
        printf("Письма дороже %.2lf не найдены.\n", min_cost);
    }
}

int compare_letters_by_cost(const void *a, const void *b) {
    double cost_a = ((struct Letter *)a)->cost;
    double cost_b = ((struct Letter *)b)->cost;
    return (cost_b - cost_a);
}

void sort_by_cost(struct Letter *letters, int count) {
    qsort(letters, count, sizeof(struct Letter), compare_letters_by_cost);
    printf("База данных отсортирована по убыванию стоимости.\n");
}

void delete_by_cost(struct Letter **letters, int *count, double cost) {
    int found = 0;
    for (int i = 0; i < *count; i++) {
        if ((*letters)[i].cost == cost) {
            // Освобождаем память, выделенную под поля структуры
            free((*letters)[i].adr1);
            free((*letters)[i].fam1);
            free((*letters)[i].name1);
            free((*letters)[i].adr2);
            free((*letters)[i].fam2);
            free((*letters)[i].name2);

            // Сдвигаем элементы массива, чтобы заполнить место удаленного письма
            for (int j = i; j < *count - 1; j++) {
                (*letters)[j] = (*letters)[j + 1];
            }

            // Уменьшаем количество писем
            (*count)--;

            // Удаляем последнюю структуру из массива, чтобы избежать утечки памяти
            *letters = (struct Letter *)realloc(*letters, (*count) * sizeof(struct Letter));

            found = 1;
            printf("Письмо с указанной стоимостью удалено.\n");
            break; // Выходим из цикла, так как письмо найдено и удалено
        }
    }

    if (!found) {
        printf("Письмо с указанной стоимостью не найдено.\n");
    }
}

// Функция для сохранения базы данных в файл
void save_database(struct Letter *letters, int count, const char *filename) {
    FILE *file = fopen(filename, "w"); // Открываем файл для записи
    if (file == NULL) {
        printf("Ошибка открытия файла для записи.\n");
        return;
    }

    fprintf(file, "%d\n", count); // Сохраняем количество писем

    // Сохраняем информацию о каждом письме
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s\n%s\n%s\n%s\n%s\n%s\n%lf\n", 
                letters[i].adr1, letters[i].fam1, letters[i].name1,
                letters[i].adr2, letters[i].fam2, letters[i].name2,
                letters[i].cost);
    }

    fclose(file); // Закрываем файл
    printf("База данных сохранена в файл %s.\n", filename);
}

// Функция для загрузки базы данных из файла
int load_database(struct Letter **letters, const char *filename) {
    FILE *file = fopen(filename, "r"); // Открываем файл для чтения
    if (file == NULL) {
        printf("Файл не найден. Будет создана новая база данных.\n");
        return 0; // Возвращаем 0 писем
    }

    int count;
    fscanf(file, "%d\n", &count); // Читаем количество писем

    *letters = (struct Letter *)malloc(count * sizeof(struct Letter)); // Выделяем память под письма

    // Читаем информацию о каждом письме
    for (int i = 0; i < count; i++) {
        char buffer[1024];
        fgets(buffer, sizeof(buffer), file);
        buffer[strcspn(buffer, "\n")] = 0;
        (*letters)[i].adr1 = strdup(buffer);

        fgets(buffer, sizeof(buffer), file);
        buffer[strcspn(buffer, "\n")] = 0;
        (*letters)[i].fam1 = strdup(buffer);

        fgets(buffer, sizeof(buffer), file);
        buffer[strcspn(buffer, "\n")] = 0;
        (*letters)[i].name1 = strdup(buffer);

        fgets(buffer, sizeof(buffer), file);
        buffer[strcspn(buffer, "\n")] = 0;
        (*letters)[i].adr2 = strdup(buffer);

        fgets(buffer, sizeof(buffer), file);
        buffer[strcspn(buffer, "\n")] = 0;
        (*letters)[i].fam2 = strdup(buffer);

        fgets(buffer, sizeof(buffer), file);
        buffer[strcspn(buffer, "\n")] = 0;
        (*letters)[i].name2 = strdup(buffer);

        fscanf(file, "%lf\n", &(*letters)[i].cost);
    }

    fclose(file); // Закрываем файл
    printf("База данных загружена из файла %s.\n", filename);
    return count; // Возвращаем количество писем
}

int main() {
    struct Letter *letters = NULL;
    int count = 0;
    int choice;

    // Загрузка базы данных из файла
    count = load_database(&letters, "database.txt");

    while (1) {
        printf("Меню:\n");
        printf("1. Добавить письмо\n");
        printf("2. Распечатать базу данных\n");
        printf("3. Поиск по отправителю\n");
        printf("4. Поиск по стоимости\n");
        printf("5. Сортировать по стоимости\n");
        printf("6. Удалить письмо по стоимости\n");
        printf("7. Сохранить базу данных\n"); // Добавлен пункт меню для сохранения
        printf("8. Выход\n");
        printf("Выберите действие: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                count = add_letter(&letters, count);
                break;
            case 2:
                print_letters(letters, count);
                break;
            case 3: {
                char fam2[1024], name2[1024];
                printf("Введите фамилию отправителя: ");
                fgets(fam2, sizeof(fam2), stdin);
                fam2[strcspn(fam2, "\n")] = 0;
                printf("Введите имя отправителя: ");
                fgets(name2, sizeof(name2), stdin);
                name2[strcspn(name2, "\n")] = 0;
                find_by_sender(letters, count, fam2, name2);
                break;
            }
            case 4: {
                double min_cost;
                printf("Введите минимальную стоимость: ");
                scanf("%lf", &min_cost);
                getchar();
                find_by_cost(letters, count, min_cost);
                break;
            }
            case 5:
                sort_by_cost(letters, count);
                break;
            case 6: { 
                double cost;
                printf("Введите стоимость письма для удаления: ");
                scanf("%lf", &cost);
                getchar();
                delete_by_cost(&letters, &count, cost); 
                break;
            }
            case 7:
                save_database(letters, count, "database.txt"); // Сохранение базы данных
                break;
            case 8:
                // Освобождаем память перед выходом
                for (int i = 0; i < count; i++) {
                    free(letters[i].adr1);
                    free(letters[i].fam1);
                    free(letters[i].name1);
                    free(letters[i].adr2);
                    free(letters[i].fam2);
                    free(letters[i].name2);
                }
                free(letters);
                return 0;
            default:
                printf("Неверный выбор.\n");
        }
    }
}