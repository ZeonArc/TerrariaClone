#pragma once
#include <array>
#include <cstdint>

// A simple struct to hold an item and its quantity
struct ItemStack {
    uint16_t blockID = 0; // 0 = Empty
    int count = 0;
};

class Inventory {
public:
    Inventory();

    // Tries to add an item. Returns true if successful, false if inventory is full.
    bool addItem(uint16_t blockID, int amount);

    // Tries to use one item from the currently selected slot.
    bool consumeSelectedItem();

    // Getters and Setters for the UI and Input
    const ItemStack& getSlot(int index) const;
    void setSelectedSlot(int index);
    int getSelectedSlot() const;
    uint16_t getSelectedBlockID() const;

private:
    static const int HOTBAR_SIZE = 9;
    std::array<ItemStack, HOTBAR_SIZE> m_slots;
    int m_selectedSlot;
};