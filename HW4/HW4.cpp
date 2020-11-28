#include <iostream>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <omp.h>

std::mutex mtx;

/*
 * Студент: Назмутдинов Роман Ренатович
 * Группа: БПИ-194
 * Вариант 13
 * Определить множество индексов i, для которых (A[i] - B[i]) или
 * (A[i] + B[i]) являются простыми числами. Входные данные: массивы целых
 * положительных чисел А и B, произвольной длины ≥ 1000. Количество
 * потоков является входным параметром.
 */

/**
 * Проверяет является ли переданное число простым
 * @param num проверяемое число
 * @return
 */
bool IsPrime(int num) {
    num = abs(num);
    for (int i = 2; i <= num / 2; ++i)
        if (num % i == 0)
            return false;
    return true;
}

/**
 * Заполняет массив случайными числами
 * @param arr ссылка на заполняемый массив
 * @param elemCount количество элементов в массиве
 * @param seed ключ генерации
 */
void GenerateArr(unsigned int* arr, int elemCount, int seed) {
    srand(seed);
    for (int i = 0; i < elemCount; ++i) {
        arr[i] = abs(rand());
    }
}

/**
 * Проверяет является ли arrA[i] + arrB[i] или arrA[i] - arrB[i]
 * простым числом и если является, то записывает индекс в вектор
 * result
 * @param resVec вектор с индексами
 * @param arrA
 * @param arrB
 * @param startInd начальный индекс проверки
 * @param endInd конечный индекс проверки
 */
void checkPairs(std::vector<std::pair<int, int>>* resVec, unsigned int* arrA, unsigned int* arrB, int startInd, int endInd) {
    for (int i = startInd; i < endInd; ++i) {
        bool sum = false;
        if ((sum = IsPrime( arrA[i] + arrB[i])) || IsPrime(arrA[i] - arrB[i])) { //проверка на простоту
            int prime;
            if (sum)
                prime = arrA[i] + arrB[i];
            else
                prime = arrA[i] - arrB[i];
#pragma omp critical
            resVec->push_back(std::pair<int, int>(i, prime));
        }
    }
}

/**
 * Считывает число в отрезке [minValue, maxValue]
 * @param num ссылка по которой будет записано значение
 * @param minValue минимальное значение
 * @param maxValue максимальное значение
 */
void ReadNumber(int &num, int minValue, int maxValue = INT_MAX) {
    std::cin >> num;
    while (num < minValue || num > maxValue) {
        std::cout << "Incorrect input!" << std::endl;
        std::cout << "Enter number again:";
        std::cin >> num;
    }
}

int main() {
    srand(static_cast<int>(time(0))); //для генерации случайных чисел

    //считываем входные данные
    int size, threadCount, countOfLines;
    std::cout << "Enter size of arrays:";
    ReadNumber(size, 1000, 1000000);
    std::cout << "Enter count of threads:";
    ReadNumber(threadCount, 1, size);
    std::cout << "Enter the max number of lines to output:";
    ReadNumber(countOfLines, 1, size);

    //для таймера
    std::clock_t t1, t2;
    t1 = std::clock();

    //Создаем массивы
    unsigned int* arrA = new unsigned int[size];
    GenerateArr(arrA, size, rand());
    unsigned int* arrB = new unsigned int[size];
    GenerateArr(arrB, size, rand());

    //Создаем вектор для записи результата
    std::vector<std::pair<int, int>> result;
    int elemsCountForThread = size / threadCount; //вычисляем количество элементов на каждый поток

    #pragma omp parallel num_threads(threadCount)
    {
        int i = omp_get_thread_num();
        int start = i * elemsCountForThread;
        int end = i < threadCount - 1 ? (i + 1) * elemsCountForThread : size;
        checkPairs(&result, arrA, arrB, start, end);
    }

    std::cout << "\nResult:" << std::endl;
    t2 = std::clock(); //останавливаем таймер
    std::cout << "time: " << (t2 - t1) / 1000.0 << " sec." << std::endl; //выводим время выполнения операции
    std::sort(result.begin(), result.end()); //сортируем вектор с нужными индексами
    int countOutputLines = countOfLines > result.size() ? result.size() : countOfLines;
    for (int i = result.size() - countOutputLines; i < result.size(); ++i) {
        std::printf("[%d] %u +/- %u = %d\n", result[i].first, arrA[result[i].first],
                    arrB[result[i].first], result[i].second);
    }

    //удаляем массивы
    delete[] arrA;
    delete[] arrB;
    return 0;
}
