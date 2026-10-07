// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/nif_object_factory.h"

#include "niflib/ni_geometry.h"
#include "niflib/ni_animation.h"
#include "niflib/ni_extra_data.h"
#include "niflib/ni_properties.h"
#include "niflib/ni_lights.h"
#include "niflib/ni_particles.h"
#include "niflib/ni_textures.h"
#include "niflib/ni_skinning.h"
#include "niflib/ni_morph.h"
#include "niflib/ni_collision.h"
#include "niflib/ni_scene.h"
#include "niflib/ni_scene_variants.h"
#include "niflib/nif_records.h"

namespace niflib
{
    void NifObjectFactory::register_type(std::string type_name, Reader reader)
    {
        if (type_name.empty() || !reader)
        {
            throw NifFormatError("NIF block registration requires a name and reader.");
        }
        if (!readers_.emplace(std::move(type_name), std::move(reader)).second)
        {
            throw NifFormatError("Duplicate NIF block type registration.");
        }
    }

    std::shared_ptr<NiObject> NifObjectFactory::create(
        const std::string& type_name,
        std::uint32_t version,
        std::uint32_t user_version,
        NifBinaryReader& reader) const
    {
        const auto found = readers_.find(type_name);
        if (found == readers_.end())
        {
            throw NifFormatError("Unsupported NIF block type: " + type_name);
        }
        return found->second(version, user_version, reader);
    }

