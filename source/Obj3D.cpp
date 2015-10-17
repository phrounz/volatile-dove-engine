
#include "../include/Engine.h"
#include "../include/RenderGroup.h"
#include "../include/Material.h"
#include "../include/Obj3D.h"
#include "../include/Sphere3D.h"
#include "../include/Cube3D.h"

#include "../include/VBOGroup.h"

namespace Obj3D
{
	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------

	void Drawable3DObject::drawBunchOfDrawable3DObjects(const std::set<Drawable3DObject*> objs)
	{
		std::vector<const VBOGroup*> allVBOGroups;
		iterateSetConst(objs, Drawable3DObject*)
		{
			//const std::vector<const VBOGroup*>& vboGroups = (*it)->getVBOGroups();
			//allVBOGroups.insert(allVBOGroups.end(), vboGroups.begin(), vboGroups.end());
			(*it)->draw();
		}
		//VBOGroup::drawBunchOfVBOGroups(Engine::instance().getScene3DMgr().getCamera().getPosition(), allVBOGroups);
	}

	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------

	namespace
	{
		Meshes3D::GroupOfQuads* getGroupOfQuadsForSimpleCube(const Float3& size)
		{
			float4 color(1.f, 1.f, 1.f, 1.f);
			Meshes3D::GroupOfQuads* groupOfQuads = new Meshes3D::GroupOfQuads;
			groupOfQuads->allocateMoreQuads(6);
			for (int i = 0; i < 6; ++i)
			{
				Meshes3D::BasicQuad* basicQuad = groupOfQuads->getLast();
				Meshes3D::Cube3D::getVerticesAndTexCoordCubeFace(static_cast<FaceId>(i),
					basicQuad,
					Float3(0.f, 0.f, 0.f), Float3(1.f, 1.f, 1.f),
					Float3(0.f, 0.f, 0.f), Float3(1.f, 1.f, 1.f),
					size*0.5f, Float3(0.f, 0.f, 0.f), false);
				basicQuad->copyColorFrom((float*)&color);
				groupOfQuads->next();
			}
			return groupOfQuads;
		}

		Meshes3D::GroupOfQuads* getGroupOfQuadsForSimpleCubeFace(const Float3& size, const FaceId faceId)
		{
			float4 color(1.f, 1.f, 1.f, 1.f);
			Meshes3D::GroupOfQuads* groupOfQuads = new Meshes3D::GroupOfQuads;
			groupOfQuads->allocateMoreQuads(1);
			Meshes3D::BasicQuad* basicQuad = groupOfQuads->getLast();
			Meshes3D::Cube3D::getVerticesAndTexCoordCubeFace(faceId,
				basicQuad,
				Float3(0.f, 0.f, 0.f), Float3(1.f, 1.f, 1.f),
				Float3(0.f, 0.f, 0.f), Float3(1.f, 1.f, 1.f),
				size*0.5f, Float3(0.f, 0.f, 0.f), false);
			basicQuad->copyColorFrom((float*)&color);
			groupOfQuads->next();
			return groupOfQuads;
		}

		Meshes3D::GroupOfQuads* getGroupOfQuadsForFaceOrientedToTheTop()
		{
			float4 color(1.f, 1.f, 1.f, 1.f);
			Meshes3D::GroupOfQuads* groupOfQuads = new Meshes3D::GroupOfQuads;
			groupOfQuads->allocateMoreQuads(1);
			Meshes3D::BasicQuad* basicQuad = groupOfQuads->getLast();
			Meshes3D::Cube3D::getVerticesAndTexCoordCubeFace(Meshes3D::Cube3D::FACE_TOP, basicQuad,
				Float3(0.f, 0.f, 0.f), Float3(1.f, 1.f, 1.f), Float3(0.f, 0.f, 0.f), Float3(1.f, 1.f, 1.f), 1.f, Float3(0.f, 0.f, 0.f), false);
			basicQuad->copyColorFrom((float*)&color);
			groupOfQuads->next();
			return groupOfQuads;
		}
	}

	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------

	void Obj3DAbstract::draw() const
	{
		Scene3D& scene3D = Engine::instance().getScene3DMgr();

		scene3D.set3DMode(!m_isShadowed && m_enableLight);//enableLighting

		Float3 prevPos = scene3D.getPosition();
		scene3D.setPositionAndScaleAndAngle(m_pos, m_scale, m_angle);
		this->drawNoPos();
		scene3D.setPosition(prevPos);

		if (!m_isShadowed && m_enableLight)
		{
			scene3D.enableLighting(false);
		}
	}

	float Obj3DAbstract::distanceToCamera() const
	{
		return Engine::instance().getScene3DMgr().getCamera().getPosition().distanceTo(m_pos);
	}

	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------

	ObjCuboid::ObjCuboid(const char* textureFile, const Float3& size)
	{
		Utils::indentLog("Init ObjCuboid...");
		Material* material = new Material(textureFile, false);
		m_materials.push_back(material);

		m_renderGroup = new RenderGroup(NULL);
		outputln("RenderGroup::setGroupOfQuadsForMaterial...");
		m_renderGroup->setGroupOfQuadsForMaterial(*material, getGroupOfQuadsForSimpleCube(size));
		outputln("RenderGroup::finalize...");
		m_renderGroup->finalize(true);
		Utils::unindentLog("Init ObjCuboid done.");
	}

