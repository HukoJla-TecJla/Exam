//MemoryBlock: Структура для хранения информации о блоках памяти.
//FreeBlock : Структура для хранения информации о свободных блоках с дескрипторами границ.
//MemoryManager : Класс для управления динамической памятью с использованием дескрипторов границ.
//allocate(size_t size) : Метод для выделения памяти заданного размера с использованием лучшего подходящего блока.Он ищет свободный блок с наименьшим размером, который больше или равен запрашиваемому размеру.
//deallocate(void* address, size_t size) : Метод для освобождения блока памяти по указанному адресу и размеру.При освобождении блока памяти вставляется в список свободных блоков, а затем соседние свободные блоки объединяются для уменьшения фрагментации.
#include <iostream>
#include <vector>

// Структура для хранения информации о блоках памяти
struct MemoryBlock {
    void* address; // Адрес начала блока памяти
    size_t size;   // Размер блока памяти

    MemoryBlock(void* addr, size_t sz) : address(addr), size(sz) {}
};

// Структура для хранения информации о свободных блоках с дескрипторами границ
struct FreeBlock {
    size_t size;   // Размер свободного блока
    bool free;     // Флаг, указывающий, свободен ли блок
    FreeBlock* next; // Следующий свободный блок

    FreeBlock(size_t sz, bool fr) : size(sz), free(fr), next(nullptr) {}
};

// Класс для управления динамической памятью с использованием дескрипторов границ
class MemoryManager {
private:
    void* memoryPool;     // Указатель на начало области памяти
    size_t memorySize;    // Общий размер памяти
    std::vector<FreeBlock*> freeBlocks; // Список свободных блоков

public:
    MemoryManager(size_t size) : memorySize(size) {
        // Выделение памяти для управления
        memoryPool = new char[size];

        // Инициализация первого свободного блока с дескрипторами границ
        FreeBlock* initialBlock = new FreeBlock(size, true);
        freeBlocks.push_back(initialBlock);
    }

    ~MemoryManager() {
        // Освобождение памяти при уничтожении объекта
        delete[] reinterpret_cast<char*>(memoryPool);
        for (auto block : freeBlocks) {
            delete block;
        }
    }

    // Метод для выделения памяти заданного размера (лучший подходящий блок)
    void* allocate(size_t size) {
        FreeBlock* bestFitBlock = nullptr;
        size_t bestFitSize = memorySize + 1; // Начальное значение больше максимального размера

        // Поиск лучшего подходящего свободного блока
        for (auto block : freeBlocks) {
            if (block->free && block->size >= size && block->size < bestFitSize) {
                bestFitBlock = block;
                bestFitSize = block->size;
            }
        }

        if (bestFitBlock) {
            // Найден подходящий блок
            bestFitBlock->free = false; // Помечаем блок как занятый

            // Добавляем дескриптор границы в начало блока
            size_t* boundaryTag = reinterpret_cast<size_t*>(bestFitBlock);
            *boundaryTag = bestFitBlock->size;

            // Вычисляем адрес начала блока памяти для возврата
            void* allocatedAddress = reinterpret_cast<void*>(reinterpret_cast<char*>(bestFitBlock) + sizeof(size_t));

            // Обновляем список свободных блоков
            updateFreeBlocks();

            return allocatedAddress;
        }

        // Если подходящий блок не найден, вернуть nullptr
        return nullptr;
    }

    // Метод для выделения памяти заданного размера (первый подходящий блок)
    void* allocate(size_t size) {
        for (auto block : freeBlocks) {
            if (block->free && block->size >= size) {
                // Найден подходящий блок
                block->free = false; // Помечаем блок как занятый

                // Добавляем дескриптор границы в начало блока
                size_t* boundaryTag = reinterpret_cast<size_t*>(block);
                *boundaryTag = block->size;

                // Вычисляем адрес начала блока памяти для возврата
                void* allocatedAddress = reinterpret_cast<void*>(reinterpret_cast<char*>(block) + sizeof(size_t));

                // Обновляем список свободных блоков
                updateFreeBlocks();

                return allocatedAddress;
            }
        }

        // Если подходящий блок не найден, вернуть nullptr
        return nullptr;
    }



    // Метод для освобождения блока памяти по указанному адресу и размеру
    void deallocate(void* address, size_t size) {
        // Определяем указатель на дескриптор границы
        size_t* boundaryTag = reinterpret_cast<size_t*>(reinterpret_cast<char*>(address) - sizeof(size_t));

        // Получаем размер блока памяти из дескриптора границы
        size_t blockSize = *boundaryTag;

        // Вычисляем указатель на начало свободного блока памяти
        FreeBlock* newFreeBlock = reinterpret_cast<FreeBlock*>(reinterpret_cast<char*>(address) - sizeof(size_t));

        // Устанавливаем новый свободный блок
        newFreeBlock->size = blockSize;
        newFreeBlock->free = true;

        // Добавляем блок в список свободных блоков
        freeBlocks.push_back(newFreeBlock);

        // Обновляем список свободных блоков
        updateFreeBlocks();
    }

    // Метод для выделения памяти заданного размера (худший подходящий блок)
    void* allocate(size_t size) {
        FreeBlock* worstFitBlock = nullptr;
        size_t worstFitSize = 0; // Начальное значение меньше минимального размера блока

        // Поиск худшего подходящего свободного блока
        for (auto block : freeBlocks) {
            if (block->free && block->size >= size && block->size > worstFitSize) {
                worstFitBlock = block;
                worstFitSize = block->size;
            }
        }

        if (worstFitBlock) {
            // Найден подходящий блок
            worstFitBlock->free = false; // Помечаем блок как занятый

            // Добавляем дескриптор границы в начало блока
            size_t* boundaryTag = reinterpret_cast<size_t*>(worstFitBlock);
            *boundaryTag = worstFitBlock->size;

            // Вычисляем адрес начала блока памяти для возврата
            void* allocatedAddress = reinterpret_cast<void*>(reinterpret_cast<char*>(worstFitBlock) + sizeof(size_t));

            // Обновляем список свободных блоков
            updateFreeBlocks();

            return allocatedAddress;
        }

        // Если подходящий блок не найден, вернуть nullptr
        return nullptr;
    }



private:
    // Вспомогательный метод для обновления списка свободных блоков
    void updateFreeBlocks() {
        // Сортируем список свободных блоков по адресам
        std::sort(freeBlocks.begin(), freeBlocks.end(), [](const FreeBlock* a, const FreeBlock* b) {
            return reinterpret_cast<char*>(a) < reinterpret_cast<char*>(b);
            });

        // Объединяем смежные свободные блоки
        auto it = freeBlocks.begin();
        while (it != freeBlocks.end() - 1) {
            FreeBlock* current = *it;
            FreeBlock* next = *(it + 1);
            if (reinterpret_cast<char*>(current) + current->size == reinterpret_cast<char*>(next)) {
                // Объединяем блоки, если они смежные
                current->size += next->size;
                // Удаляем следующий блок из списка
                it = freeBlocks.erase(it + 1);
                delete next;
            }
            else {
                ++it;
            }
        }
    }
};

// Пример использования
int main() {
    // Создание менеджера памяти с общим размером 1MB
    MemoryManager manager(1024 * 1024);

    // Выделение памяти
    void* ptr1 = manager.allocate(100);
    void* ptr2 = manager.allocate(200);
    void* ptr3 = manager.allocate(50);

    // Освобождение памяти
    manager.deallocate(ptr1, 100);
    manager.deallocate(ptr2, 200);
    manager.deallocate(ptr3, 50);

    return 0;
}
