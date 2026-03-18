#pragma once

#include "ContainerScreen.hpp"

class FurnaceTile;

class FurnaceScreen : public ContainerScreen
{
public:
    FurnaceScreen(Inventory* playerInventory, FurnaceTile* furnace);

protected:
    void _renderLabels() override;
    void _renderBg(float partialTick) override;
    SlotDisplay _createSlotDisplay(const Slot&) override;

private:
    FurnaceTile* m_furnace;
};