/******************************************************************
Sprites bounding boxes
Done by Francois Braud
******************************************************************/

#ifndef Boxes_h_INCLUDED
#define Boxes_h_INCLUDED

#include "MathUtils.h"
#include "CoreUtils.h"

namespace BoundingBoxes
{
	//---------------------------------------------------------------------
	/**
	* \brief very simple 2D rectangular bounding box
	*/

	class PositionBox : public Int4
	{
	public:
		PositionBox(i32 x0, i32 y0, i32 x1, i32 y1)
		{
			data[0] = x0;
			data[1] = y0;
			data[2] = x1;
			data[3] = y1;
		}
		inline i32 x0() const { return data[0]; }
		inline i32 y0() const { return data[1]; }
		inline i32 x1() const { return data[2]; }
		inline i32 y1() const { return data[3]; }

		inline bool isThereIntersection(const PositionBox& other)
		{
			if (other.y0() < y1() && other.y1() > y0() && other.x0() < x1() && other.x1() > x0())
				return true;
			return false;
		}
	};

	std::ostream& operator << (std::ostream& flux, const PositionBox& in);

	//---------------------------------------------------------------------
	/**
	* \brief 2D bounding box with rotation, scaling, ...
	*/

	class AdvancedBox : public CoreUtils::BasicVector<4, Int2>
	{
	public:
		AdvancedBox() {}
		AdvancedBox(
			const Int2& parExtremePointRightBottom,
			const Int2& parExtremePointLeftBottom,
			const Int2& parExtremePointLeftTop,
			const Int2& parExtremePointRightTop)
		{
			data[0] = parExtremePointRightBottom;
			data[1] = parExtremePointLeftBottom;
			data[2] = parExtremePointLeftTop;
			data[3] = parExtremePointRightTop;
		}

		inline Int2 getMin() const
		{
			Int2 result(MathUtils::minimum(data[0].x(), data[1].x()), MathUtils::minimum(data[0].y(), data[1].y()));
			result.data[0] = MathUtils::minimum(result.x(), data[2].x());
			result.data[0] = MathUtils::minimum(result.x(), data[3].x());
			result.data[1] = MathUtils::minimum(result.y(), data[2].y());
			result.data[1] = MathUtils::minimum(result.y(), data[3].y());
			return result;
		}

		inline Int2 getMax() const
		{
			Int2 result(MathUtils::maximum(data[0].x(), data[1].x()), MathUtils::maximum(data[0].y(), data[1].y()));
			result.data[0] = MathUtils::maximum(result.x(), data[2].x());
			result.data[0] = MathUtils::maximum(result.x(), data[3].x());
			result.data[1] = MathUtils::maximum(result.y(), data[2].y());
			result.data[1] = MathUtils::maximum(result.y(), data[3].y());
			return result;
		}

		//! Return A PositionBox giving Sprite extreme positions
		inline PositionBox getBoxLimits() const
		{
			Int2 mintmp = getMin();
			Int2 maxtmp = getMax();
			return PositionBox(mintmp.x(), mintmp.y(), maxtmp.x(), maxtmp.y());
		}

		inline const Int2& getRightBottomPoint() const { return data[0]; }
		inline const Int2& getLeftBottomPoint()  const { return data[1]; }
		inline const Int2& getLeftTopPoint()     const { return data[2]; }
		inline const Int2& getRightTopPoint()    const { return data[3]; }

		inline bool isThereIntersection(const AdvancedBox& other) const { return isThereIntersectionFast(other); }
		inline bool isThereIntersection(const Int2& pointToTest) const { return isThereIntersectionSlow(pointToTest); }

		inline bool isThereRoughlyIntersection(const AdvancedBox& other) const
		{
			return (other.getMax() > getMin() && other.getMin() < getMax());
		}

		bool isThereIntersectionSlow(const AdvancedBox& other) const;
		bool isThereIntersectionSlow(const Int2& pointToTest) const;
		bool isThereIntersectionFast(const AdvancedBox& other) const;

		void calculate(const Int2& parPos, const Int2& parSize, const Int2& parHotSpot, float angleDegrees, int xTopDeformation = 0, int xBottomDeformation = 0);
	};

}

#endif //Boxes_h_INCLUDED
