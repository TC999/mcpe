/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FurnaceResultSlot.hpp"
#include "world/inventory/FurnaceMenu.hpp"
#include "world/tile/FurnaceTile.hpp"

FurnaceResultSlot::FurnaceResultSlot(FurnaceMenu* menu, FurnaceTile* furnace, int index, int x, int y) : Slot(furnace, index, x, y)
{
    m_menu = menu;
    m_furnace = furnace;
}

FurnaceResultSlot::~FurnaceResultSlot()
{
}

bool FurnaceResultSlot::isItemValid(const ItemStack& item) const
{
    return false;
}

void FurnaceResultSlot::onTake(ItemStack& item)
{
    // Implementation depends on the game's crafting system
}