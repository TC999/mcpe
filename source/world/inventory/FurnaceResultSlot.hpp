#pragma once

#include "Slot.hpp"

class FurnaceMenu;
class FurnaceTile;

class FurnaceResultSlot : public Slot
{
public:
    FurnaceResultSlot(FurnaceMenu* menu, FurnaceTile* furnace, int index);
    ~FurnaceResultSlot();

public:
    bool mayPlace(const ItemStack& item) const override;
    void onTake(ItemStack& item) override;

private:
    FurnaceMenu* m_menu;
    FurnaceTile* m_furnace;
};