#pragma once

#include "ContainerScreen.hpp"

class FurnaceTile;

class FurnaceScreen : public ContainerScreen
{
public:
    FurnaceScreen(Inventory* playerInventory, FurnaceTile* furnace);
    ~FurnaceScreen();

protected:
    void renderBg(float partialTicks, int x, int y) override;
    void renderLabels() override;

private:
    FurnaceTile* m_furnace;
};