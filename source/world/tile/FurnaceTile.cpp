#include "FurnaceTile.hpp"
#include "world/level/Level.hpp"
#include "world/phys/Vec3.hpp"
#include "world/Facing.hpp"
#include "world/inventory/FurnaceMenu.hpp"
#include "world/item/crafting/FurnaceRecipes.hpp"

FurnaceTile::FurnaceTile(TileID id, bool isActive) : Tile(id, Material::stone)
{
    m_isActive = isActive;
    setDescriptionId("furnace");
    setSoundType(SOUND_STONE);
    setDestroyTime(3.5f);
    if (isActive)
    {
        setLightEmission(0.875f);
    }
    setTicking(true);

    // Initialize inventory
    for (int i = 0; i < 3; i++)
    {
        m_items[i] = ItemStack::EMPTY;
    }
    m_cookTime = 0;
    m_burnTime = 0;
    m_currentItemBurnTime = 0;
}

bool FurnaceTile::use(Level* level, const TilePos& pos, Player* player)
{
    if (level->isClientSide())
    {
        return true;
    }
    else
    {
        player->displayGUIFurnace(pos);
        return true;
    }
}

int FurnaceTile::getTexture(Facing::Name face) const
{
    if (face == Facing::TOP || face == Facing::BOTTOM)
    {
        return 45 + 17;
    }
    if (face == Facing::NORTH)
    {
        return 45 - 1;
    }
    else
    {
        return 45;
    }
}

int FurnaceTile::getTexture(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
    if (face == Facing::TOP || face == Facing::BOTTOM)
    {
        return 45 + 17;
    }
    int meta = level->getData(pos);
    if (face != (Facing::Name)meta)
    {
        return 45;
    }
    if (m_isActive)
    {
        return 45 + 16;
    }
    else
    {
        return 45 - 1;
    }
}

void FurnaceTile::onPlace(Level* level, const TilePos& pos)
{
    Tile::onPlace(level, pos);
    setDefaultDirection(level, pos);
}

void FurnaceTile::setDefaultDirection(Level* level, const TilePos& pos)
{
    if (level->isClientSide())
    {
        return;
    }
    int north = level->getTile(pos.add(0, 0, -1)).m_ID;
    int south = level->getTile(pos.add(0, 0, 1)).m_ID;
    int west = level->getTile(pos.add(-1, 0, 0)).m_ID;
    int east = level->getTile(pos.add(1, 0, 0)).m_ID;
    byte direction = 3; // North
    if (Tile::solid[north] && !Tile::solid[south])
    {
        direction = 3;
    }
    if (Tile::solid[south] && !Tile::solid[north])
    {
        direction = 2;
    }
    if (Tile::solid[west] && !Tile::solid[east])
    {
        direction = 5;
    }
    if (Tile::solid[east] && !Tile::solid[west])
    {
        direction = 4;
    }
    level->setData(pos, direction);
}

void FurnaceTile::randomDisplayTick(Level* level, const TilePos& pos, Random* random)
{
    if (!m_isActive)
    {
        return;
    }
    int meta = level->getData(pos);
    float x = (float)pos.x + 0.5f;
    float y = (float)pos.y + 0.0f + (random->nextFloat() * 6.0f) / 16.0f;
    float z = (float)pos.z + 0.5f;
    float f3 = 0.52f;
    float f4 = random->nextFloat() * 0.6f - 0.3f;
    if (meta == 4) // East
    {
        level->addParticle("smoke", x - f3, y, z + f4, 0.0f, 0.0f, 0.0f);
        level->addParticle("flame", x - f3, y, z + f4, 0.0f, 0.0f, 0.0f);
    }
    else if (meta == 5) // West
    {
        level->addParticle("smoke", x + f3, y, z + f4, 0.0f, 0.0f, 0.0f);
        level->addParticle("flame", x + f3, y, z + f4, 0.0f, 0.0f, 0.0f);
    }
    else if (meta == 2) // South
    {
        level->addParticle("smoke", x + f4, y, z - f3, 0.0f, 0.0f, 0.0f);
        level->addParticle("flame", x + f4, y, z - f3, 0.0f, 0.0f, 0.0f);
    }
    else if (meta == 3) // North
    {
        level->addParticle("smoke", x + f4, y, z + f3, 0.0f, 0.0f, 0.0f);
        level->addParticle("flame", x + f4, y, z + f3, 0.0f, 0.0f, 0.0f);
    }
}

void FurnaceTile::setPlacedBy(Level* level, const TilePos& pos, Mob* mob)
{
    int rotation = (int)((mob->rotationYaw * 4.0f) / 360.0f + 0.5f) & 3;
    if (rotation == 0)
    {
        level->setData(pos, 2); // South
    }
    if (rotation == 1)
    {
        level->setData(pos, 5); // West
    }
    if (rotation == 2)
    {
        level->setData(pos, 3); // North
    }
    if (rotation == 3)
    {
        level->setData(pos, 4); // East
    }
}

void FurnaceTile::onRemove(Level* level, const TilePos& pos)
{
    if (!m_keepInventory)
    {
        for (int i = 0; i < 3; i++)
        {
            ItemStack item = m_items[i];
            if (!item.isEmpty())
            {
                float f = level->m_random.nextFloat() * 0.8f + 0.1f;
                float f1 = level->m_random.nextFloat() * 0.8f + 0.1f;
                float f2 = level->m_random.nextFloat() * 0.8f + 0.1f;
                while (item.getCount() > 0)
                {
                    int count = level->m_random.nextInt(21) + 10;
                    if (count > item.getCount())
                    {
                        count = item.getCount();
                    }
                    item.setCount(item.getCount() - count);
                    EntityItem* entityItem = new EntityItem(level, (float)pos.x + f, (float)pos.y + f1, (float)pos.z + f2, ItemStack(item));
                    float f3 = 0.05f;
                    entityItem->motionX = (float)level->m_random.nextGaussian() * f3;
                    entityItem->motionY = (float)level->m_random.nextGaussian() * f3 + 0.2f;
                    entityItem->motionZ = (float)level->m_random.nextGaussian() * f3;
                    level->addEntity(entityItem);
                }
            }
        }
    }
    Tile::onRemove(level, pos);
}

