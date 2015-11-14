
#include <map>
#include <set>
#include <vector>

#include "../include/GroupOfQuads.h"

namespace
{
	static const bool ENABLE_COPY_NORMALS_IN_OPTIMIZE_CONCAT_QUADS = false;

	//-------------

	const bool DEBUG_PERFS = true;

	inline bool float3ApproxEquals(float3 f1, float3 f2)
	{
		//return memcmp(&f1, &f2, sizeof(float3)) == 0;
		return fabsf(f1.x - f2.x) < 0.01f && fabsf(f1.y - f2.y) < 0.01f && fabsf(f1.z - f2.z) < 0.01f;
	}
	
	inline float3 floored(const float3& param)
	{
		return param;
		/*float3 res;
		res.x = floor(param.x + 0.001f);
		res.y = floor(param.y + 0.001f);
		res.z = floor(param.z + 0.001f);
		return res;*/
	}

	inline std::pair<float3, float3> getPair(const float3& f1, const float3& f2)
	{
		return std::pair<float3, float3>(floored(f1), floored(f2));
	}
}


namespace Meshes3D
{
	//---------------------------------------------------------------------

	void GroupOfQuads::mergeTwoPreviousQuadsIfPossible(int faceId, bool isUsualSurface)
	{
		if (m_nbQuads < 2) return;
		const int nbQuadInit = m_nbQuads;

		BasicQuad& firstQuad = *this->getData(m_nbQuads - 2);
		const float* firstQuad_v1 = (const float*)&firstQuad.getVertex(0).pos;
		const float* firstQuad_v2 = (const float*)&firstQuad.getVertex(1).pos;//=
		const float* firstQuad_v3 = (const float*)&firstQuad.getVertex(2).pos;//-
		const float* firstQuad_v4 = (const float*)&firstQuad.getVertex(3).pos;

		BasicQuad& secondQuad = *this->getData(m_nbQuads - 1);
		const float* secondQuad_v1 = (const float*)&secondQuad.getVertex(0).pos;//=
		const float* secondQuad_v2 = (const float*)&secondQuad.getVertex(1).pos;
		const float* secondQuad_v3 = (const float*)&secondQuad.getVertex(2).pos;
		const float* secondQuad_v4 = (const float*)&secondQuad.getVertex(3).pos;//-

		bool isOk = false;
		bool isReverseFace = (faceId == 0 || faceId == 3 || faceId == 5);

		/*if (isUsualSurface)
		{
		// this is simpler: we don't need to check so must things because we already know
		// that all vertices are on the same plane
		isOk = (isReverseFace && memcmp(firstQuad_v3, secondQuad_v4, 3*sizeof(float))==0)
		|| (!isReverseFace && memcmp(firstQuad_v4, secondQuad_v1, 3*sizeof(float))==0);
		}
		else
		{*/
		//-----------------
		// 1 2 | 1 2
		// 4 3 | 4 3
		//-----------------

		if (isReverseFace && memcmp(firstQuad_v3, secondQuad_v4, 3 * sizeof(float)) == 0 && memcmp(firstQuad_v2, secondQuad_v1, 3 * sizeof(float)) == 0)
		{
			switch (faceId)
			{
			case 0://top - same Y/Z
				isOk = firstQuad_v1[1] == secondQuad_v2[1] && firstQuad_v1[1] == firstQuad_v2[1]// && firstQuad_v1[2] == secondQuad_v2[2]
					&& firstQuad_v4[1] == secondQuad_v3[1] && firstQuad_v4[1] == firstQuad_v3[1];// && firstQuad_v4[2] == secondQuad_v3[2]
				break;
			case 3://back - same Y/Z
				isOk = firstQuad_v1[2] == secondQuad_v2[2] && firstQuad_v1[2] == firstQuad_v2[2]
					&& firstQuad_v4[2] == secondQuad_v3[2] && firstQuad_v4[2] == firstQuad_v3[2];
				break;
			case 5://right - same X/Y
				isOk = firstQuad_v1[0] == secondQuad_v2[0] && firstQuad_v1[0] == firstQuad_v2[0]
					&& firstQuad_v4[0] == secondQuad_v3[0] && firstQuad_v4[0] == firstQuad_v3[0];
				break;
			}
		}

		//-----------------
		// 1 4 | 1 4
		// 2 3 | 2 3
		//-----------------

		if (!isReverseFace && memcmp(firstQuad_v4, secondQuad_v1, 3 * sizeof(float)) == 0 && memcmp(firstQuad_v3, secondQuad_v2, 3 * sizeof(float)) == 0)
		{
			switch (faceId)
			{
			case 1://bottom - same Y/Z
				isOk = firstQuad_v1[1] == secondQuad_v4[1] && firstQuad_v1[1] == firstQuad_v4[1]
					&& firstQuad_v2[1] == secondQuad_v3[1] && firstQuad_v2[1] == firstQuad_v3[1];
				break;
			case 2://front - same Y/Z
				isOk = firstQuad_v1[2] == secondQuad_v4[2] && firstQuad_v1[2] == firstQuad_v4[2]
					&& firstQuad_v2[2] == secondQuad_v3[2] && firstQuad_v2[2] == firstQuad_v3[2];
				break;
			case 4://left - same X/Y
				isOk = firstQuad_v1[0] == secondQuad_v4[0] && firstQuad_v1[0] == firstQuad_v4[0]
					&& firstQuad_v2[0] == secondQuad_v3[0] && firstQuad_v2[0] == firstQuad_v3[0];
				break;
			}
		}
		//}

		if (isOk)
		{
			if (isReverseFace)
			{
				// note: v1 and v4 are already good

				// firstQuad_v1 <- firstQuad_v1 (already done)
				// firstQuad_v2 <- secondQuad_v2
				firstQuad.basicVertex[1] = secondQuad.basicVertex[1];
				// firstQuad_v3 <- secondQuad_v3
				firstQuad.basicVertex[2] = secondQuad.basicVertex[2];
				// firstQuad_v4 <- firstQuad_v4 (already done)
			}
			else
			{
				// note: v1 and v2 are already good

				// firstQuad_v1 <- firstQuad_v1 (already done)
				// firstQuad_v4 <- secondQuad_v4
				firstQuad.basicVertex[3] = secondQuad.basicVertex[3];
				// firstQuad_v3 <- secondQuad_v3
				firstQuad.basicVertex[2] = secondQuad.basicVertex[2];
				// firstQuad_v2 <- firstQuad_v2 (already done)
			}
			m_nbQuads--;// //-;//you can set to nbQuads-=2 in order to display which quads are merged
		}

		//outputln("Removed for " << this << ": " << (nbQuadInit - m_nbQuads) << " / " << nbQuadInit << " ");
	}



