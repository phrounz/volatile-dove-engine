#ifndef Obj3D_h_INCLUDED
#define Obj3D_h_INCLUDED

#include <vector>
#include <string>
#include <set>

#include "CoreUtils.h"
#include "Cube3D.h"

class RenderGroup;
class Material;
class VBOGroup;
class ObjModel;
class Water;

namespace Obj3D
{
	//---------------------------------------------------------------------------------------------
	/**
	* \brief Interface for drawable 3D objects
	*/

	class Drawable3DObject
	{
	public:
		virtual float distanceToCamera() const = 0;
		virtual bool isVisibleByCamera() const = 0;
		virtual bool containsTransparency() const = 0;
		virtual void draw() const = 0;
		virtual ~Drawable3DObject() {}
		void setVisible(bool yesNo) { m_isVisible = yesNo; }
		bool isVisible() const { return m_isVisible; }

		static void drawBunchOfDrawable3DObjects(const std::set<Drawable3DObject*> objs);
	protected:
		virtual const std::vector<const VBOGroup*> getVBOGroups() const = 0;
		Drawable3DObject() : m_isVisible(true) {}
		bool m_isVisible;
	};

	//---------------------------------------------------------------------------------------------
	/**
	* \brief Interface for 3D objects which can be positioned/rotated/scaled
	*/

	class Obj3DAbstract : public Drawable3DObject
	{
	public:
		Obj3DAbstract() : m_pos(Float3(0.f, 0.f, 0.f)), m_scale(Float3(1.f, 1.f, 1.f)), m_angle(Float3(0.f, 0.f, 0.f)), m_enableLight(true), m_isShadowed(false) {}

		// Drawable3DObject methods
		virtual float distanceToCamera() const;
		virtual bool isVisibleByCamera() const { return true; } // TODO
		virtual bool containsTransparency() const { return false; } // TODO

		void setShadowed(bool isShadowed) { m_isShadowed = isShadowed; }
		void enableLighting(bool yesNo) { m_enableLight = yesNo; }

		void setPosition(const Float3& pos) { m_pos = pos; }
		void setScale(const Float3& pos)    { m_scale = pos; }
		void setAngle(const Float3& angle)  { m_angle = angle; }

		const Float3& getPosition() const { return m_pos; }
		const Float3& getScale()    const { return m_scale; }
		const Float3& getAngle()    const { return m_angle; }

		void draw() const;

		virtual ~Obj3DAbstract() {}

		virtual void removeFromPicking() = 0;
	protected:
		virtual void drawNoPos() const = 0;
		Float3 m_pos;
		Float3 m_scale;
		Float3 m_angle;
		bool m_enableLight;
		bool m_isShadowed;
	};

	//---------------------------------------------------------------------------------------------
	/**
	* \brief A textured 3D cuboid object
	*/

	class ObjCuboid : public Obj3DAbstract
	{
	public:
		ObjCuboid(const char* textureFile, const Float3& size);
		ObjCuboid(const char* textureFileFaces[6], const Float3& size);
		ObjCuboid(const Material* materialsEachFace[6], const Float3& size);
		~ObjCuboid();
		void removeFromPicking();
	private:
		const std::vector<const VBOGroup*> getVBOGroups() const;
		virtual void drawNoPos() const;
		RenderGroup* m_renderGroup;
		std::vector<Material*> m_materials;
		std::vector<const Material*> m_materialsConst;
	};

	//---------------------------------------------------------------------------------------------
	/**
	* \brief A textured 3D sphere object
	*/

	class ObjSphere : public Obj3DAbstract
	{
	public:
		ObjSphere(const char* textureFile, int verticesDetailLevel);// for example verticesDetailLevel could be 10
		~ObjSphere();
		void removeFromPicking();
	private:
		const std::vector<const VBOGroup*> getVBOGroups() const;
		virtual void drawNoPos() const;
		VBOGroup* m_vboGroup;
		Material* m_material;
	};

	//---------------------------------------------------------------------------------------------
	/**
	* \brief A textured 3D plane (oriented to the top by default)
	*/

	class ObjFaceOrientedToTheTop : public Obj3DAbstract
	{
		friend class ::Water;
	public:
		ObjFaceOrientedToTheTop(const char* textureFile);
		~ObjFaceOrientedToTheTop();
		void removeFromPicking();
	private:
		const std::vector<const VBOGroup*> getVBOGroups() const;
		virtual void drawNoPos() const;
		std::vector<Material*> m_materials;
		RenderGroup* m_renderGroup;
	};

	//---------------------------------------------------------------------------------------------
}

#endif //Obj3D_h_INCLUDED
