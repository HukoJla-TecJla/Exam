#include <iostream>

// ��������� ��� �������� ���������� � ������ ������
struct MemoryBlock {
    void* address; // ����� ������ ����� ������
    size_t size;   // ������ ����� ������

    MemoryBlock(void* addr, size_t sz) : address(addr), size(sz) {}
};

// ����� ��� ���������� ������������ �������
class MemoryManager {
private:
    struct Node {
        MemoryBlock* block;
        Node* next;
        Node* prev;

        Node(MemoryBlock* blk) : block(blk), next(nullptr), prev(nullptr) {}
    };

    Node* head; // ������ ������

public:
    MemoryManager() : head(nullptr) {}

    ~MemoryManager() {
        // ������������ ���� ������� ������ ��� ����������� �������
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current->block; // ������������ ����� ������
            delete current;        // ������������ ���� ������
            current = next;
        }
    }

    // ����� ��� ��������� ������ ��������� �������
    void* allocate(size_t size) {
        // ����� ���������� ����������� ����� ������
        Node* current = head;
        while (current) {
            if (current->block->size >= size) {
                // ������ ���������� ����
                void* address = current->block->address;
                // �������� ���� �� ������
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
        // ���� ���������� ���� �� ������, ������� nullptr
        return nullptr;
    }
    //���������� ������ ��� ��������� ������ �� ��������� ������� �����������
    void* allocate(size_t size) {
        Node* current = head;
        while (current) {
            if (current->block->size >= size) {
                // ������ ���������� ����
                void* address = current->block->address;
                // �������� ���� �� ������
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
        // ���� ���������� ���� �� ������, ������� nullptr
        return nullptr;
    }

    // ���������� ������ ��� ��������� ������ �� ��������� ���������� �����������
    void* allocate(size_t size) {
        Node* current = head;
        Node* bestFitNode = nullptr;
        size_t maxBlockSize = 0;

        // ����� ������� ����������� ����� (���������� ������ >= size)
        while (current) {
            if (current->block->size >= size && (bestFitNode == nullptr || current->block->size > maxBlockSize)) {
                bestFitNode = current;
                maxBlockSize = current->block->size;
            }
            current = current->next;
        }

        if (bestFitNode) {
            // ������ ���������� ����
            void* address = bestFitNode->block->address;
            // �������� ���� �� ������
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

        // ���� ���������� ���� �� ������, ������� nullptr
        return nullptr;
    }

    // ����� ��� ������������ ����� ������ �� ���������� ������ � �������
    void deallocate(void* address, size_t size) {
        // �������� ������ ����� ������
        MemoryBlock* newBlock = new MemoryBlock(address, size);
        // ������� ������ ����� ������ � ��������������� ������ �� ������
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

// ������ �������������
int main() {
    MemoryManager manager;

    // ��������� ������
    void* ptr1 = manager.allocate(100);
    void* ptr2 = manager.allocate(50);
    void* ptr3 = manager.allocate(200);

    // ������������ ������
    manager.deallocate(ptr1, 100);
    manager.deallocate(ptr2, 50);
    manager.deallocate(ptr3, 200);

    return 0;
}
