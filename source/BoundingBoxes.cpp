/******************************************************************
Boites de detection de collision
Done by Francois Braud
******************************************************************/

#include "../include/MathUtils.h"

#include "../include/BoundingBoxes.h"

#define roundNumber(x) (int)(x<0?ceil((x)-0.5):floor((x)+0.5))

namespace BoundingBoxes
{
	//---------------------------------------------------------------------
	/**
		Operateur d'affichage de PositionBox
		*/

	std::ostream& operator << (std::ostream& flux, const PositionBox& in)
	{
		flux << "PositionBox (x0=" << in.x0()
			<< ";y0=" << in.y0()
			<< ";x1=" << in.x1()
			<< ";y1=" << in.y1()
			<< ")";
		return flux;
	}

	//---------------------------------------------------------------------
	/**
		Fonction de calcul pour isThereIntersection
		*/

	static bool isThereIntersection2(const AdvancedBox& first, const AdvancedBox& second)
	{
		for (int i = 0; i < 4; ++i)
		{
			const Int2& currentPoint = first.data[i];

			if (second.isThereIntersection(currentPoint))
				return true;
		}
		return false;
	}

	//---------------------------------------------------------------------
	/**
		Pour savoir est-ce qu'il y a intersection entre deux AdvancedBox
		*/

	bool AdvancedBox::isThereIntersectionSlow(const AdvancedBox& other) const
	{
		return isThereIntersection2(*this, other) || isThereIntersection2(other, *this);
	}

	//---------------------------------------------------------------------
	/**
		Pour savoir est-ce qu'il y a intersection entre un AdvancedBox et un point
		*/

	bool AdvancedBox::isThereIntersectionSlow(const Int2& pointToTest) const
	{
		//cas courant
		if (data[0].y() == data[1].y() && data[2].y() == data[2].y()
			&& data[1].x() == data[2].x() && data[3].x() == data[0].x()
			&& data[0].x() > data[1].x() && data[1].y() > data[2].y())
		{
			return pointToTest.x() < data[0].x() && pointToTest.x() > data[1].x()
				&& pointToTest.y() < data[1].y() && pointToTest.y() > data[2].y();
		}

		Int2 pointBox1InNewReference;
		Int2 pointBox2InNewReference;

		int j = 0;
		for (; j < 4; ++j)
		{
			const Int2& pointBox1 = data[j];
			const Int2& pointBox2 = data[(j + 1) % 4];

			//verifie si pointToTest est du bon cote de la droite (pointBox1, pointBox2)
			pointBox1InNewReference = pointBox1 - pointToTest;
			pointBox2InNewReference = pointBox2 - pointToTest;

			float anglePointBox1 = atan2(-(float)pointBox1InNewReference.y(), (float)pointBox1InNewReference.x());
			float anglePointBox2 = atan2(-(float)pointBox2InNewReference.y(), (float)pointBox2InNewReference.x());

			if (anglePointBox1 > anglePointBox2 + MathUtils::PI)
			{
				anglePointBox1 -= MathUtils::PI * 2;
				Assert(anglePointBox1 <= anglePointBox2 + MathUtils::PI);
			}
			else if (anglePointBox2 > anglePointBox1 + MathUtils::PI)
			{
				anglePointBox2 -= MathUtils::PI * 2;
				Assert(anglePointBox2 <= anglePointBox1 + MathUtils::PI);
			}

			if (anglePointBox1 <= anglePointBox2)
				break;
		}

		return j == 4;
	}

	//---------------------------------------------------------------------
	/**
		Fonction de calcul pour isThereIntersectionFast
		(merci orelero de http://www.developpez.net/forums/showthread.php?t=369370)
		*/

	static bool intersectionSegments(const Int2& ptA, const Int2& ptB, const Int2& ptC, const Int2& ptD)
	{
		Float2 ptS;
		if (ptA.x() == ptB.x())
		{
			if (ptC.x() == ptD.x())
				return false;
			else
			{
				float pCD = (float)(ptC.y() - ptD.y()) / (float)(ptC.x() - ptD.x());
				ptS.data[0] = (float)ptA.x();
				ptS.data[1] = pCD*(float)(ptA.x() - ptC.x()) + (float)ptC.y();
			}
		}
		else
		{
			if (ptC.x() == ptD.x())
			{
				float pAB = (float)(ptA.y() - ptB.y()) / (float)(ptA.x() - ptB.x());
				ptS.data[0] = (float)ptC.x();
				ptS.data[1] = pAB*(float)(ptC.x() - ptA.x()) + (float)ptA.y();
			}
			else
			{
				float pCD = (float)(ptC.y() - ptD.y()) / (float)(ptC.x() - ptD.x());
				float pAB = (float)(ptA.y() - ptB.y()) / (float)(ptA.x() - ptB.x());
				float oCD = (float)ptC.y() - pCD*(float)ptC.x();
				float oAB = (float)ptA.y() - pAB*(float)ptA.x();
				ptS.data[0] = (oAB - oCD) / (pCD - pAB);
				ptS.data[1] = pCD*ptS.data[0] + oCD;
			}
		}

		Int2 ptSInt;
		ptSInt.data[0] = (int)ptS.data[0];//roundNumber(ptS.data[0]);
		ptSInt.data[1] = (int)ptS.data[1];//roundNumber(ptS.data[1]);

		if ((ptSInt.x() < ptA.x() && ptSInt.x() < ptB.x())
			|| (ptSInt.x() > ptA.x() && ptSInt.x() > ptB.x())
			|| (ptSInt.x() < ptC.x() && ptSInt.x() < ptD.x())
			|| (ptSInt.x() > ptC.x() && ptSInt.x() > ptD.x())
			|| (ptSInt.y() < ptA.y() && ptSInt.y() < ptB.y())
			|| (ptSInt.y() > ptA.y() && ptSInt.y() > ptB.y())
			|| (ptSInt.y() < ptC.y() && ptSInt.y() < ptD.y())
			|| (ptSInt.y() > ptC.y() && ptSInt.y() > ptD.y()))
		{
			return false;
		}

		return true;
	}

