/******************************************************************
Done by Francois Braud
******************************************************************/

#ifndef Animated3DModel_h_INCLUDED
#define Animated3DModel_h_INCLUDED

#include <vector>
#include <string>

#include "Obj3D.h"

class ObjModel;

namespace Obj3D
{
	//---------------------------------------------------------------------------------------------
	/**
	* \brief A textured 3D model based on an .obj file (and its .mtl)
	*/

	class ObjModel3D : public Obj3DAbstract
	{
		friend class Animated3DModel;
	public:
		ObjModel3D(const char* objModelFile, float meshWeld = -1.f); ///< @param meshWeld is for polygon reduction (you may for example set 5.f); let -1.f for no reduction at all
		bool isThere3DIntersection(const Float3& parSegmentPt1, const Float3& parSegmentPt2, Float3& outIntersectionPt);
		~ObjModel3D();
		void removeFromPicking();
	private:
		const std::vector<const VBOGroup*> getVBOGroups() const;
		virtual void drawNoPos() const;
		ObjModel* m_objModel;
	};

	/**
	* \brief A 3D animated model, which contains several ObjModel3D and displays each of them as a frame of the animation.
	*/
	class Animated3DModel : public Obj3DAbstract
	{
	public:
		/// constructor
		Animated3DModel(const std::vector<std::string>& frameModelFilenames, float scale, float meshWeld = -1.f);

		/// set animation frame duration
		inline void setAnimationFrameDuration(int parAnimationFrameDuration) { animationFrameDuration = parAnimationFrameDuration; }

		/// is the animation running
		inline bool getIsRunningAnimation() { return isRunningAnimation; }

		/// run animation
		inline void runAnimation(bool trueOrFalse) { isRunningAnimation = trueOrFalse; }

		/// reverse frames animation
		inline void reverseAnimation(bool trueOrFalse) { isReverseAnimation = trueOrFalse; }

		/// select animation frame
		void setAnimationFrame(unsigned int idFrame);

		/// get the number of frames (= number of meshes)
		size_t getNbFrames() { return frames.size(); }

		/// set first and last animation frame
		void setAnimationMinAndMaxIds(unsigned int idFrameMin, unsigned int idFrameMax);

		/// is there a 3d intersection
		bool isThere3DIntersection(const Float3& parSegmentPt1, const Float3& parSegmentPt2, Float3& outIntersectionPt) const;

		/// set shadowed
		void setShadowed(bool isShadowed) { m_isShadowed = isShadowed; }

		/// destructor
		~Animated3DModel();

		void removeFromPicking();

	private:
		void drawNoPos() const;
		const std::vector<const VBOGroup*> getVBOGroups() const;

		std::vector<ObjModel3D*> frames;
		int minId;
		int maxId;
		mutable int64_t timeNextFrame;
		mutable int frameId;
		int animationFrameDuration;
		bool isReverseAnimation;
		bool isRunningAnimation;
		bool m_isShadowed;
	};

}

#endif //Animated3DModel_h_INCLUDED
