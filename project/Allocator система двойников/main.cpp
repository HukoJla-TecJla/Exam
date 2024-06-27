//MemoryBlock: Структура для представления блока памяти с адресом и размером.
//MemoryManager : Класс, реализующий управление динамической памятью с использованием системы двойников(buddy system).
//MemoryManager(size_t size) : Конструктор класса, который инициализирует пул памяти и начальный блок на максимальном уровне.
//~MemoryManager() : Деструктор класса, освобождающий все выделенные ресурсы.
//allocate(size_t size) : Метод для выделения блока памяти заданного размера, используя лучший подходящий блок(наименьший блок, который достаточно большой для запрашиваемого размера).
//deallocate(void* address, size_t size) : Метод для освобождения блока памяти по указанному адресу и размеру, объединяя блок с его "бати", если возможно.
//getBuddyAddress(void* addr, size_t block_size) : Вспомогательный метод для получения адреса "бати" блока.
#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>

// Структура для представления блока памяти
struct MemoryBlock {
    void* address; // Адрес начала блока памяти
    size_t size;   // Размер блока памяти

    MemoryBlock(void* addr, size_t sz) : address(addr), size(sz) {}
};

// Класс для управления динамической памятью с использованием системы двойников (buddy system)
class MemoryManager {
private:
    void* memoryPool;       // Указатель на начало области памяти
    size_t memorySize;      // Общий размер памяти
    int maxLevel;           // Максимальный уровень двойниковой системы
    std::vector<std::vector<MemoryBlock*>> freeLists; // Списки свободных блоков по уровням

public:
    MemoryManager(size_t size) : memorySize(size) {
        // Выделение памяти для управления
        memoryPool = new char[size];

        // Вычисление максимального уровня двойниковой системы
        maxLevel = log2(size);

        // Инициализация списков свободных блоков по уровням
        freeLists.resize(maxLevel + 1);
        freeLists[maxLevel].push_back(new MemoryBlock(memoryPool, size));
    }

    ~MemoryManager() {
        // Освобождение памяти при уничтожении объекта
        for (int level = maxLevel; level >= 0; --level) {
            for (auto block : freeLists[level]) {
                delete block;
            }
        }
        delete[] reinterpret_cast<char*>(memoryPool);
    }

    // Метод для выделения памяти заданного размера (лучший подходящий блок)
    void* allocate(size_t size) {
        int level = ceil(log2(size)); // Находим минимальный уровень, на котором блок будет достаточно большим
        for (int l = level; l <= maxLevel; ++l) {
            if (!freeLists[l].empty()) {
                // Находим первый подходящий блок на текущем уровне или более высоких уровнях
                MemoryBlock* block = freeLists[l].back();
                freeLists[l].pop_back();

                // Делим блок до тех пор, пока не достигнем нужного размера
                while (block->size > size && l > level) {
                    l--;
                    void* buddyAddress = reinterpret_cast<char*>(block->address) + (block->size / 2);
                    freeLists[l].push_back(new MemoryBlock(buddyAddress, block->size / 2));
                    block->size /= 2;
                }

                return block->address;
            }
        }
        return nullptr; // Если подходящий блок не найден
    }

    // Метод для выделения памяти заданного размера (первый подходящий блок)
    void* allocate(size_t size) {
        int level = ceil(log2(size)); // Находим минимальный уровень, на котором блок будет достаточно большим
        for (int l = level; l <= maxLevel; ++l) {
            if (!freeLists[l].empty()) {
                // Находим первый подходящий блок на текущем уровне или более высоких уровнях
                MemoryBlock* block = freeLists[l].back();
                freeLists[l].pop_back();

                // Делим блок до тех пор, пока не достигнем нужного размера
                while (block->size > size && l > level) {
                    l--;
                    void* buddyAddress = reinterpret_cast<char*>(block->address) + (block->size / 2);
                    freeLists[l].push_back(new MemoryBlock(buddyAddress, block->size / 2));
                    block->size /= 2;
                }

                return block->address;
            }
        }
        return nullptr; // Если подходящий блок не найден
    }

    // Метод для выделения памяти заданного размера (худший подходящий блок)
    void* allocate(size_t size) {
        int level = ceil(log2(size)); // Находим минимальный уровень, на котором блок будет достаточно большим
        for (int l = maxLevel; l >= level; --l) {
            if (!freeLists[l].empty()) {
                // Находим худший подходящий блок на текущем уровне или более низких уровнях
                MemoryBlock* block = freeLists[l].back();
                freeLists[l].pop_back();

                // Делим блок до тех пор, пока не достигнем нужного размера
                while (block->size > size && l > level) {
                    l--;
                    void* buddyAddress = reinterpret_cast<char*>(block->address) + (block->size / 2);
                    freeLists[l].push_back(new MemoryBlock(buddyAddress, block->size / 2));
                    block->size /= 2;
                }

                return block->address;
            }
        }
        return nullptr; // Если подходящий блок не найден
    }

    // Метод для освобождения блока памяти по указанному адресу и размеру (Лучший подходящий)
    void deallocate(void* address, size_t size) {
        int level = ceil(log2(size));
        MemoryBlock* block = new MemoryBlock(address, size);
        while (level <= maxLevel) {
            void* buddyAddress = getBuddyAddress(block->address, block->size);
            bool merged = false;
            for (auto it = freeLists[level].begin(); it != freeLists[level].end(); ++it) {
                if ((*it)->address == buddyAddress) {
                    block->address = std::min(block->address, buddyAddress);
                    block->size *= 2;
                    delete (*it);
                    freeLists[level].erase(it);
                    merged = true;
                    break;
                }
            }
            if (!merged) {
                freeLists[level].push_back(block);
                return;
            }
            level++;
        }
    }

   //Для первого подходящего и худшего подходящего
    void deallocate(void* address, size_t size) {
        int level = ceil(log2(size));
        MemoryBlock* block = new MemoryBlock(address, size);
        freeLists[level].push_back(block);
        mergeBlocks(level);
    }
private:
    private:
        // Вспомогательный метод для слияния блоков на заданном уровне
        void mergeBlocks(int level) {
            while (level < maxLevel) {
                auto it = freeLists[level].begin();
                while (it != freeLists[level].end()) {
                    MemoryBlock* block = *it;
                    void* buddyAddress = getBuddyAddress(block->address, block->size);
                    bool merged = false;
                    for (auto it_next = freeLists[level].begin(); it_next != freeLists[level].end(); ++it_next) {
                        if ((*it_next)->address == buddyAddress) {
                            // Найден батя блока, можно объединять
                            block->address = std::min(block->address, buddyAddress);
                            block->size *= 2;
                            delete* it_next;
                            freeLists[level].erase(it_next);
                            merged = true;
                            break;
                        }
                    }
                    if (!merged) {
                        ++it;
                    }
                }
                ++level;
            }
        }
    // Вспомогательный метод для получения адреса "бати" блока
    void* getBuddyAddress(void* addr, size_t block_size) {
        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(addr) ^ block_size);
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