	//---------------------------------------------------------------------------------------------

	ObjCuboid::ObjCuboid(const char* textureFileFaces[6], const Float3& size)
	{
		m_renderGroup = new RenderGroup(NULL);
		for (int i = 0; i < 6; ++i)
		{
			Material* material = new Material(textureFileFaces[i], false);
			m_materials.push_back(material);
			m_renderGroup->addGroupOfQuadsForMaterial(*material, getGroupOfQuadsForSimpleCubeFace(size, static_cast<FaceId>(i)));
		}
		m_renderGroup->finalize(true);
	}

	//---------------------------------------------------------------------------------------------

	ObjCuboid::ObjCuboid(const Material* materialsEachFace[6], const Float3& size)
	{
		m_renderGroup = new RenderGroup(NULL);
		for (int i = 0; i < 6; ++i)
		{
			m_materialsConst.push_back(materialsEachFace[i]);
			m_renderGroup->addGroupOfQuadsForMaterial(*(materialsEachFace[i]), getGroupOfQuadsForSimpleCubeFace(size, static_cast<FaceId>(i)));
		}
		m_renderGroup->finalize(true);
	}

	//---------------------------------------------------------------------------------------------

	const std::vector<const VBOGroup*> ObjCuboid::getVBOGroups() const
	{
		std::vector<const VBOGroup*> output;
		output.push_back(m_renderGroup->getVBOGroup());
		return output;
	}

	//---------------------------------------------------------------------------------------------

	void ObjCuboid::drawNoPos() const
	{
		m_renderGroup->draw(Engine::instance().getScene3DMgr().getCamera().getPosition());
	}

	//---------------------------------------------------------------------------------------------

	ObjCuboid::~ObjCuboid()
	{
		Utils::indentLog("Uninit ObjCuboid...");
		delete m_renderGroup;
		iterateVector(m_materials, Material*)
		{
			delete (*it);
		}
		Utils::unindentLog("Uninit ObjCuboid done.");
	}

	//---------------------------------------------------------------------------------------------

	void ObjCuboid::removeFromPicking()
	{
		m_renderGroup->removeFromPicking();
	}

	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------

	ObjSphere::ObjSphere(const char* textureFile, int verticesDetailLevel)
	{
		Utils::indentLog("Init ObjSphere...");
		int size = -1;
		Meshes3D::BasicVertex* basicVertices = Meshes3D::Sphere3D::getBasicVertices(verticesDetailLevel, 1.f, &size);
		m_material = new Material(textureFile, false);
		m_vboGroup = new VBOGroup;
		m_vboGroup->insert(m_material->getMaterialId(), size, basicVertices);
		delete basicVertices;
		Utils::unindentLog("Init ObjSphere done.");
	}

	//---------------------------------------------------------------------------------------------

	void ObjSphere::drawNoPos() const
	{
		m_vboGroup->draw(Engine::instance().getScene3DMgr().getCamera().getPosition(), false);
	}

	//---------------------------------------------------------------------------------------------

	ObjSphere::~ObjSphere()
	{
		Utils::indentLog("Uninit ObjSphere...");
		delete m_vboGroup;
		delete m_material;
		Utils::unindentLog("Uninit ObjSphere done.");
	}

	//---------------------------------------------------------------------------------------------

	const std::vector<const VBOGroup*> ObjSphere::getVBOGroups() const
	{
		std::vector<const VBOGroup*> output;
		output.push_back(m_vboGroup);
		return output;
	}

	//---------------------------------------------------------------------------------------------

	void ObjSphere::removeFromPicking()
	{
		m_vboGroup->removeFromPicking();
	}

	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------

	ObjFaceOrientedToTheTop::ObjFaceOrientedToTheTop(const char* textureFile)
	{
		Utils::indentLog("Init ObjFaceOrientedToTheTop...");
		Material* material = new Material(textureFile, false);
		m_materials.push_back(material);
		m_renderGroup = new RenderGroup(NULL);
		m_renderGroup->setGroupOfQuadsForMaterial(*material, getGroupOfQuadsForFaceOrientedToTheTop());
		m_renderGroup->finalize(true);
		//no need to delete groupOfQuads
		Utils::unindentLog("Init ObjFaceOrientedToTheTop done.");
	}

	//---------------------------------------------------------------------------------------------

	void ObjFaceOrientedToTheTop::drawNoPos() const
	{
		m_renderGroup->draw(Engine::instance().getScene3DMgr().getCamera().getPosition());
	}

	//---------------------------------------------------------------------------------------------

	const std::vector<const VBOGroup*> ObjFaceOrientedToTheTop::getVBOGroups() const
	{
		std::vector<const VBOGroup*> output;
		output.push_back(m_renderGroup->getVBOGroup());
		return output;
	}

	//---------------------------------------------------------------------------------------------

	ObjFaceOrientedToTheTop::~ObjFaceOrientedToTheTop()
	{
		Utils::indentLog("Uninit ObjFaceOrientedToTheTop...");
		delete m_renderGroup;
		iterateVector(m_materials, Material*)
		{
			delete (*it);
		}
		Utils::unindentLog("Uninit ObjFaceOrientedToTheTop done.");
	}

	//---------------------------------------------------------------------------------------------

	void ObjFaceOrientedToTheTop::removeFromPicking()
	{
		m_renderGroup->removeFromPicking();
	}

	//---------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------
}
