#include "FurnaceTile.hpp"
#include "world/level/Level.hpp"
#include "world/phys/Vec3.hpp"
#include "world/Facing.hpp"
#include "world/item/crafting/FurnaceRecipes.hpp"
#include "world/entity/ItemEntity.hpp"
#include "world/entity/Player.hpp"

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
    (void)pos;
    if (level->m_bIsClientSide)
    {
        return true;
    }

    player->openFurnace(this);
    return true;
}

int FurnaceTile::getTexture(Facing::Name face) const
{
    if (face == Facing::UP || face == Facing::DOWN)
    {
        return TEXTURE_FURNACE_TOP;
    }

    if (face == Facing::NORTH)
    {
        return m_isActive ? TEXTURE_FURNACE_LIT : TEXTURE_FURNACE_FRONT;
    }

    return TEXTURE_FURNACE_SIDE;
}

int FurnaceTile::getTexture(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
    if (face == Facing::UP || face == Facing::DOWN)
    {
        return TEXTURE_FURNACE_TOP;
    }

    int meta = level->getData(pos);
    if (face != (Facing::Name)meta)
    {
        return TEXTURE_FURNACE_SIDE;
    }

    return m_isActive ? TEXTURE_FURNACE_LIT : TEXTURE_FURNACE_FRONT;
}

void FurnaceTile::onPlace(Level* level, const TilePos& pos)
{
    Tile::onPlace(level, pos);
    setDefaultDirection(level, pos);
}

void FurnaceTile::setDefaultDirection(Level* level, const TilePos& pos)
{
    if (level->m_bIsClientSide)
    {
        return;
    }

    TileID north = level->getTile(pos.north());
    TileID south = level->getTile(pos.south());
    TileID west = level->getTile(pos.west());
    TileID east = level->getTile(pos.east());
    int direction = Facing::NORTH;

    if (Tile::solid[north] && !Tile::solid[south])
    {
        direction = Facing::NORTH;
    }
    if (Tile::solid[south] && !Tile::solid[north])
    {
        direction = Facing::SOUTH;
    }
    if (Tile::solid[west] && !Tile::solid[east])
    {
        direction = Facing::WEST;
    }
    if (Tile::solid[east] && !Tile::solid[west])
    {
        direction = Facing::EAST;
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
        Vec3 part(x - f3, y, z + f4);
        level->addParticle("smoke", part);
        level->addParticle("flame", part);
    }
    else if (meta == 5) // West
    {
        Vec3 part(x + f3, y, z + f4);
        level->addParticle("smoke", part);
        level->addParticle("flame", part);
    }
    else if (meta == 2) // South
    {
        Vec3 part(x + f4, y, z - f3);
        level->addParticle("smoke", part);
        level->addParticle("flame", part);
    }
    else if (meta == 3) // North
    {
        Vec3 part(x + f4, y, z + f3);
        level->addParticle("smoke", part);
        level->addParticle("flame", part);
    }
}

void FurnaceTile::setPlacedBy(Level* level, const TilePos& pos, Mob* mob)
{
    int rotation = (int)((mob->m_rot.y * 4.0f) / 360.0f + 0.5f) & 3;
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
                while (!item.isEmpty())
                {
                    int count = level->m_random.nextInt(21) + 10;
                    if (count > item.m_count)
                    {
                        count = item.m_count;
                    }
                    ItemStack dropped = item.remove(count);
                    ItemEntity* entityItem = new ItemEntity(level, Vec3((float)pos.x + f, (float)pos.y + f1, (float)pos.z + f2), dropped);
                    float f3 = 0.05f;
                    entityItem->m_vel.x = (float)level->m_random.nextGaussian() * f3;
                    entityItem->m_vel.y = (float)level->m_random.nextGaussian() * f3 + 0.2f;
                    entityItem->m_vel.z = (float)level->m_random.nextGaussian() * f3;
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
    if (!level->m_bIsClientSide)
    {
        if (!isBurning() && canSmelt())
        {
            m_currentItemBurnTime = m_burnTime = FurnaceRecipes::singleton().getBurnDuration(m_items[1]);
            if (isBurning())
            {
                shouldUpdate = true;
                if (!m_items[1].isEmpty())
                {
                    m_items[1].shrink();
                    if (m_items[1].isEmpty())
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
    TileID currentTile = level->getTile(pos);
    int meta = level->getData(pos);

    FurnaceTile* from = nullptr;
    FurnaceTile* to = nullptr;

    if (currentTile == Tile::furnaceIdle->m_ID)
    {
        from = (FurnaceTile*)Tile::furnaceIdle;
        to = (FurnaceTile*)(isBurning ? Tile::furnaceActive : Tile::furnaceIdle);
    }
    else if (currentTile == Tile::furnaceActive->m_ID)
    {
        from = (FurnaceTile*)Tile::furnaceActive;
        to = (FurnaceTile*)(isBurning ? Tile::furnaceActive : Tile::furnaceIdle);
    }

    if (from && to && from != to)
    {
        for (int i = 0; i < 3; ++i)
        {
            to->m_items[i] = from->m_items[i];
        }
        to->m_cookTime = from->m_cookTime;
        to->m_burnTime = from->m_burnTime;
        to->m_currentItemBurnTime = from->m_currentItemBurnTime;
    }

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

    static ItemStack invalid = ItemStack::EMPTY;
    return invalid;
}

ItemStack FurnaceTile::removeItem(int index, int count)
{
    if (index >= 0 && index < 3)
    {
        if (!m_items[index].isEmpty())
        {
            if (m_items[index].m_count <= count)
            {
                ItemStack item = m_items[index];
                m_items[index] = ItemStack::EMPTY;
                return item;
            }
            else
            {
                ItemStack item = m_items[index].remove(count);
                if (m_items[index].isEmpty())
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
        if (!item.isEmpty() && item.m_count > getMaxStackSize())
        {
            m_items[index].set(getMaxStackSize());
        }
    }
}

std::string FurnaceTile::getName() const
{
    return "Furnace";
}

void FurnaceTile::setChanged()
{
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
    int currentItemBurnTime = m_currentItemBurnTime;
    if (currentItemBurnTime == 0)
    {
        currentItemBurnTime = 200;
    }
    return (m_burnTime * scale) / currentItemBurnTime;
}

int FurnaceTile::getCookProgressScaled(int scale) const
{
    return (m_cookTime * scale) / 200;
}

void FurnaceTile::burn(Level* level, const TilePos& pos)
{
    (void)level;
    (void)pos;
    // Implementation depends on the game's burning system
}

void FurnaceTile::smeltItem(Level* level, const TilePos& pos)
{
    (void)level;
    (void)pos;
    if (canSmelt())
    {
        ItemStack& input = m_items[0];
        const ItemStack& output = FurnaceRecipes::singleton().getItemFor(this);
        if (m_items[2].isEmpty())
        {
            m_items[2] = output;
        }
        else if (m_items[2].getId() == output.getId())
        {
            m_items[2].set(m_items[2].m_count + output.m_count);
        }
        input.shrink();
        if (input.isEmpty())
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
    const ItemStack& output = FurnaceRecipes::singleton().getItemFor((Container*)this);
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
    int totalCount = m_items[2].m_count + output.m_count;
    return totalCount <= C_MAX_CONTAINER_STACK_SIZE && totalCount <= output.getMaxStackSize();
}

bool FurnaceTile::m_keepInventory = false;