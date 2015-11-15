
#include "../include/Utils.h"

#include "../include/PositionsGraph.h"

namespace
{
	const bool DEBUG_VERBOSE = false;
}

void PositionsGraph::clear()
{
	iterateVector(allNodes, PGNode*)
	{
		delete (*it);
	}
	allNodes.clear();
}

void PositionsGraph::addSegment(const Float3& first, const Float3& second)
{
	//std::cout << "add: " << first << "," << second << std::endl;

	PGNode* nodeFirst = NULL;
	PGNode* nodeSecond = NULL;

	iterateVector(allNodes, PGNode*)
	{
		if ((*it)->pos == first)
			nodeFirst = (*it);
		else if ((*it)->pos == second)
			nodeSecond = (*it);
	}
	if (nodeFirst == NULL)
	{
		nodeFirst = new PGNode(first);
		allNodes.push_back(nodeFirst);
	}

	if (nodeSecond == NULL)
	{
		nodeSecond = new PGNode(second);
		allNodes.push_back(nodeSecond);
	}

	nodeSecond->linkedNodes.push_back(nodeFirst);
	nodeFirst->linkedNodes.push_back(nodeSecond);
}

void PositionsGraph::printDebug() const
{
	iterateVectorConst(allNodes, PGNode*)
	{
		std::cout << "" << ((*it)->pos) << ":" << std::endl;
		iterateVectorConst2((*it)->linkedNodes, PGNode*)
		{
			std::cout << "  " << ((*it2)->pos) << std::endl;
		}
	}
}

void PositionsGraph::removeSegment(const Float3& first, const Float3& second)
{
	PGNode* nodeFirst = NULL;
	PGNode* nodeSecond = NULL;

	iterateVector(allNodes, PGNode*)
	{
		if ((*it)->pos == first)
		{
			iterateVector2((*it)->linkedNodes, PGNode*)
			{
				if ((*it2)->pos == second)
				{
					(*it)->linkedNodes.erase(it2);
					break;
				}
			}
			if ((*it)->linkedNodes.size() == 0)
			{
				allNodes.erase(it);
				break;
			}
		}
	}

	iterateVector(allNodes, PGNode*)
	{
		if ((*it)->pos == second)
		{
			iterateVector2((*it)->linkedNodes, PGNode*)
			{
				if ((*it2)->pos == first)
				{
					(*it)->linkedNodes.erase(it2);
					break;
				}
			}
			if ((*it)->linkedNodes.size() == 0)
			{
				allNodes.erase(it);
				break;
			}
		}
	}
}

int PositionsGraph::findPathLength(const Float3& startPosition, const Float3& endPosition) const
{
	PGNode* node = NULL;
	iterateVectorConst(allNodes, PGNode*)
	{
		(*it)->isVisited = false;
		if ((*it)->pos == startPosition)
			node = (*it);
	}
	if (DEBUG_VERBOSE) { outputln( "start:" << (long int)(void*)node ); }
	if (node == NULL) return -1;
	return findPathLengthRecursiveCall(node, endPosition);
}

int PositionsGraph::findPathLengthRecursiveCall(PGNode* node, const Float3& endPosition) const
{
	if (node->isVisited)
		return -1;
	if (DEBUG_VERBOSE) { outputln( "node: "<< node->pos ); }
	node->isVisited = true;
	if (node->pos == endPosition) //found!
		return 0;
	iterateVectorConst(node->linkedNodes, PGNode*)
	{
		int depthFound = findPathLengthRecursiveCall((*it), endPosition);
		if (depthFound > -1) return depthFound+1;
	}
	return -1;
}

std::pair<bool,std::vector<Float3> > PositionsGraph::findPath(const Float3& startPosition, const Float3& endPosition) const
{
	PGNode* node = NULL;
	iterateVectorConst(allNodes, PGNode*)
	{
		(*it)->isVisited = false;
		if ((*it)->pos == startPosition)
			node = (*it);
	}
	if (DEBUG_VERBOSE) { outputln( "start:" << (long int)(void*)node ); }
	return findPathRecursiveCall(node, endPosition, 0);
}

std::pair<bool,std::vector<Float3> > PositionsGraph::findPathRecursiveCall(PGNode* node, const Float3& endPosition, int stack) const
{
	std::pair<bool,std::vector<Float3> > result;
	if (DEBUG_VERBOSE) { for (int i = 0; i < stack; ++i) output("  ");outputln( "node: "<< node->pos ); }
	result.second.push_back(node->pos);

	if (node->isVisited)
	{
		result.first = false;
		if (DEBUG_VERBOSE) { for (int i = 0; i < stack; ++i) output("  ");outputln("  (already visited)"); }
	}
	else
	{
		node->isVisited = true;
		if (node->pos == endPosition)
		{
			result.first = true;
			if (DEBUG_VERBOSE) { for (int i = 0; i < stack; ++i) output("  ");outputln("  (found end)"); }
		}
		else
		{
			iterateVectorConst(node->linkedNodes, PGNode*)
			{
				std::pair<bool,std::vector<Float3> > resultThatWay = findPathRecursiveCall((*it), endPosition, stack + 1);
				if (resultThatWay.first)
				{
					if (DEBUG_VERBOSE) { for (int i = 0; i < stack; ++i) output("  ");outputln("  (return end)"); }
					result.first = true;
					result.second.insert( result.second.end(), resultThatWay.second.begin(), resultThatWay.second.end() );
					return result;
				}
			}
		}
	}
	return result;
}

/*
int PositionsGraph::findPath(const Float3& startPosition, const Float3& endPosition) const
{
	Float3 previousPos = startPosition;
	Float3 pos = startPosition;
	bool foundWay = false;
	int nbNodes = 0;
	while (true)
	{
		++nbNodes;
		std::cout << pos << std::endl;
		std::map<Float3, PositionsGraphNode>::const_iterator it = ppm.find(pos);
		foundWay = false;
		if (it != ppm.end())
		{
			std::cout << " ok found" << std::endl;
			const PositionsGraph::PositionsGraphNode& pgn = (*it).second;
			const std::vector<Float3>& vect = pgn.vect;
			for (int i = 0; i < pgn.vect.size(); ++i)
			{
				std::cout << "  " << vect[i] << std::endl;
				if (vect[i] != previousPos && vect[i] != pos)
				{
					previousPos = pos;
					pos = vect[i];
					foundWay = true;
					std::cout << "  ->" << std::endl;
					break;
				}
			}
			if (pos == endPosition)
				break;
			else if (foundWay)
				continue;
		}
		break;
	}

	std::cout << "ended: " << foundWay << "," << nbNodes << std::endl;
	return foundWay ? nbNodes : -1;
}
*/

//void PositionsGraph::addSegment(const Float3& first, const Float3& second)
//{
//	nodes.push_back();
//	segment.
//}
