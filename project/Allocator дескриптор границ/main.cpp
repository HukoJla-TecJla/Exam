//MemoryBlock: ��������� ��� �������� ���������� � ������ ������.
//FreeBlock : ��������� ��� �������� ���������� � ��������� ������ � ������������� ������.
//MemoryManager : ����� ��� ���������� ������������ ������� � �������������� ������������ ������.
//allocate(size_t size) : ����� ��� ��������� ������ ��������� ������� � �������������� ������� ����������� �����.�� ���� ��������� ���� � ���������� ��������, ������� ������ ��� ����� �������������� �������.
//deallocate(void* address, size_t size) : ����� ��� ������������ ����� ������ �� ���������� ������ � �������.��� ������������ ����� ������ ����������� � ������ ��������� ������, � ����� �������� ��������� ����� ������������ ��� ���������� ������������.
#include <iostream>
#include <vector>

// ��������� ��� �������� ���������� � ������ ������
struct MemoryBlock {
    void* address; // ����� ������ ����� ������
    size_t size;   // ������ ����� ������

    MemoryBlock(void* addr, size_t sz) : address(addr), size(sz) {}
};

// ��������� ��� �������� ���������� � ��������� ������ � ������������� ������
struct FreeBlock {
    size_t size;   // ������ ���������� �����
    bool free;     // ����, �����������, �������� �� ����
    FreeBlock* next; // ��������� ��������� ����

    FreeBlock(size_t sz, bool fr) : size(sz), free(fr), next(nullptr) {}
};

// ����� ��� ���������� ������������ ������� � �������������� ������������ ������
class MemoryManager {
private:
    void* memoryPool;     // ��������� �� ������ ������� ������
    size_t memorySize;    // ����� ������ ������
    std::vector<FreeBlock*> freeBlocks; // ������ ��������� ������

public:
    MemoryManager(size_t size) : memorySize(size) {
        // ��������� ������ ��� ����������
        memoryPool = new char[size];

        // ������������� ������� ���������� ����� � ������������� ������
        FreeBlock* initialBlock = new FreeBlock(size, true);
        freeBlocks.push_back(initialBlock);
    }

    ~MemoryManager() {
        // ������������ ������ ��� ����������� �������
        delete[] reinterpret_cast<char*>(memoryPool);
        for (auto block : freeBlocks) {
            delete block;
        }
    }

    // ����� ��� ��������� ������ ��������� ������� (������ ���������� ����)
    void* allocate(size_t size) {
        FreeBlock* bestFitBlock = nullptr;
        size_t bestFitSize = memorySize + 1; // ��������� �������� ������ ������������� �������

        // ����� ������� ����������� ���������� �����
        for (auto block : freeBlocks) {
            if (block->free && block->size >= size && block->size < bestFitSize) {
                bestFitBlock = block;
                bestFitSize = block->size;
            }
        }

        if (bestFitBlock) {
            // ������ ���������� ����
            bestFitBlock->free = false; // �������� ���� ��� �������

            // ��������� ���������� ������� � ������ �����
            size_t* boundaryTag = reinterpret_cast<size_t*>(bestFitBlock);
            *boundaryTag = bestFitBlock->size;

            // ��������� ����� ������ ����� ������ ��� ��������
            void* allocatedAddress = reinterpret_cast<void*>(reinterpret_cast<char*>(bestFitBlock) + sizeof(size_t));

            // ��������� ������ ��������� ������
            updateFreeBlocks();

            return allocatedAddress;
        }

        // ���� ���������� ���� �� ������, ������� nullptr
        return nullptr;
    }

