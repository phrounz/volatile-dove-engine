#ifndef PositionsGraph_h_INCLUDED
#define PositionsGraph_h_INCLUDED

#include <vector>

#include "CoreUtils.h"

//---------------------------------------------------------------------

class PositionsGraph
{
public:
	void clear();
	void addSegment(const Float3& first, const Float3& second);
	void removeSegment(const Float3& first, const Float3& second);
	int findPathLength(const Float3& startPosition, const Float3& endPosition) const;
	std::pair<bool,std::vector<Float3> > findPath(const Float3& startPosition, const Float3& endPosition) const;
	void printDebug() const;
private:
	class PGNode
	{
		public:
			PGNode(const Float3& parPos):pos(parPos) {}
			Float3 pos;
			std::vector<PGNode*> linkedNodes;
			mutable bool isVisited;
	};
	std::vector<PGNode*> allNodes;
	int findPathLengthRecursiveCall(PGNode* node, const Float3& endPosition) const;
	std::pair<bool,std::vector<Float3> > PositionsGraph::findPathRecursiveCall(PGNode* node, const Float3& endPosition, int stack) const;
};

//---------------------------------------------------------------------

#endif //PositionsGraph_h_INCLUDED