    NifObjectFactory NifObjectFactory::with_builtin_types()
    {
        NifObjectFactory factory;
        factory.register_type("NiObject", [](std::uint32_t version, std::uint32_t, NifBinaryReader&)
        {
            return std::make_shared<NiObject>(version);
        });
        factory.register_type("NiObjectNET", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiObjectNET>(version, reader);
        });
        factory.register_type("NiAVObject", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiAVObject>(version, reader);
        });
        factory.register_type("NiExtraData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiExtraData>(version, reader);
        });
        factory.register_type("NiProperty", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiProperty>(version, reader);
        });
        factory.register_type("NiGeometry", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiGeometry>(version, user_version, reader);
        });
        factory.register_type("NiTriBasedGeometry", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiTriBasedGeometry>(version, user_version, reader);
        });
        factory.register_type("NiTriBasedGeomData", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiTriBasedGeomData>(version, user_version, reader);
        });
        factory.register_type("NiLODData", [](std::uint32_t version, std::uint32_t, NifBinaryReader&)
        {
            return std::make_shared<NiLODData>(version);
        });
        factory.register_type("NiLight", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiLight>(version, reader);
        });
        factory.register_type("NiParticleModifier", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiParticleModifier>(version, reader);
        });
        factory.register_type("NiInterpController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiInterpController>(version, reader);
        });
        factory.register_type("NiSingleInterpController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiSingleInterpController>(version, reader);
        });
        factory.register_type("NiFloatInterpController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiFloatInterpController>(version, reader);
        });
        factory.register_type("NiNode", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiNode>(version, reader);
        });
        factory.register_type("NiCamera", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiCamera>(version, reader);
        });
        factory.register_type("NiSwitchNode", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiSwitchNode>(version, reader);
        });
        factory.register_type("NiBillboardNode", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiBillboardNode>(version, reader);
        });
        factory.register_type("NiLODNode", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiLODNode>(version, reader);
        });
        factory.register_type("NiTriShape", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiTriShape>(version, user_version, reader);
        });
        factory.register_type("NiGeometryData", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiGeometryData>(version, user_version, reader);
        });
        factory.register_type("NiTriShapeData", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiTriShapeData>(version, user_version, reader);
        });
        factory.register_type("NiTriStrips", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiTriStrips>(version, user_version, reader);
        });
        factory.register_type("NiTriStripsData", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiTriStripsData>(version, user_version, reader);
        });
        factory.register_type("NiPalette", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiPalette>(version, reader);
        });
        factory.register_type("NiPixelData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiPixelData>(version, reader);
        });
        factory.register_type("NiSkinInstance", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiSkinInstance>(version, reader);
        });
        factory.register_type("NiSkinData", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiSkinData>(version, user_version, reader);
        });
        factory.register_type("NiSkinPartition", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiSkinPartition>(version, user_version, reader);
        });
        factory.register_type("NiMorphData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiMorphData>(version, reader);
        });
        factory.register_type("NiGeomMorpherController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiGeomMorpherController>(version, reader);
        });
        factory.register_type("NiCollisionObject", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiCollisionObject>(version, reader);
        });
        factory.register_type("NiGravity", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiGravity>(version, reader);
        });
        factory.register_type("NiPlanarCollider", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiPlanarCollider>(version, reader);
        });
        factory.register_type("NiSphericalCollider", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiSphericalCollider>(version, reader);
        });
        factory.register_type("NiParticleColorModifier", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiParticleColorModifier>(version, reader);
        });
        factory.register_type("NiParticleGrowFade", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiParticleGrowFade>(version, reader);
        });
        factory.register_type("NiParticleRotation", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiParticleRotation>(version, reader);
        });
        factory.register_type("NiParticleMeshModifier", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiParticleMeshModifier>(version, reader);
        });
        factory.register_type("NiPathController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiPathController>(version, reader);
        });
        factory.register_type("NiParticles", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiParticles>(version, user_version, reader);
        });
        factory.register_type("NiAutoNormalParticles", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiAutoNormalParticles>(version, user_version, reader);
        });
        factory.register_type("NiRotatingParticles", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiRotatingParticles>(version, user_version, reader);
        });
            factory.register_type("NiParticleMeshesData", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
            {
                return std::make_shared<NiParticleMeshesData>(version, user_version, reader);
            });
        factory.register_type("NiParticleMeshes", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiParticleMeshes>(version, user_version, reader);
        });
            factory.register_type("NiParticleBomb", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
            {
                return std::make_shared<NiParticleBomb>(version, reader);
            });
        factory.register_type("NiParticleSystemController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiParticleSystemController>(version, reader);
        });
            factory.register_type("NiTextureEffect", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
            {
                return std::make_shared<NiTextureEffect>(version, reader);
            });
        factory.register_type("NiParticlesData", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiParticlesData>(version, user_version, reader);
        });
        factory.register_type("NiAutoNormalParticlesData", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiAutoNormalParticlesData>(version, user_version, reader);
        });
        factory.register_type("NiRotatingParticlesData", [](std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        {
            return std::make_shared<NiRotatingParticlesData>(version, user_version, reader);
        });
        factory.register_type("NiRangeLODData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiRangeLODData>(version, reader);
        });
        factory.register_type("NiScreenLODData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiScreenLODData>(version, reader);
        });
        factory.register_type("NiStringExtraData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiStringExtraData>(version, reader);
        });
        factory.register_type("NiBinaryExtraData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiBinaryExtraData>(version, reader);
        });
        factory.register_type("NiIntegerExtraData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiIntegerExtraData>(version, reader);
        });
        factory.register_type("NiBooleanExtraData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiBooleanExtraData>(version, reader);
        });
        factory.register_type("NiFloatExtraData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiFloatExtraData>(version, reader);
        });
        factory.register_type("NiColorExtraData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiColorExtraData>(version, reader);
        });
        factory.register_type("NiVectorExtraData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiVectorExtraData>(version, reader);
        });
        factory.register_type("NiIntegersExtraData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiIntegersExtraData>(version, reader);
        });
        factory.register_type("NiStringsExtraData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiStringsExtraData>(version, reader);
        });
        factory.register_type("NiTextKeyExtraData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiTextKeyExtraData>(version, reader);
        });
        factory.register_type("NiAlphaProperty", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiAlphaProperty>(version, reader);
        });
        factory.register_type("NiMaterialProperty", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiMaterialProperty>(version, reader);
        });
        factory.register_type("NiTexturingProperty", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiTexturingProperty>(version, reader);
        });
        factory.register_type("NiVertexColorProperty", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiVertexColorProperty>(version, reader);
        });
        factory.register_type("NiSpecularProperty", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiSpecularProperty>(version, reader);
        });
        factory.register_type("NiShadeProperty", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiShadeProperty>(version, reader);
        });
        factory.register_type("NiDitherProperty", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiDitherProperty>(version, reader);
        });
        factory.register_type("NiWireframeProperty", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiWireframeProperty>(version, reader);
        });
        factory.register_type("NiZBufferProperty", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiZBufferProperty>(version, reader);
        });
        factory.register_type("NiStencilProperty", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiStencilProperty>(version, reader);
        });
        factory.register_type("NiFogProperty", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiFogProperty>(version, reader);
        });
        factory.register_type("NiDynamicEffect", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiDynamicEffect>(version, reader);
        });
        factory.register_type("NiAmbientLight", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiAmbientLight>(version, reader);
        });
        factory.register_type("NiDirectionalLight", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiDirectionalLight>(version, reader);
        });
        factory.register_type("NiPointLight", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiPointLight>(version, reader);
        });
        factory.register_type("NiSpotLight", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiSpotLight>(version, reader);
        });
        factory.register_type("NiTimeController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiTimeController>(version, reader);
        });
        factory.register_type("NiAlphaController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiAlphaController>(version, reader);
        });
        factory.register_type("NiKeyframeController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiKeyframeController>(version, reader);
        });
        factory.register_type("NiBoolInterpController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiBoolInterpController>(version, reader);
        });
        factory.register_type("NiPoint3InterpController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiPoint3InterpController>(version, reader);
        });
        factory.register_type("NiMaterialColorController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiMaterialColorController>(version, reader);
        });
        factory.register_type("NiLightColorController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiLightColorController>(version, reader);
        });
        factory.register_type("NiUVController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiUVController>(version, reader);
        });
        factory.register_type("NiVisController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiVisController>(version, reader);
        });
        factory.register_type("NiLookAtController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiLookAtController>(version, reader);
        });
        factory.register_type("NiTextureTransformController", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiTextureTransformController>(version, reader);
        });
        factory.register_type("NiFloatData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiFloatData>(version, reader);
        });
        factory.register_type("NiPosData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiPosData>(version, reader);
        });
        factory.register_type("NiColorData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiColorData>(version, reader);
        });
        factory.register_type("NiVisData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiVisData>(version, reader);
        });
        factory.register_type("NiKeyframeData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiKeyframeData>(version, reader);
        });
        factory.register_type("NiUVData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiUVData>(version, reader);
        });
        factory.register_type("NiInterpolator", [](std::uint32_t version, std::uint32_t, NifBinaryReader&)
        {
            return std::make_shared<NiInterpolator>(version);
        });
        factory.register_type("ATextureRenderData", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<ATextureRenderData>(version, reader);
        });
        factory.register_type("NiTexture", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiTexture>(version, reader);
        });
        factory.register_type("NiSourceTexture", [](std::uint32_t version, std::uint32_t, NifBinaryReader& reader)
        {
            return std::make_shared<NiSourceTexture>(version, reader);
        });
        return factory;
    }
}