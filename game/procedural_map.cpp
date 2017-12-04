#include <cstdlib>
#include <cstring>
#include <time.h>

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
inline uint32 map_countNeighbours(MapSettings& settings, uint8* map, int32 cellX, int32 cellY, uint8 alive=1)
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
				//LogInfo("%dx%d", neighbourX, neighbourY);
				//neighbourCount++;
				continue;
			}

				//if(map[neighbourX * settings.width + neighbourY] > 0)
				if(map[neighbourX * settings.width + neighbourY] == alive)
				{
					//LogInfo("%dx%d", neighbourX, neighbourY);
					neighbourCount++;
				}
		}
	}

	return neighbourCount;
}

#define CELL_TOP_LEFT 1
#define CELL_TOP 2
#define CELL_TOP_RIGHT 3

#define CELL_BOTTOM_LEFT 4
#define CELL_BOTTOM 5
#define CELL_BOTTOM_RIGHT 6

#define CELL_CENTER_LEFT 7
#define CELL_CENTER 8
#define CELL_CENTER_RIGHT 9

inline uint32 map_getCellType(MapSettings& settings, uint8* map, int32 cellX, int32 cellY, uint8 alive=1)
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
				//LogInfo("%dx%d", neighbourX, neighbourY);
				//neighbourCount++;
				continue;
			}

				//if(map[neighbourX * settings.width + neighbourY] > 0)
				if(map[neighbourX * settings.width + neighbourY] == alive)
				{
					//LogInfo("%dx%d", neighbourX, neighbourY);
					neighbourCount++;
				}
		}
	}

	return neighbourCount;
}

void map_initialize(MapSettings& settings, uint8* map)
{
	clock_t time = clock();
	srand((int32) time);
  for(uint32 x=0; x < settings.width; x++)
	{
		for(uint32 y=0; y < settings.height; y++)
		{
			int8 isAlive = rand() % 100 < settings.cellInitialAliveChance;
			map[x * settings.width + y] = (uint8)isAlive;
		}
	}
}

static void map_addLayer(MapSettings& settings, uint8* map, uint32 layerType)
{
	clock_t time = clock();
	srand((int32) time);
  for(uint32 x=0; x < settings.width; x++)
	{
		for(uint32 y=0; y < settings.height; y++)
		{
			int32 index = x * settings.width + y;
			if (map[index] != 0)
			{
			 if (rand() % 100 < settings.cellInitialAliveChance )
				 map[x * settings.width + y] = layerType;
			}
		}
	}
}
