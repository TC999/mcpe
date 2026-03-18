#include "FurnaceMenu.hpp"
#include "world/entity/Player.hpp"
#include "world/item/Inventory.hpp"
#include "world/inventory/Slot.hpp"
#include "world/inventory/FurnaceResultSlot.hpp"
#include "world/item/crafting/FurnaceRecipes.hpp"

FurnaceMenu::FurnaceMenu(FurnaceTile* furnace, Inventory* playerInventory) : ContainerMenu(Container::FURNACE)
{
    m_furnace = furnace;
    m_cookTime = 0;
    m_burnTime = 0;
    m_currentItemBurnTime = 0;

    // Add furnace slots
    addSlot(new Slot(furnace, 0, Slot::INPUT));
    addSlot(new Slot(furnace, 1, Slot::INPUT));
    addSlot(new FurnaceResultSlot(this, furnace, 2));

    // Add player inventory slots
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            addSlot(new Slot(playerInventory, j + i * 9 + 9, Slot::INVENTORY));
        }
    }

    // Add player hotbar slots
    for (int i = 0; i < 9; i++)
    {
        addSlot(new Slot(playerInventory, i, Slot::HOTBAR));
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
    Slot* slot = getSlot(index);
    if (slot && slot->hasItem())
    {
        ItemStack& itemstack1 = slot->getItem();
        itemstack = itemstack1;
        if (index == 2) // Output slot
        {
            moveItemStackTo(itemstack1, 3, 39, true);
        }
        else if (index == 0 || index == 1)
        {
            moveItemStackTo(itemstack1, 3, 39, false);
        }
        else if (index >= 3 && index < 39)
        {
            if (FurnaceRecipes::singleton().isFurnaceItem(itemstack1))
            {
                moveItemStackTo(itemstack1, 0, 1, false);
            }
            else if (FurnaceRecipes::singleton().getBurnDuration(itemstack1) > 0)
            {
                moveItemStackTo(itemstack1, 1, 2, false);
            }
            else if (index >= 3 && index < 30)
            {
                moveItemStackTo(itemstack1, 30, 39, false);
            }
            else if (index >= 30 && index < 39)
            {
                moveItemStackTo(itemstack1, 3, 30, false);
            }
        }
        else
        {
            moveItemStackTo(itemstack1, 3, 39, false);
        }

        if (itemstack1.isEmpty())
        {
            slot->set(ItemStack::EMPTY);
        }
        else
        {
            slot->setChanged();
        }

        if (itemstack1.m_count == itemstack.m_count)
        {
            return ItemStack::EMPTY;
        }

        slot->onTake(itemstack);
    }

    return itemstack;
}

void FurnaceMenu::slotsChanged(Container* container)
{
    ContainerMenu::slotsChanged(container);

    if (container == m_furnace)
    {
        if (m_cookTime != m_furnace->getCookTime())
        {
            sendData(0, m_furnace->getCookTime());
        }
        if (m_burnTime != m_furnace->getBurnTime())
        {
            sendData(1, m_furnace->getBurnTime());
        }
        if (m_currentItemBurnTime != m_furnace->getCurrentItemBurnTime())
        {
            sendData(2, m_furnace->getCurrentItemBurnTime());
        }

        m_cookTime = m_furnace->getCookTime();
        m_burnTime = m_furnace->getBurnTime();
        m_currentItemBurnTime = m_furnace->getCurrentItemBurnTime();
    }
}