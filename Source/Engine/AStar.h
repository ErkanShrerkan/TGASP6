#pragma once
#include "Heap.hpp"

#include <vector>
#include <array>
#include <functional>
#include <cmath>

template <class T>
using Ref = std::reference_wrapper<T>;

namespace CommonUtilities
{
	static const int MapWidth = 20;
	static const int MapHeight = 20;
	static const int TileCount = MapWidth * MapHeight;

	enum class Tile
	{
		Impassable,
		Passable
	};

	struct MapNode
	{
		uint32_t distance;
		size_t index;
		Tile tile;

		size_t row;
		size_t column;

		bool isInToBeChecked;

		size_t predecessor = (size_t)-1;
		uint32_t heuristic = 0;
	};

	bool operator<(const MapNode& aNode, const MapNode& anotherNode)
	{
		return aNode.distance < anotherNode.distance;
	}

	std::vector<int> AStar(const std::vector<Tile>& aMap, int aStartIndex, int anEndIndex)
	{
		std::vector<int> result;

		std::array<std::array<MapNode, MapWidth>, MapHeight> tilemap;

		Heap<Ref<MapNode>> toBeChecked;

		const int64_t startRow = aStartIndex / MapWidth;
		const int64_t startColumn = aStartIndex % MapWidth;

		for (size_t index = 0U; index < aMap.size(); ++index)
		{
			const size_t row = index / MapWidth;
			const size_t column = index % MapWidth;

			if (index == aStartIndex)
			{
				tilemap[row][column] = MapNode({ (uint32_t)-1, index, aMap[index], row, column, false,(size_t)aStartIndex });
				tilemap[row][column].distance = 0;
				toBeChecked.Enqueue(Ref<MapNode>(tilemap[row][column]));
			}
			else
			{
				tilemap[row][column] = MapNode({ (uint32_t)-1, index, aMap[index], row, column, false });
			}

			if (aMap[index] == Tile::Passable)
			{
				tilemap[row][column].isInToBeChecked = true;
			}
			
			 //tilemap[row][column].heuristic = abs((int64_t)row - startRow) + abs((int64_t)column - startColumn);
		}

		auto checkNeighbor = [&](size_t row, size_t column, std::vector<Ref<MapNode>>& aBuffer)
		{
			if (row > MapHeight - 1 || column > MapWidth - 1)
			{
				return;
			}

			if (!tilemap[row][column].isInToBeChecked || tilemap[row][column].tile == Tile::Impassable)
			{
				return;
			}

			aBuffer.push_back(Ref<MapNode>(tilemap[row][column]));
		};

		std::vector<Ref<MapNode>> nodeBuffer;
		nodeBuffer.reserve(4u);
		while (toBeChecked.GetSize())
		{
			nodeBuffer.clear();
			Ref<MapNode> currentNode = toBeChecked.Dequeue();

			if (currentNode.get().index == anEndIndex)
			{
				Ref<MapNode> current = currentNode;

				do
				{
					//result.push_back(current.get().index);

					if (current.get().index == aStartIndex)
					{
						break;
					}

					current = Ref<MapNode>(tilemap[current.get().predecessor / MapWidth][current.get().predecessor % MapWidth]);
				} while (current.get().predecessor < TileCount);

				break;
			}

			checkNeighbor(currentNode.get().row + 1, currentNode.get().column, nodeBuffer);
			checkNeighbor(currentNode.get().row, currentNode.get().column + 1, nodeBuffer);
			checkNeighbor(currentNode.get().row - 1, currentNode.get().column, nodeBuffer);
			checkNeighbor(currentNode.get().row, currentNode.get().column - 1, nodeBuffer);

			for (Ref<MapNode> neighbourNode : nodeBuffer)
			{
				const uint32_t neighbourF = neighbourNode.get().distance;
				const uint32_t currentF = currentNode.get().distance + 1 + neighbourNode.get().heuristic;
				if (currentF < neighbourF)
				{
					neighbourNode.get().predecessor = currentNode.get().index;
					neighbourNode.get().distance = currentF;
					toBeChecked.Enqueue(neighbourNode);
				}
			}

			currentNode.get().isInToBeChecked = false;
		}

		std::reverse(result.begin(), result.end());

		return result;
	}
}