	//---------------------------------------------------------------------
	/**
		Autre technique plus rapide d'intersection
		*/

	bool AdvancedBox::isThereIntersectionFast(const AdvancedBox& other) const
	{
		for (int i = 0; i < 4; ++i)
		{
			const Int2& pointBox1 = data[i];
			const Int2& pointBox2 = data[(i + 1) % 4];
			for (int j = 0; j < 4; ++j)
			{
				const Int2& pointBoxOther1 = other.data[j];
				const Int2& pointBoxOther2 = other.data[(j + 1) % 4];

				if (intersectionSegments(pointBox1, pointBox2, pointBoxOther1, pointBoxOther2))
					return true;
			}
		}
		return false;
		/*
			PositionBox thisBox(data[2].x(), data[2].y(), data[0].x(), data[0].y());
			PositionBox otherBox(other.data[2].x(), other.data[2].y(), other.data[0].x(), other.data[0].y());
			if (thisBox.isThereIntersection(otherBox))
			return true;
			return false;
			*/
	}

	//---------------------------------------------------------------------
	/**
		Fonction de calcul pour calculate()
		*/

	static Int2 getRotatedPoint(int posX, int posY, float angleRadians, int posRotationCenterX, int posRotationCenterY, int xDeformation)
	{
		//definition de la position du point dans le nouveau repere dont le centre est le point de rotation
		float newPosX = (float)(posX - posRotationCenterX);
		float newPosY = (float)(posY - posRotationCenterY);

		//calcul de la position du point apres rotation (toujours dans ce nouveau repere)
		float angleNewPos = atan2f(newPosY, newPosX);
#ifdef WIN32
		float moduleNewPos = 1.f / MathUtils::fastInverseSqrt(newPosX*newPosX + newPosY*newPosY);
#else
		float moduleNewPos = sqrt(newPosX*newPosX+newPosY*newPosY);
#endif
		float finalAngle = angleNewPos + angleRadians;
		float rotatedNewPosX = cos(finalAngle) * moduleNewPos;
		float rotatedNewPosY = sin(finalAngle) * moduleNewPos;

		//retour des valeurs en remettant les coordonnees dans le repere normal
		return Int2(roundNumber(rotatedNewPosX) + posRotationCenterX + xDeformation, roundNumber(rotatedNewPosY) + posRotationCenterY);
	}


	//---------------------------------------------------------------------
	/**
		Calcule la boite englobante
		*/

	void AdvancedBox::calculate(const Int2& parPos, const Int2& parSize, const Int2& parHotSpot, float angleDegrees, int xTopDeformation, int xBottomDeformation)
	{
		float angleRadians = angleDegrees * MathUtils::PI / 180.0f;

		data[0] = getRotatedPoint(
			parPos.x() + parSize.width() - parHotSpot.x(), parPos.y() + parSize.height() - parHotSpot.y(),
			angleRadians,
			parPos.x()/*-1*/, parPos.y(), xBottomDeformation);

		data[1] = getRotatedPoint(
			parPos.x() - parHotSpot.x(), parPos.y() + parSize.height() - parHotSpot.y(),
			angleRadians,
			parPos.x(), parPos.y(), xBottomDeformation);

		data[2] = getRotatedPoint(
			parPos.x() - parHotSpot.x(), parPos.y() - parHotSpot.y(),
			angleRadians,
			parPos.x(), parPos.y(), xTopDeformation);

		data[3] = getRotatedPoint(
			parPos.x() + parSize.width() - parHotSpot.x(), parPos.y() - parHotSpot.y(),
			angleRadians,
			parPos.x()/*-1*/, parPos.y(), xTopDeformation);

		/*PrintValue(data[0]);
		PrintValue(data[1]);
		PrintValue(data[2]);
		PrintValue(data[3]);
		Print("");*/
	}

}