	void GroupOfQuads::simplify(bool horiz)
	{
		if (m_nbQuads == 0) return;

		const size_t nbQuadInit = m_nbQuads;
		long long int startTime = Utils::getMicrosecondTime();
		int nbLoop = 0;

		if (DEBUG_PERFS) outputln("=> " << this << " 0 " << Utils::getMicrosecondTime());

		unsigned char* tableDeleted = new unsigned char[m_nbQuads];

		std::map<std::pair<float3, float3>, std::set<int> > indexesByPosition;

		int nbQuadsToRemove = 0;

		do
		{
			memset(tableDeleted, 0, sizeof(unsigned char)*m_nbQuads);

			if (DEBUG_PERFS) outputln("=> " << this << " 1 " << Utils::getMicrosecondTime() << " " << m_nbQuads);

			for (std::map<std::pair<float3, float3>, std::set<int > >::iterator itz = indexesByPosition.begin(); itz != indexesByPosition.end(); itz++)
			{
				(*itz).second.clear();
			}

			//---------------
			// fill the indexesByPosition table

			for (int i = 0; i < (int)m_nbQuads; ++i)
			{
				const BasicQuad& basicQuad = *(this->getData(i));
				bool sameX = basicQuad.getVertex(1).pos.x == basicQuad.getVertex(0).pos.x && basicQuad.getVertex(2).pos.x == basicQuad.getVertex(3).pos.x;
				bool sameY = basicQuad.getVertex(1).pos.y == basicQuad.getVertex(0).pos.y && basicQuad.getVertex(2).pos.y == basicQuad.getVertex(3).pos.y;
				bool sameZ = basicQuad.getVertex(1).pos.z == basicQuad.getVertex(0).pos.z && basicQuad.getVertex(2).pos.z == basicQuad.getVertex(3).pos.z;
				if ((sameX && sameY) || (sameX && sameZ) || (sameY && sameZ))
				{
					indexesByPosition[getPair(basicQuad.getVertex(0).pos, basicQuad.getVertex(3).pos)].insert(i);
					indexesByPosition[getPair(basicQuad.getVertex(1).pos, basicQuad.getVertex(0).pos)].insert(i);
					indexesByPosition[getPair(basicQuad.getVertex(1).pos, basicQuad.getVertex(2).pos)].insert(i);
					indexesByPosition[getPair(basicQuad.getVertex(2).pos, basicQuad.getVertex(3).pos)].insert(i);
				}
			}

			if (DEBUG_PERFS) outputln("=> " << this << " 2 " << Utils::getMicrosecondTime());

			//---------------
			// get pairs

			std::set<std::pair<int, int> > pairIndexes;

			for (std::map<std::pair<float3, float3>, std::set<int > >::iterator itz = indexesByPosition.begin(); itz != indexesByPosition.end(); itz++)
			{
				std::set<int>& indexes = (*itz).second;
				if (indexes.size() <= 1) continue;
				iterateSetConst(indexes, int)
				{
					int i = *it;
					iterateSetConst2(indexes, int)
					{
						int j = *it2;
						if (j != i)
						{
							//if (pairIndexes.find(std::pair<int, int>(j, i)) == pairIndexes.end())
							pairIndexes.insert(std::pair<int, int>(i, j));
						}
					}
				}
			}

			if (DEBUG_PERFS) outputln("=> " << this << " 3 " << Utils::getMicrosecondTime() << " " << pairIndexes.size());

			//---------------
			// modify points

			std::vector<BasicQuad> quadsToAdd;
			quadsToAdd.reserve(m_nbQuads);

			std::vector<std::pair<int, int> > pairIndexesAsVector;
			for (std::set<std::pair<int, int> >::const_iterator it = pairIndexes.begin(); it != pairIndexes.end(); it++)
			{
				pairIndexesAsVector.push_back(*it);
			}

			if (DEBUG_PERFS) outputln("=> " << this << " 4 " << Utils::getMicrosecondTime());

			int nbEquals = 0;
			for (std::vector<std::pair<int, int> >::const_iterator it = pairIndexesAsVector.begin(); it != pairIndexesAsVector.end(); it++)
			{
				int i = (*it).first;
				int j = (*it).second;

				if (tableDeleted[i] == 1 || tableDeleted[j] == 1) continue;

				const BasicQuad& basicQuad = *(this->getData(i));
				const BasicQuad& basicQuadSecondary = *(this->getData(j));

				//-----------------
				// 1 2
				// 0 3
				// ----
				// 1 2
				// 0 3
				//-----------------

				if (float3ApproxEquals(basicQuad.getVertex(0).pos, basicQuadSecondary.getVertex(1).pos)
					&& float3ApproxEquals(basicQuad.getVertex(3).pos, basicQuadSecondary.getVertex(2).pos) && !horiz)
				{
					nbEquals++;
					const float3& pt0 = floored(basicQuad.getVertex(0).pos);
					const float3& pt1 = floored(basicQuad.getVertex(1).pos);
					const float3& pt2 = floored(basicQuad.getVertex(2).pos);
					const float3& pt3 = floored(basicQuad.getVertex(3).pos);
					const float3& pt0s = floored(basicQuadSecondary.getVertex(0).pos);
					const float3& pt3s = floored(basicQuadSecondary.getVertex(3).pos);
					bool sameX = pt1.x == pt0s.x && pt1.x == pt0.x && pt2.x == pt3s.x && pt2.x == pt3.x;
					bool sameY = pt1.y == pt0s.y && pt1.y == pt0.y && pt2.y == pt3s.y && pt2.y == pt3.y;
					bool sameZ = pt1.z == pt0s.z && pt1.z == pt0.z && pt2.z == pt3s.z && pt2.z == pt3.z;
					if ((sameX && sameY) || (sameX && sameZ) || (sameY && sameZ))
					{
						BasicQuad newQuad;
						newQuad.basicVertex[0] = basicQuadSecondary.getVertex(0);
						newQuad.basicVertex[1] = basicQuad.getVertex(1);
						newQuad.basicVertex[2] = basicQuad.getVertex(2);
						newQuad.basicVertex[3] = basicQuadSecondary.getVertex(3);
						quadsToAdd.push_back(newQuad);
						tableDeleted[i] = 1;
						tableDeleted[j] = 1;
					}
				}

				//-----------------
				// 1 2 | 1 2
				// 0 3 | 0 3
				//-----------------

				else if (float3ApproxEquals(basicQuad.getVertex(2).pos, basicQuadSecondary.getVertex(1).pos)
					&& float3ApproxEquals(basicQuad.getVertex(3).pos, basicQuadSecondary.getVertex(0).pos) && horiz)
				{
					nbEquals++;
					const float3& pt0 = floored(basicQuad.getVertex(0).pos);
					const float3& pt1 = floored(basicQuad.getVertex(1).pos);
					const float3& pt2 = floored(basicQuad.getVertex(2).pos);
					const float3& pt3 = floored(basicQuad.getVertex(3).pos);
					const float3& pt2s = floored(basicQuadSecondary.getVertex(2).pos);
					const float3& pt3s = floored(basicQuadSecondary.getVertex(3).pos);
					bool sameX = pt1.x == pt2s.x && pt1.x == pt2.x && pt0.x == pt3s.x && pt0.x == pt3.x;
					bool sameY = pt1.y == pt2s.y && pt1.y == pt2.y && pt0.y == pt3s.y && pt0.y == pt3.y;
					bool sameZ = pt1.z == pt2s.z && pt1.z == pt2.z && pt0.z == pt3s.z && pt0.z == pt3.z;
					if ((sameX && sameY) || (sameX && sameZ) || (sameY && sameZ))
					{
						BasicQuad newQuad;
						newQuad.basicVertex[0] = basicQuad.getVertex(0);
						newQuad.basicVertex[1] = basicQuad.getVertex(1);
						newQuad.basicVertex[2] = basicQuadSecondary.getVertex(2);
						newQuad.basicVertex[3] = basicQuadSecondary.getVertex(3);
						quadsToAdd.push_back(newQuad);
						tableDeleted[i] = 1;
						tableDeleted[j] = 1;
					}
				}
			}

			if (DEBUG_PERFS) outputln("=> " << this << " 5 " << Utils::getMicrosecondTime() << " " << quadsToAdd.size());

			nbQuadsToRemove = 0;
			for (int i = 0; i < (int)m_nbQuads; ++i)
			{
				if (tableDeleted[i]) ++nbQuadsToRemove;
			}

			if (DEBUG_PERFS) outputln("=> " << this << " 6 " << Utils::getMicrosecondTime() << " " << nbEquals);

			//---------------
			// create verticesNew and texCoordNew

			if (nbQuadsToRemove > 0 || quadsToAdd.size() > 0)
			{
				GroupOfQuads newQuads;
				int newNbQuads = m_nbQuads - (int)nbQuadsToRemove + (int)quadsToAdd.size();
				newQuads.allocateMoreQuads(newNbQuads);

				for (size_t i = 0; i < m_nbQuads; ++i)
				{
					if (!tableDeleted[i])
						newQuads.getLastAndNext()->copyFrom(this->getData(i));
				}

				iterateVectorConst(quadsToAdd, BasicQuad)
				{
					newQuads.getLastAndNext()->copyFrom(&*it);
				}

				//---------------
				// replace vertices and texCoord by verticesNew and texCoordNew

				Assert(newNbQuads <= (int)m_nbQuads);
				this->replaceBy(newQuads);
			}

			if (DEBUG_PERFS) outputln("=> " << this << " 7 " << Utils::getMicrosecondTime());

			++nbLoop;
		} while (nbQuadsToRemove > 0);

		delete[] tableDeleted;

		outputln("Removed for " << this << ": " << ((int)nbQuadInit - (int)m_nbQuads) << " / " << nbQuadInit << " (looped " << nbLoop << " times, took " << (Utils::getMicrosecondTime() - startTime) << " ms)");
	}

	//---------------------------------------------------------------------
}
