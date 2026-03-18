#include "FurnaceScreen.hpp"
#include "world/tile/FurnaceTile.hpp"
#include "world/inventory/FurnaceMenu.hpp"
#include "renderer/ShaderConstants.hpp"

FurnaceScreen::FurnaceScreen(Inventory* playerInventory, FurnaceTile* furnace) : ContainerScreen(new FurnaceMenu(furnace, playerInventory))
{
    m_furnace = furnace;
}

void FurnaceScreen::_renderBg(float partialTick)
{
    (void)partialTick;
    currentShaderColor = Color::WHITE;
    m_pMinecraft->m_pTextures->loadAndBindTexture("gui/furnace.png");

    blit(m_leftPos, m_topPos, 0, 0, m_imageWidth, m_imageHeight, 0, 0);

    if (m_furnace->isBurning())
    {
        int burnTime = m_furnace->getBurnTimeRemainingScaled(12);
        blit(m_leftPos + 56, m_topPos + 36 + 12 - burnTime, 176, 12 - burnTime, 14, burnTime + 1, 0, 0);
    }

    int cookTime = m_furnace->getCookProgressScaled(24);
    blit(m_leftPos + 79, m_topPos + 34, 176, 14, cookTime + 1, 16, 0, 0);
}

void FurnaceScreen::_renderLabels()
{
    m_pFont->draw(m_furnace->getName(), 60, 6, Color::TEXT_GREY);
    m_pFont->draw(m_pMinecraft->m_pLocalPlayer->m_pInventory->getName(), 8, m_imageHeight - 96 + 2, Color::TEXT_GREY);
}

SlotDisplay FurnaceScreen::_createSlotDisplay(const Slot& slot)
{
    constexpr int slotSize = 18;

    switch (slot.m_group)
    {
    case Slot::OUTPUT:
        return SlotDisplay(116, 35, slotSize);
    case Slot::INPUT:
        if (slot.m_slot == 0)
            return SlotDisplay(56, 17, slotSize);
        if (slot.m_slot == 1)
            return SlotDisplay(56, 53, slotSize);
        return SlotDisplay();
    case Slot::INVENTORY:
        return SlotDisplay(8 + (slot.m_slot % 9) * slotSize, 84 + ((slot.m_slot / 9) - 1) * slotSize, slotSize);
    case Slot::HOTBAR:
        return SlotDisplay(8 + (slot.m_slot % 9) * slotSize, 142, slotSize);
    default:
        return SlotDisplay();
    }
}