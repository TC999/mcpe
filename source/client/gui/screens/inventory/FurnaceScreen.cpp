
#include "FurnaceScreen.hpp"
//#include "client/app/Minecraft.hpp"
#include "world/tile/FurnaceTile.hpp"
#include "world/inventory/FurnaceMenu.hpp"


FurnaceScreen::FurnaceScreen(Inventory* playerInventory, FurnaceTile* furnace) : ContainerScreen(new FurnaceMenu(furnace, playerInventory))
{
    m_furnace = furnace;
    m_title = "Furnace";
}

FurnaceScreen::~FurnaceScreen()
{
}

void FurnaceScreen::renderBg(float partialTicks, int x, int y)
{
    int texture = m_minecraft->getTextureManager().getTextureId("gui/furnace.png");
    m_minecraft->getTextureManager().bindTexture(texture);
    int xOffset = (width - xSize) / 2;
    int yOffset = (height - ySize) / 2;
    blit(xOffset, yOffset, 0, 0, xSize, ySize);
    if (m_furnace->isBurning())
    {
        int burnTime = m_furnace->getBurnTimeRemainingScaled(12);
        blit(xOffset + 56, yOffset + 36 + 12 - burnTime, 176, 12 - burnTime, 14, burnTime + 1);
    }
    int cookTime = m_furnace->getCookProgressScaled(24);
    blit(xOffset + 79, yOffset + 34, 176, 14, cookTime + 1, 16);
}

void FurnaceScreen::renderLabels()
{
    m_font->draw(m_title, 60, 6, 0x404040);
    m_font->draw("Inventory", 8, ySize - 96 + 2, 0x404040);
}