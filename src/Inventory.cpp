#include "Inventory.h"
#include <algorithm>

Inventory::Inventory() : m_selectedSlot(0) {
    // Start with an empty inventory
    for (int i = 0; i < HOTBAR_SIZE; ++i) {
        m_slots[i] = { 0, 0 };
    }
}

bool Inventory::addItem(uint16_t blockID, int amount) {
    if (blockID == 0) return false; // Don't pick up Air!

    // 1. Try to find an existing stack of this block
    for (int i = 0; i < HOTBAR_SIZE; ++i) {
        if (m_slots[i].blockID == blockID && m_slots[i].count < 99) {
            m_slots[i].count += amount;
            return true;
        }
    }

    // 2. If we don't have this block yet, find the first empty slot
    for (int i = 0; i < HOTBAR_SIZE; ++i) {
        if (m_slots[i].blockID == 0) {
            m_slots[i].blockID = blockID;
            m_slots[i].count = amount;
            return true;
        }
    }

    // Inventory is full!
    return false;
}

bool Inventory::consumeSelectedItem() {
    if (m_slots[m_selectedSlot].blockID != 0 && m_slots[m_selectedSlot].count > 0) {
        m_slots[m_selectedSlot].count--;

        // If we run out, clear the slot
        if (m_slots[m_selectedSlot].count <= 0) {
            m_slots[m_selectedSlot].blockID = 0;
            m_slots[m_selectedSlot].count = 0;
        }
        return true;
    }
    return false;
}

const ItemStack& Inventory::getSlot(int index) const {
    // Safety check
    if (index < 0 || index >= HOTBAR_SIZE) return m_slots[0];
    return m_slots[index];
}

void Inventory::setSelectedSlot(int index) {
    if (index >= 0 && index < HOTBAR_SIZE) {
        m_selectedSlot = index;
    }
}

int Inventory::getSelectedSlot() const {
    return m_selectedSlot;
}

uint16_t Inventory::getSelectedBlockID() const {
    return m_slots[m_selectedSlot].blockID;
}