//MemoryBlock: ��������� ��� ������������� ����� ������ � ������� � ��������.
//MemoryManager : �����, ����������� ���������� ������������ ������� � �������������� ������� ���������(buddy system).
//MemoryManager(size_t size) : ����������� ������, ������� �������������� ��� ������ � ��������� ���� �� ������������ ������.
//~MemoryManager() : ���������� ������, ������������� ��� ���������� �������.
//allocate(size_t size) : ����� ��� ��������� ����� ������ ��������� �������, ��������� ������ ���������� ����(���������� ����, ������� ���������� ������� ��� �������������� �������).
//deallocate(void* address, size_t size) : ����� ��� ������������ ����� ������ �� ���������� ������ � �������, ��������� ���� � ��� "����", ���� ��������.
//getBuddyAddress(void* addr, size_t block_size) : ��������������� ����� ��� ��������� ������ "����" �����.
#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>

// ��������� ��� ������������� ����� ������
struct MemoryBlock {
    void* address; // ����� ������ ����� ������
    size_t size;   // ������ ����� ������

    MemoryBlock(void* addr, size_t sz) : address(addr), size(sz) {}
};

// ����� ��� ���������� ������������ ������� � �������������� ������� ��������� (buddy system)
class MemoryManager {
private:
    void* memoryPool;       // ��������� �� ������ ������� ������
    size_t memorySize;      // ����� ������ ������
    int maxLevel;           // ������������ ������� ����������� �������
    std::vector<std::vector<MemoryBlock*>> freeLists; // ������ ��������� ������ �� �������

public:
    MemoryManager(size_t size) : memorySize(size) {
        // ��������� ������ ��� ����������
        memoryPool = new char[size];

        // ���������� ������������� ������ ����������� �������
        maxLevel = log2(size);

        // ������������� ������� ��������� ������ �� �������
        freeLists.resize(maxLevel + 1);
        freeLists[maxLevel].push_back(new MemoryBlock(memoryPool, size));
    }

    ~MemoryManager() {
        // ������������ ������ ��� ����������� �������
        for (int level = maxLevel; level >= 0; --level) {
            for (auto block : freeLists[level]) {
                delete block;
            }
        }
        delete[] reinterpret_cast<char*>(memoryPool);
    }

    // ����� ��� ��������� ������ ��������� ������� (������ ���������� ����)
    void* allocate(size_t size) {
        int level = ceil(log2(size)); // ������� ����������� �������, �� ������� ���� ����� ���������� �������
        for (int l = level; l <= maxLevel; ++l) {
            if (!freeLists[l].empty()) {
                // ������� ������ ���������� ���� �� ������� ������ ��� ����� ������� �������
                MemoryBlock* block = freeLists[l].back();
                freeLists[l].pop_back();

                // ����� ���� �� ��� ���, ���� �� ��������� ������� �������
                while (block->size > size && l > level) {
                    l--;
                    void* buddyAddress = reinterpret_cast<char*>(block->address) + (block->size / 2);
                    freeLists[l].push_back(new MemoryBlock(buddyAddress, block->size / 2));
                    block->size /= 2;
                }

                return block->address;
            }
        }
        return nullptr; // ���� ���������� ���� �� ������
    }

    // ����� ��� ��������� ������ ��������� ������� (������ ���������� ����)
    void* allocate(size_t size) {
        int level = ceil(log2(size)); // ������� ����������� �������, �� ������� ���� ����� ���������� �������
        for (int l = level; l <= maxLevel; ++l) {
            if (!freeLists[l].empty()) {
                // ������� ������ ���������� ���� �� ������� ������ ��� ����� ������� �������
                MemoryBlock* block = freeLists[l].back();
                freeLists[l].pop_back();

                // ����� ���� �� ��� ���, ���� �� ��������� ������� �������
                while (block->size > size && l > level) {
                    l--;
                    void* buddyAddress = reinterpret_cast<char*>(block->address) + (block->size / 2);
                    freeLists[l].push_back(new MemoryBlock(buddyAddress, block->size / 2));
                    block->size /= 2;
                }

                return block->address;
            }
        }
        return nullptr; // ���� ���������� ���� �� ������
    }

    // ����� ��� ��������� ������ ��������� ������� (������ ���������� ����)
    void* allocate(size_t size) {
        int level = ceil(log2(size)); // ������� ����������� �������, �� ������� ���� ����� ���������� �������
        for (int l = maxLevel; l >= level; --l) {
            if (!freeLists[l].empty()) {
                // ������� ������ ���������� ���� �� ������� ������ ��� ����� ������ �������
                MemoryBlock* block = freeLists[l].back();
                freeLists[l].pop_back();

                // ����� ���� �� ��� ���, ���� �� ��������� ������� �������
                while (block->size > size && l > level) {
                    l--;
                    void* buddyAddress = reinterpret_cast<char*>(block->address) + (block->size / 2);
                    freeLists[l].push_back(new MemoryBlock(buddyAddress, block->size / 2));
                    block->size /= 2;
                }

                return block->address;
            }
        }
        return nullptr; // ���� ���������� ���� �� ������
    }

    // ����� ��� ������������ ����� ������ �� ���������� ������ � ������� (������ ����������)
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

   //��� ������� ����������� � ������� �����������
    void deallocate(void* address, size_t size) {
        int level = ceil(log2(size));
        MemoryBlock* block = new MemoryBlock(address, size);
        freeLists[level].push_back(block);
        mergeBlocks(level);
    }
private:
    private:
        // ��������������� ����� ��� ������� ������ �� �������� ������
        void mergeBlocks(int level) {
            while (level < maxLevel) {
                auto it = freeLists[level].begin();
                while (it != freeLists[level].end()) {
                    MemoryBlock* block = *it;
                    void* buddyAddress = getBuddyAddress(block->address, block->size);
                    bool merged = false;
                    for (auto it_next = freeLists[level].begin(); it_next != freeLists[level].end(); ++it_next) {
                        if ((*it_next)->address == buddyAddress) {
                            // ������ ���� �����, ����� ����������
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
    // ��������������� ����� ��� ��������� ������ "����" �����
    void* getBuddyAddress(void* addr, size_t block_size) {
        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(addr) ^ block_size);
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
