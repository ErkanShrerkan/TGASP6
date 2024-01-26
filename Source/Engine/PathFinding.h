#pragma once
#include "Engine\Heap.hpp"
#include "NavMeshLoader.h"
#include <vector>

namespace SE
{
    class CPathFinding
    {
    public:
        enum class eStatus
        {
            eUnvisited,
            eOpen,
            eClosed
        };

        struct SNode
        {
            int predecessorIndex;
            float h;
            float g;
            float f;
            eStatus status;

        };

        struct SNewNode
        {
            int f;
            eStatus status;
            int predecessorIndex;
        };

    private:

        int GetLowestCurrDistIndex(std::map<SNavTriangle*, SNode>& someNodes);

        int GetLowestNewCurrDistIndex(std::map<SNavTriangle*, SNewNode>& someNodes);

        bool IndexToBeChecked(int anIndex, std::vector<int>& aVector);

        bool HasOpenNodes(std::map<SNavTriangle*, SNode>& someNodes);

        bool HasOpenNewNodes(std::map<SNavTriangle*, SNewNode>& someNodes);

        double CalcTriangleArea(double x1, double y1, double x2, double y2, double x3, double y3);
        
        void stringPull(std::vector<SE::SNavVertex> leftPortals, std::vector<SE::SNavVertex> rightPortals,
            std::vector<Vector3f>* pts);

    public:
        std::vector<int> AStar(const std::vector<SNavTriangle*>& someNavTriangles, int aStartIndex, int anEndIndex, const bool& aShouldBeReversed);
        std::vector<int> FloodFill(const std::vector<SNavTriangle*>& someNavTriangles, int aStartIndex, int anEndIndex, const bool& aShouldBeReversed, std::map<SE::SNavTriangle*, SNode>& someNodes);


        std::vector<int> BjornFill(const std::vector<SNavTriangle*>& someNavTriangles, int aPlayerIndex,  std::map<SE::SNavTriangle*, SNewNode>& someNodes);
        void FindClosestNeighbour(SE::SNavTriangle* currentTriangle, std::map<SE::SNavTriangle*, SNewNode>& someNodes, std::vector<int>& aIndexPath);
        std::vector<int> BjornFind(const std::vector<SE::SNavTriangle*>& someNavTriangles, std::map<SE::SNavTriangle*, SNewNode>& someNodes, int aStartIndex, int anEndIndex, const bool& aShouldBeReversed);


        std::vector<Vector3f> Funnel(const std::vector<SE::SNavTriangle*>& someNavTriangles, std::vector<int> aIndexPath, Vector3f aStartPos, Vector3f anEndPos);
              
    };
}

