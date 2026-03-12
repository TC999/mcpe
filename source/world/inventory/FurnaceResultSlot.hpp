#pragma once

#include "Slot.hpp"

class FurnaceMenu;
class FurnaceTile;

class FurnaceResultSlot : public Slot
{
public:
    FurnaceResultSlot(FurnaceMenu* menu, FurnaceTile* furnace, int index, int x, int y);
    ~FurnaceResultSlot();

public:
    bool isItemValid(const ItemStack& item) const override;
    void onTake(ItemStack& item) override;

private:
    FurnaceMenu* m_menu;
    FurnaceTile* m_furnace;
};