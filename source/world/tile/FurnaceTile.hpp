#pragma once

#include "Tile.hpp"
#include "world/Container.hpp"

class FurnaceTile : public Tile, public Container
{
public:
    FurnaceTile(TileID id, bool isActive);

public:
    bool use(Level*, const TilePos& pos, Player*) override;
    int getTexture(Facing::Name face) const override;
    int getTexture(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
    void onPlace(Level*, const TilePos& pos) override;
    void onRemove(Level*, const TilePos& pos) override;
    void randomDisplayTick(Level*, const TilePos& pos, Random*) override;
    void setPlacedBy(Level*, const TilePos& pos, Mob*) override;
    void neighborChanged(Level*, const TilePos& pos, TileID tile) override;
    Tile* setDescriptionId(std::string const&) override;
    void triggerEvent(Level*, const TileEvent& event) override;
    void tick(Level*, const TilePos& pos, Random*) override;

public:
    static void updateFurnaceBlockState(bool isBurning, Level* level, const TilePos& pos);

    // Inventory methods
    uint16_t getContainerSize() const override;
    ItemStack& getItem(int index) override;
    ItemStack removeItem(int index, int count) override;
    void setItem(int index, const ItemStack& item) override;
    std::string getName() const override;
    void setChanged() override;
    bool stillValid(Player* player) const override;

    int getCookTime() const { return m_cookTime; }
    int getBurnTime() const { return m_burnTime; }
    int getCurrentItemBurnTime() const { return m_currentItemBurnTime; }

    // Furnace methods
    bool isBurning() const;
    int getBurnTimeRemainingScaled(int scale) const;
    int getCookProgressScaled(int scale) const;

private:
    void setDefaultDirection(Level* level, const TilePos& pos);
    void burn(Level* level, const TilePos& pos);
    void smeltItem(Level* level, const TilePos& pos);
    bool canSmelt() const;

private:
    bool m_isActive;
    static bool m_keepInventory;

    // Furnace state
    ItemStack m_items[3];
    int m_cookTime;
    int m_burnTime;
    int m_currentItemBurnTime;
};