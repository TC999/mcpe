#pragma once

#include "ContainerMenu.hpp"
#include "world/tile/FurnaceTile.hpp"

class FurnaceMenu : public ContainerMenu
{
public:
    FurnaceMenu(FurnaceTile* furnace, Inventory* playerInventory);
    ~FurnaceMenu();

public:
    bool stillValid(Player* player) const override;
    ItemStack quickMoveStack(int index) override;
    void slotsChanged(Container* container) override;

private:
    FurnaceTile* m_furnace;
    int m_cookTime;
    int m_burnTime;
    int m_currentItemBurnTime;
};