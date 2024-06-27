#include <iostream>

// Структура для хранения информации о блоках памяти
struct MemoryBlock {
    void* address; // Адрес начала блока памяти
    size_t size;   // Размер блока памяти

    MemoryBlock(void* addr, size_t sz) : address(addr), size(sz) {}
};

// Класс для управления динамической памятью
class MemoryManager {
private:
    struct Node {
        MemoryBlock* block;
        Node* next;
        Node* prev;

        Node(MemoryBlock* blk) : block(blk), next(nullptr), prev(nullptr) {}
    };

    Node* head; // Голова списка

public:
    MemoryManager() : head(nullptr) {}

    ~MemoryManager() {
        // Освобождение всех занятых блоков при уничтожении объекта
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current->block; // Освобождение блока памяти
            delete current;        // Освобождение узла списка
            current = next;
        }
    }

    // Метод для выделения памяти заданного размера
    void* allocate(size_t size) {
        // Поиск наилучшего подходящего блока памяти
        Node* current = head;
        while (current) {
            if (current->block->size >= size) {
                // Найден подходящий блок
                void* address = current->block->address;
                // Удаление узла из списка
                if (current == head) {
                    head = current->next;
                    if (head) head->prev = nullptr;
                }
                else {
                    current->prev->next = current->next;
                    if (current->next) current->next->prev = current->prev;
                }
                delete current->block;
                delete current;
                return address;
            }
            current = current->next;
        }
        // Если подходящий блок не найден, вернуть nullptr
        return nullptr;
    }
    //Реализация метода для выделения памяти по алгоритму первого подходящего
    void* allocate(size_t size) {
        Node* current = head;
        while (current) {
            if (current->block->size >= size) {
                // Найден подходящий блок
                void* address = current->block->address;
                // Удаление узла из списка
                if (current == head) {
                    head = current->next;
                    if (head) head->prev = nullptr;
                }
                else {
                    current->prev->next = current->next;
                    if (current->next) current->next->prev = current->prev;
                }
                delete current->block;
                delete current;
                return address;
            }
            current = current->next;
        }
        // Если подходящий блок не найден, вернуть nullptr
        return nullptr;
    }

    // Реализация метода для выделения памяти по алгоритму наихудшего подходящего
    void* allocate(size_t size) {
        Node* current = head;
        Node* bestFitNode = nullptr;
        size_t maxBlockSize = 0;

        // Поиск худшего подходящего блока (наибольший размер >= size)
        while (current) {
            if (current->block->size >= size && (bestFitNode == nullptr || current->block->size > maxBlockSize)) {
                bestFitNode = current;
                maxBlockSize = current->block->size;
            }
            current = current->next;
        }

        if (bestFitNode) {
            // Найден подходящий блок
            void* address = bestFitNode->block->address;
            // Удаление узла из списка
            if (bestFitNode == head) {
                head = bestFitNode->next;
                if (head) head->prev = nullptr;
            }
            else {
                bestFitNode->prev->next = bestFitNode->next;
                if (bestFitNode->next) bestFitNode->next->prev = bestFitNode->prev;
            }
            delete bestFitNode->block;
            delete bestFitNode;
            return address;
        }

        // Если подходящий блок не найден, вернуть nullptr
        return nullptr;
    }

    // Метод для освобождения блока памяти по указанному адресу и размеру
    void deallocate(void* address, size_t size) {
        // Создание нового блока памяти
        MemoryBlock* newBlock = new MemoryBlock(address, size);
        // Вставка нового блока памяти в отсортированный список по адресу
        Node* newNode = new Node(newBlock);
        Node* current = head;
        while (current) {
            if (current->block->address > address)
                break;
            current = current->next;
        }
        if (current == head) {
            newNode->next = head;
            if (head) head->prev = newNode;
            head = newNode;
        }
        else {
            newNode->next = current;
            newNode->prev = current->prev;
            current->prev = newNode;
            if (newNode->prev) newNode->prev->next = newNode;
        }
    }
};

// Пример использования
int main() {
    MemoryManager manager;

    // Выделение памяти
    void* ptr1 = manager.allocate(100);
    void* ptr2 = manager.allocate(50);
    void* ptr3 = manager.allocate(200);

    // Освобождение памяти
    manager.deallocate(ptr1, 100);
    manager.deallocate(ptr2, 50);
    manager.deallocate(ptr3, 200);

    return 0;
}
