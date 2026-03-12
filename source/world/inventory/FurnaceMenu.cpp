/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FurnaceMenu.hpp"
#include "world/entity/Player.hpp"
#include "world/item/Inventory.hpp"
#include "world/inventory/Slot.hpp"
#include "world/inventory/FurnaceResultSlot.hpp"

FurnaceMenu::FurnaceMenu(FurnaceTile* furnace, Inventory* playerInventory) : ContainerMenu(Container::FURNACE)
{
    m_furnace = furnace;
    m_cookTime = 0;
    m_burnTime = 0;
    m_currentItemBurnTime = 0;

    // Add furnace slots
    addSlot(new Slot(furnace, 0, 56, 17)); // Input slot
    addSlot(new Slot(furnace, 1, 56, 53)); // Fuel slot
    addSlot(new FurnaceResultSlot(this, furnace, 2, 116, 35)); // Output slot

    // Add player inventory slots
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            addSlot(new Slot(playerInventory, j + i * 9 + 9, 8 + j * 18, 84 + i * 18));
        }
    }

    // Add player hotbar slots
    for (int i = 0; i < 9; i++)
    {
        addSlot(new Slot(playerInventory, i, 8 + i * 18, 142));
    }
}

FurnaceMenu::~FurnaceMenu()
{
}

bool FurnaceMenu::stillValid(Player* player) const
{
    return m_furnace->stillValid(player);
}

ItemStack FurnaceMenu::quickMoveStack(int index)
{
    ItemStack itemstack = ItemStack::EMPTY;
    Slot* slot = m_slots[index];
    if (slot && slot->hasItem())
    {
        ItemStack itemstack1 = slot->getItem();
        itemstack = itemstack1.copy();
        if (index == 2) // Output slot
        {
            if (!moveItemStackTo(itemstack1, 3, 39, true))
            {
                return ItemStack::EMPTY;
            }
        }
        else if (index >= 3 && index < 30) // Player inventory
        {
            if (!moveItemStackTo(itemstack1, 0, 2, false))
            {
                if (!moveItemStackTo(itemstack1, 30, 39, false))
                {
                    return ItemStack::EMPTY;
                }
            }
        }
        else if (index >= 30 && index < 39) // Player hotbar
        {
            if (!moveItemStackTo(itemstack1, 0, 2, false))
            {
                if (!moveItemStackTo(itemstack1, 3, 30, false))
                {
                    return ItemStack::EMPTY;
                }
            }
        }
        else if (!moveItemStackTo(itemstack1, 3, 39, false))
        {
            return ItemStack::EMPTY;
        }
        if (itemstack1.isEmpty())
        {
            slot->set(ItemStack::EMPTY);
        }
        else
        {
            slot->onSlotChanged();
        }
        if (itemstack1.getCount() == itemstack.getCount())
        {
            return ItemStack::EMPTY;
        }
        slot->onTake(itemstack1);
    }
    return itemstack;
}

void FurnaceMenu::slotsChanged(Container* container)
{
    ContainerMenu::slotsChanged(container);
    if (container == m_furnace)
    {
        for (int i = 0; i < m_listeners.size(); i++)
        {
            m_listeners[i]->containerChanged(this, 0, m_furnace->m_cookTime);
            m_listeners[i]->containerChanged(this, 1, m_furnace->m_burnTime);
            m_listeners[i]->containerChanged(this, 2, m_furnace->m_currentItemBurnTime);
        }
        m_cookTime = m_furnace->m_cookTime;
        m_burnTime = m_furnace->m_burnTime;
        m_currentItemBurnTime = m_furnace->m_currentItemBurnTime;
    }
}