#include "FurnaceResultSlot.hpp"
#include "world/inventory/FurnaceMenu.hpp"
#include "world/tile/FurnaceTile.hpp"

FurnaceResultSlot::FurnaceResultSlot(FurnaceMenu* menu, FurnaceTile* furnace, int index) : Slot(furnace, index, OUTPUT)
{
    m_menu = menu;
    m_furnace = furnace;
}

FurnaceResultSlot::~FurnaceResultSlot()
{
}

bool FurnaceResultSlot::mayPlace(const ItemStack& item) const
{
    (void)item;
    return false;
}

void FurnaceResultSlot::onTake(ItemStack& item)
{
    (void)item;
    setChanged();
}