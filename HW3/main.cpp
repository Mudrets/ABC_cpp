#include <iostream>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

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
void checkPairs(std::vector<int> &resVec, unsigned int* arrA, unsigned int* arrB, int startInd, int endInd) {
    for (int i = startInd; i < endInd; ++i) {
        if (IsPrime(arrA[i] + arrB[i]) || IsPrime(arrA[i] - arrB[i])) { //проверка на простоту
            mtx.lock(); //не даем потокам возможности записывать данные одновременно
            resVec.push_back(i);
            mtx.unlock();
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
    int size, threadCount;
    std::cout << "Enter size of arrays:";
    ReadNumber(size, 1000, 1000000);
    std::cout << "Enter count of threads:";
    ReadNumber(threadCount, 1, size);

    //для таймера
    std::clock_t t1, t2;
    t1 = std::clock();

    //Создаем массивы
    unsigned int* arrA = new unsigned int[size];
    GenerateArr(arrA, size, rand());
    unsigned int* arrB = new unsigned int[size];
    GenerateArr(arrB, size, rand());

    //Создаем вектор для записи результата
    std::vector<int> result;

    int elemsCountForThread = size / threadCount; //вычисляем количество элементов на каждый поток
    std::thread threads[threadCount]; //создаем массив потоков
    for (int i = 0; i < threadCount; ++i) {
        int start = i * elemsCountForThread;
        int end = i < threadCount - 1 ? (i + 1) * elemsCountForThread : size;
        threads[i] = std::thread(checkPairs, std::ref(result), arrA, arrB, start, end); //создаем поток
    }

    for (int i = 0; i < threadCount; ++i) {
        threads[i].join(); //объединяем все потоки
    }

    t2 = std::clock(); //останавливаем таймер
    std::cout << "time: " << (t2 - t1) / 1000.0 << " sec." << std::endl; //выводим время выполнения операции
    std::sort(result.begin(), result.end()); //сортируем вектор с нужными индексами
    for (int i = 0; i < result.size(); ++i) {
        std::cout << result[i] << " "; //выводим вектор
    }

    //удаляем массивы
    delete[] arrA;
    delete[] arrB;
    return 0;
}