void FurnaceTile::tick(Level* level, const TilePos& pos, Random* random)
{
    bool wasBurning = isBurning();
    bool shouldUpdate = false;
    if (isBurning())
    {
        m_burnTime--;
    }
    if (!level->isClientSide())
    {
        if (!isBurning() && !m_items[0].isEmpty())
        {
            m_currentItemBurnTime = m_burnTime = FurnaceRecipes::singleton().getBurnDuration(m_items[1]);
            if (isBurning())
            {
                shouldUpdate = true;
                if (!m_items[1].isEmpty())
                {
                    m_items[1].setCount(m_items[1].getCount() - 1);
                    if (m_items[1].getCount() == 0)
                    {
                        m_items[1] = ItemStack::EMPTY;
                    }
                }
            }
        }
        if (isBurning() && canSmelt())
        {
            m_cookTime++;
            if (m_cookTime == 200)
            {
                m_cookTime = 0;
                smeltItem(level, pos);
                shouldUpdate = true;
            }
        }
        else
        {
            m_cookTime = 0;
        }
        if (wasBurning != isBurning())
        {
            shouldUpdate = true;
            updateFurnaceBlockState(isBurning(), level, pos);
        }
    }
    if (shouldUpdate)
    {
        level->setTilesDirty(pos, pos);
    }
}

void FurnaceTile::updateFurnaceBlockState(bool isBurning, Level* level, const TilePos& pos)
{
    int meta = level->getData(pos);
    m_keepInventory = true;
    if (isBurning)
    {
        level->setTile(pos, Tile::furnaceActive->m_ID);
    }
    else
    {
        level->setTile(pos, Tile::furnaceIdle->m_ID);
    }
    m_keepInventory = false;
    level->setData(pos, meta);
}

void FurnaceTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
    Tile::neighborChanged(level, pos, tile);
}

Tile* FurnaceTile::setDescriptionId(std::string const& descriptionId)
{
    return Tile::setDescriptionId(descriptionId);
}

void FurnaceTile::triggerEvent(Level* level, const TileEvent& event)
{
    Tile::triggerEvent(level, event);
}

// Inventory methods
uint16_t FurnaceTile::getContainerSize() const
{
    return 3;
}

ItemStack& FurnaceTile::getItem(int index)
{
    if (index >= 0 && index < 3)
    {
        return m_items[index];
    }
    return ItemStack::EMPTY;
}

ItemStack FurnaceTile::removeItem(int index, int count)
{
    if (index >= 0 && index < 3)
    {
        if (!m_items[index].isEmpty())
        {
            if (m_items[index].getCount() <= count)
            {
                ItemStack item = m_items[index];
                m_items[index] = ItemStack::EMPTY;
                return item;
            }
            else
            {
                ItemStack item = m_items[index].split(count);
                if (m_items[index].getCount() == 0)
                {
                    m_items[index] = ItemStack::EMPTY;
                }
                return item;
            }
        }
    }
    return ItemStack::EMPTY;
}

void FurnaceTile::setItem(int index, const ItemStack& item)
{
    if (index >= 0 && index < 3)
    {
        m_items[index] = item;
        if (!item.isEmpty() && item.getCount() > getMaxStackSize())
        {
            m_items[index].setCount(getMaxStackSize());
        }
    }
}

std::string FurnaceTile::getName() const
{
    return "Furnace";
}

bool FurnaceTile::stillValid(Player* player) const
{
    return true;
}

// Furnace methods
bool FurnaceTile::isBurning() const
{
    return m_burnTime > 0;
}

int FurnaceTile::getBurnTimeRemainingScaled(int scale) const
{
    if (m_currentItemBurnTime == 0)
    {
        m_currentItemBurnTime = 200;
    }
    return (m_burnTime * scale) / m_currentItemBurnTime;
}

int FurnaceTile::getCookProgressScaled(int scale) const
{
    return (m_cookTime * scale) / 200;
}

void FurnaceTile::burn(Level* level, const TilePos& pos)
{
    // Implementation depends on the game's burning system
}

void FurnaceTile::smeltItem(Level* level, const TilePos& pos)
{
    if (canSmelt())
    {
        ItemStack input = m_items[0];
        ItemStack output = FurnaceRecipes::singleton().getItemFor(this);
        if (m_items[2].isEmpty())
        {
            m_items[2] = output.copy();
        }
        else if (m_items[2].getId() == output.getId())
        {
            m_items[2].setCount(m_items[2].getCount() + output.getCount());
        }
        input.setCount(input.getCount() - 1);
        if (input.getCount() == 0)
        {
            m_items[0] = ItemStack::EMPTY;
        }
    }
}

bool FurnaceTile::canSmelt() const
{
    if (m_items[0].isEmpty())
    {
        return false;
    }
    ItemStack output = FurnaceRecipes::singleton().getItemFor(this);
    if (output.isEmpty())
    {
        return false;
    }
    if (m_items[2].isEmpty())
    {
        return true;
    }
    if (m_items[2].getId() != output.getId())
    {
        return false;
    }
    int totalCount = m_items[2].getCount() + output.getCount();
    return totalCount <= getMaxStackSize() && totalCount <= output.getMaxStackSize();
}

bool FurnaceTile::m_keepInventory = false;