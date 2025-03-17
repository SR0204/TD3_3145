#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>

class MapChipField
{
private:
	std::vector<std::vector<int>> mapData;
	float tileSize;

public:
	MapChipField(float tileSize, const std::string& filename);
	bool IsWall(float x, float z) const;

private:
	std::vector<std::vector<int>> LoadMapFromCSV(const std::string& filename);
};

#endif // MAP_H
