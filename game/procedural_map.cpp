#include <cstring>
#include <time.h>

struct MapNode
{
	uint16 type;
	uint16 border;
};

struct MapSettings
{
	uint8 cellInitialAliveChance;
	uint32 width;
	uint32 height;
	uint32 deathLimit;
	uint32 birthLimit;
};

//---------------------------------------------------------------------------
// Count alive neighbours around a given cell
//---------------------------------------------------------------------------
inline uint32 map_countNeighbours(MapSettings& settings, MapNode* map, int32 cellX, int32 cellY, uint16 alive=1)
{
	uint32 neighbourCount=0;
	for (int32 x = -1; x < 2; x++)
	{
		for (int32 y = -1; y < 2; y++)
		{
			int32 neighbourX = x + cellX;
			int32 neighbourY = y + cellY;

			// ignore the 'center' (current) cell
			if (x == 0 && y == 0) 
				continue;

			// if it goes out of the map, consider it dead
			if (neighbourX >= settings.width || neighbourX < 0 || 
					neighbourY >= settings.height || neighbourY < 0)
			{
				//neighbourCount++;
				continue;
			}

			if(map[neighbourX * settings.width + neighbourY].type == alive)
			{
				neighbourCount++;
			}
		}
	}

	return neighbourCount;
}

void map_initialize(MapSettings& settings, MapNode* map)
{
	clock_t time = clock();
	ldare::seed((int32) time);
	for(uint32 x=0; x < settings.width; x++)
	{
		for(uint32 y=0; y < settings.height; y++)
		{
			int8 isAlive = ldare::random() % 100 < settings.cellInitialAliveChance;
			map[x * settings.width + y].type = (uint16)isAlive;
		}
	}
}

static void map_addLayer(MapSettings& settings, MapNode* map, uint32 layerType)
{
	clock_t time = clock();
ldare:seed((int32) time);
	for(uint32 x=0; x < settings.width; x++)
	{
		for(uint32 y=0; y < settings.height; y++)
		{
			int32 index = x * settings.width + y;
			if (map[index].type != 0)
			{
				if (ldare::random() % 100 < settings.cellInitialAliveChance )
					map[x * settings.width + y].type = layerType;
			}
		}
	}
}

void map_simulate(MapSettings& settings, MapNode* map, uint32 simulations, uint16 alive=1, uint16 dead=0)
{
	MapNode* snapshot = new MapNode[settings.width * settings.height];
	uint32 mapSize = settings.width * settings.height;
	for (int32 i=0; i < simulations; i++)
	{
		// take a snapshot of the original map
		memcpy(snapshot, map, mapSize);

		for (int32 x=0; x < settings.width; x++)
		{
			for (int32 y=0; y < settings.height; y++)
			{
				uint32 neighbourCount = map_countNeighbours(settings, snapshot, x, y, alive);
				uint32 cellIndex = x * settings.width + y;

				// is it alive now ?
				if (snapshot[cellIndex].type == alive)
				{
					if (neighbourCount < settings.deathLimit)
						map[cellIndex].type = dead;
					else
						map[cellIndex].type = alive;
				}
				else if (snapshot[cellIndex].type == dead)
				{
					if ( neighbourCount >= settings.birthLimit)
						map[cellIndex].type = alive;
					else
						map[cellIndex].type = dead;
				}
			}
		}
	}
	delete snapshot;
}

#define BORDER_CENTER 0
#define BORDER_TOP 1
#define BORDER_LEFT 0x10
#define BORDER_BOTTOM 0x100
#define BORDER_RIGHT 0x1000

void map_setMapBorders(MapSettings& settings, MapNode* map)
{
	uint32 numNodes = settings.width * settings.height;
	for(uint32 x=0; x < settings.width; x++)
	{
		for(uint32 y=0; y < settings.height; y++)
		{

			MapNode& node = map[x * settings.width + y];

			// get indices for adjacent neighbours
			int32 bottom = (x - 1)  * settings.width + y;
			int32 top = (x + 1) * settings.width + y;
			int32 left = x * settings.width + y - 1;
			int32 right = x * settings.width + y + 1;

			if ( bottom < 0 || map[bottom].type == 0)
				node.border |= BORDER_BOTTOM;

			if ( left < 0 || map[left].type == 0)
				node.border |= BORDER_LEFT;

			if ( top > numNodes || map[top].type == 0)
				node.border |= BORDER_TOP;

			if ( right > numNodes || map[right].type == 0)
				node.border |= BORDER_RIGHT;
		}
	}
}