    // ����� ��� ��������� ������ ��������� ������� (������ ���������� ����)
    void* allocate(size_t size) {
        for (auto block : freeBlocks) {
            if (block->free && block->size >= size) {
                // ������ ���������� ����
                block->free = false; // �������� ���� ��� �������

                // ��������� ���������� ������� � ������ �����
                size_t* boundaryTag = reinterpret_cast<size_t*>(block);
                *boundaryTag = block->size;

                // ��������� ����� ������ ����� ������ ��� ��������
                void* allocatedAddress = reinterpret_cast<void*>(reinterpret_cast<char*>(block) + sizeof(size_t));

                // ��������� ������ ��������� ������
                updateFreeBlocks();

                return allocatedAddress;
            }
        }

        // ���� ���������� ���� �� ������, ������� nullptr
        return nullptr;
    }



    // ����� ��� ������������ ����� ������ �� ���������� ������ � �������
    void deallocate(void* address, size_t size) {
        // ���������� ��������� �� ���������� �������
        size_t* boundaryTag = reinterpret_cast<size_t*>(reinterpret_cast<char*>(address) - sizeof(size_t));

        // �������� ������ ����� ������ �� ����������� �������
        size_t blockSize = *boundaryTag;

        // ��������� ��������� �� ������ ���������� ����� ������
        FreeBlock* newFreeBlock = reinterpret_cast<FreeBlock*>(reinterpret_cast<char*>(address) - sizeof(size_t));

        // ������������� ����� ��������� ����
        newFreeBlock->size = blockSize;
        newFreeBlock->free = true;

        // ��������� ���� � ������ ��������� ������
        freeBlocks.push_back(newFreeBlock);

        // ��������� ������ ��������� ������
        updateFreeBlocks();
    }

    // ����� ��� ��������� ������ ��������� ������� (������ ���������� ����)
    void* allocate(size_t size) {
        FreeBlock* worstFitBlock = nullptr;
        size_t worstFitSize = 0; // ��������� �������� ������ ������������ ������� �����

        // ����� ������� ����������� ���������� �����
        for (auto block : freeBlocks) {
            if (block->free && block->size >= size && block->size > worstFitSize) {
                worstFitBlock = block;
                worstFitSize = block->size;
            }
        }

        if (worstFitBlock) {
            // ������ ���������� ����
            worstFitBlock->free = false; // �������� ���� ��� �������

            // ��������� ���������� ������� � ������ �����
            size_t* boundaryTag = reinterpret_cast<size_t*>(worstFitBlock);
            *boundaryTag = worstFitBlock->size;

            // ��������� ����� ������ ����� ������ ��� ��������
            void* allocatedAddress = reinterpret_cast<void*>(reinterpret_cast<char*>(worstFitBlock) + sizeof(size_t));

            // ��������� ������ ��������� ������
            updateFreeBlocks();

            return allocatedAddress;
        }

        // ���� ���������� ���� �� ������, ������� nullptr
        return nullptr;
    }



private:
    // ��������������� ����� ��� ���������� ������ ��������� ������
    void updateFreeBlocks() {
        // ��������� ������ ��������� ������ �� �������
        std::sort(freeBlocks.begin(), freeBlocks.end(), [](const FreeBlock* a, const FreeBlock* b) {
            return reinterpret_cast<char*>(a) < reinterpret_cast<char*>(b);
            });

        // ���������� ������� ��������� �����
        auto it = freeBlocks.begin();
        while (it != freeBlocks.end() - 1) {
            FreeBlock* current = *it;
            FreeBlock* next = *(it + 1);
            if (reinterpret_cast<char*>(current) + current->size == reinterpret_cast<char*>(next)) {
                // ���������� �����, ���� ��� �������
                current->size += next->size;
                // ������� ��������� ���� �� ������
                it = freeBlocks.erase(it + 1);
                delete next;
            }
            else {
                ++it;
            }
        }
    }
};

// ������ �������������
int main() {
    // �������� ��������� ������ � ����� �������� 1MB
    MemoryManager manager(1024 * 1024);

    // ��������� ������
    void* ptr1 = manager.allocate(100);
    void* ptr2 = manager.allocate(200);
    void* ptr3 = manager.allocate(50);

    // ������������ ������
    manager.deallocate(ptr1, 100);
    manager.deallocate(ptr2, 200);
    manager.deallocate(ptr3, 50);

    return 0;
}
