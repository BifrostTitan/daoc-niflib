// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_file.h"

#include "niflib/ni_extra_data.h"
#include "niflib/ni_animation.h"
#include "niflib/ni_geometry.h"
#include "niflib/ni_lights.h"
#include "niflib/nif_records.h"
#include "niflib/ni_scene.h"
#include "niflib/ni_scene_variants.h"
#include "niflib/ni_textures.h"
#include "niflib/ni_skinning.h"
#include "niflib/ni_morph.h"
#include "niflib/ni_collision.h"
#include "niflib/ni_particles.h"

#include <limits>
#include <string>

namespace niflib
{
    namespace
    {
        constexpr std::uint32_t version_3_3_0_13 = version_value(eNifVersion::VER_3_3_0_13);
        constexpr std::uint32_t version_5_0_0_1 = version_value(eNifVersion::VER_5_0_0_1);
        constexpr std::uint32_t version_10_1_0_106 = version_value(eNifVersion::VER_10_1_0_106);
        constexpr std::uint32_t maximum_block_count = 1'000'000;
        constexpr std::uint32_t minimum_legacy_type_length = 6;
        constexpr std::uint32_t maximum_legacy_type_length = 30;

        std::string read_legacy_type_name(NifBinaryReader& reader)
        {
            const std::uint32_t length = reader.read_u32();
            if (length < minimum_legacy_type_length || length > maximum_legacy_type_length)
            {
                throw NifFormatError("Invalid legacy NIF block type string length.");
            }
            return reader.read_string(length);
        }

        template <typename T>
        void resolve_reference(
            NiRef<T>& reference,
            const std::unordered_map<std::uint32_t, std::shared_ptr<NiObject>>& objects)
        {
            if (!reference.is_valid())
            {
                return;
            }

            const auto found = objects.find(reference.ref_id());
            if (found == objects.end())
            {
                throw NifFormatError("NIF reference points to a missing object.");
            }

            auto typed_object = std::dynamic_pointer_cast<T>(found->second);
            if (!typed_object)
            {
                throw NifFormatError("NIF reference points to an object of the wrong type.");
            }
            reference.set_object(std::move(typed_object));
        }

        template <typename T>
        void resolve_references(
            std::vector<NiRef<T>>& references,
            const std::unordered_map<std::uint32_t, std::shared_ptr<NiObject>>& objects)
        {
            for (auto& reference : references)
            {
                resolve_reference(reference, objects);
            }
        }
    }

    NiFile::NiFile(std::istream& input)
        : NiFile(input, NifObjectFactory::with_builtin_types())
    {
    }

    NiFile::NiFile(std::istream& input, const NifObjectFactory& factory)
        : header_(NifHeader::read(input))
    {
        NifBinaryReader reader(input);
        read_objects(reader, factory);
        read_footer(reader);
        resolve_object_refs();
    }

    const NifHeader& NiFile::header() const noexcept
    {
        return header_;
    }

    const NiFooter& NiFile::footer() const noexcept
    {
        return footer_;
    }

    const std::unordered_map<std::uint32_t, std::shared_ptr<NiObject>>& NiFile::objects() const noexcept
    {
        return objects_;
    }

    const std::vector<NiRef<NiObject>>& NiFile::root_nodes() const noexcept
    {
        return footer_.root_nodes;
    }

    std::shared_ptr<NiObject> NiFile::object(std::uint32_t ref_id) const
    {
        const auto found = objects_.find(ref_id);
        return found == objects_.end() ? nullptr : found->second;
    }

    void NiFile::read_objects(NifBinaryReader& reader, const NifObjectFactory& factory)
    {
        if (header_.version < version_3_3_0_13)
        {
            while (true)
            {
                const std::string type_name = read_legacy_type_name(reader);
                if (type_name == "Top Level Object")
                {
                    continue;
                }
                if (type_name == "End Of File")
                {
                    return;
                }

                const std::uint32_t ref_id = reader.read_u32();
                auto block = factory.create(type_name, header_.version, header_.user_version, reader);
                if (!objects_.emplace(ref_id, std::move(block)).second)
                {
                    throw NifFormatError("Duplicate NIF object reference ID.");
                }
                if (objects_.size() > maximum_block_count)
                {
                    throw NifFormatError("NIF object count exceeds the supported limit.");
                }
            }
        }

        if (header_.block_count > maximum_block_count)
        {
            throw NifFormatError("NIF block count exceeds the supported limit.");
        }
        for (std::uint32_t index = 0; index < header_.block_count; ++index)
        {
            std::string type_name;
            if (header_.version >= version_5_0_0_1)
            {
                if (header_.version <= version_10_1_0_106 && reader.read_u32() != 0)
                {
                    throw NifFormatError("NIF block marker is not zero.");
                }
                if (index >= header_.block_type_indices.size())
                {
                    throw NifFormatError("Missing NIF block type index.");
                }
                const std::uint16_t type_index = header_.block_type_indices[index];
                if (type_index >= header_.block_types.size())
                {
                    throw NifFormatError("NIF block type index is out of range.");
                }
                type_name = header_.block_types[type_index];
            }
            else
            {
                type_name = read_legacy_type_name(reader);
            }

            const std::uint32_t ref_id = header_.version < version_3_3_0_13
                ? reader.read_u32()
                : index;
            auto block = factory.create(type_name, header_.version, header_.user_version, reader);
            if (!objects_.emplace(ref_id, std::move(block)).second)
            {
                throw NifFormatError("Duplicate NIF object reference ID.");
            }
        }
    }

    void NiFile::read_footer(NifBinaryReader& reader)
    {
        if (header_.version < version_3_3_0_13)
        {
            return;
        }

        const std::uint32_t root_count = reader.read_u32();
        if (root_count > maximum_block_count)
        {
            throw NifFormatError("NIF root count exceeds the supported limit.");
        }
        footer_.root_nodes.reserve(root_count);
        for (std::uint32_t index = 0; index < root_count; ++index)
        {
            footer_.root_nodes.emplace_back(reader.read_u32());
        }
    }

    void NiFile::resolve_object_refs()
    {
        for (const auto& entry : objects_)
        {
            const auto& block = entry.second;
            if (auto object_net = std::dynamic_pointer_cast<NiObjectNET>(block))
            {
                resolve_references(object_net->extra_data, objects_);
                resolve_reference(object_net->controller, objects_);
            }
            if (auto av_object = std::dynamic_pointer_cast<NiAVObject>(block))
            {
                resolve_references(av_object->properties, objects_);
                resolve_reference(av_object->collision_object, objects_);
            }
            if (auto collision = std::dynamic_pointer_cast<NiCollisionObject>(block))
            {
                resolve_reference(collision->target, objects_);
            }
            if (auto modifier = std::dynamic_pointer_cast<NiParticleModifier>(block))
            {
                resolve_reference(modifier->next, objects_);
                resolve_reference(modifier->controller, objects_);
            }
            if (auto particle_controller = std::dynamic_pointer_cast<NiParticleSystemController>(block))
            {
                resolve_reference(particle_controller->emitter, objects_);
                resolve_reference(particle_controller->particle_link, objects_);
                resolve_reference(particle_controller->unknown_ref, objects_);
                resolve_reference(particle_controller->particle_extra, objects_);
                resolve_reference(particle_controller->unknown_ref_2, objects_);
                resolve_reference(particle_controller->color_data, objects_);
            }
            if (auto node = std::dynamic_pointer_cast<NiNode>(block))
            {
                resolve_references(node->children, objects_);
                resolve_references(node->effects, objects_);
                for (auto& child : node->children)
                {
                    if (child.object())
                    {
                        child.object()->parent = node;
                    }
                }
            }
            if (auto effect = std::dynamic_pointer_cast<NiDynamicEffect>(block))
            {
                resolve_references(effect->affected_nodes, objects_);
            }
            if (auto texture_effect = std::dynamic_pointer_cast<NiTextureEffect>(block))
            {
                resolve_reference(texture_effect->source_texture, objects_);
            }
            if (auto geometry = std::dynamic_pointer_cast<NiGeometry>(block))
            {
                resolve_reference(geometry->data, objects_);
                resolve_reference(geometry->skin_instance, objects_);
            }
            if (auto skin_instance = std::dynamic_pointer_cast<NiSkinInstance>(block))
            {
                resolve_reference(skin_instance->data, objects_);
                resolve_reference(skin_instance->partition, objects_);
                resolve_reference(skin_instance->skeleton_root, objects_);
                resolve_references(skin_instance->bones, objects_);
            }
            if (auto skin_data = std::dynamic_pointer_cast<NiSkinData>(block))
            {
                resolve_reference(skin_data->partition, objects_);
            }
            if (auto source_texture = std::dynamic_pointer_cast<NiSourceTexture>(block))
            {
                resolve_reference(source_texture->internal_texture, objects_);
            }
            if (auto render_data = std::dynamic_pointer_cast<ATextureRenderData>(block))
            {
                resolve_reference(render_data->palette, objects_);
            }
            if (auto texturing = std::dynamic_pointer_cast<NiTexturingProperty>(block))
            {
                const auto resolve_descriptor = [this](std::optional<TexDesc>& descriptor)
                {
                    if (descriptor)
                    {
                        resolve_reference(descriptor->source, objects_);
                    }
                };
                resolve_descriptor(texturing->base_texture);
                resolve_descriptor(texturing->dark_texture);
                resolve_descriptor(texturing->detail_texture);
                resolve_descriptor(texturing->gloss_texture);
                resolve_descriptor(texturing->glow_texture);
                resolve_descriptor(texturing->bump_map_texture);
                resolve_descriptor(texturing->decal_0_texture);
                for (auto& shader_texture : texturing->shader_textures)
                {
                    resolve_reference(shader_texture.descriptor.source, objects_);
                }
            }
            if (auto extra_data = std::dynamic_pointer_cast<NiExtraData>(block))
            {
                resolve_reference(extra_data->next_extra_data, objects_);
            }
            if (auto lod_node = std::dynamic_pointer_cast<NiLODNode>(block))
            {
                resolve_reference(lod_node->lod_level_data, objects_);
            }
            if (auto camera = std::dynamic_pointer_cast<NiCamera>(block))
            {
                resolve_reference(camera->unknown_link, objects_);
            }
            if (auto controller = std::dynamic_pointer_cast<NiTimeController>(block))
            {
                resolve_reference(controller->next_controller, objects_);
                resolve_reference(controller->target, objects_);
            }
            if (auto morpher = std::dynamic_pointer_cast<NiGeomMorpherController>(block))
            {
                resolve_reference(morpher->data, objects_);
                resolve_references(morpher->interpolators, objects_);
            }
            if (auto single_controller = std::dynamic_pointer_cast<NiSingleInterpController>(block))
            {
                resolve_reference(single_controller->interpolator, objects_);
            }
            if (auto alpha_controller = std::dynamic_pointer_cast<NiAlphaController>(block))
            {
                resolve_reference(alpha_controller->data, objects_);
            }
            if (auto keyframe_controller = std::dynamic_pointer_cast<NiKeyframeController>(block))
            {
                resolve_reference(keyframe_controller->data, objects_);
            }
            if (auto point_controller = std::dynamic_pointer_cast<NiPoint3InterpController>(block))
            {
                resolve_reference(point_controller->data, objects_);
            }
            if (auto uv_controller = std::dynamic_pointer_cast<NiUVController>(block))
            {
                resolve_reference(uv_controller->data, objects_);
            }
            if (auto visibility_controller = std::dynamic_pointer_cast<NiVisController>(block))
            {
                resolve_reference(visibility_controller->data, objects_);
            }
            if (auto look_at_controller = std::dynamic_pointer_cast<NiLookAtController>(block))
            {
                resolve_reference(look_at_controller->camera_target_node, objects_);
            }
            if (auto texture_controller = std::dynamic_pointer_cast<NiTextureTransformController>(block))
            {
                resolve_reference(texture_controller->data, objects_);
            }
            if (auto path_controller = std::dynamic_pointer_cast<NiPathController>(block))
            {
                resolve_reference(path_controller->position_data, objects_);
                resolve_reference(path_controller->float_data, objects_);
            }
            if (auto particle_color = std::dynamic_pointer_cast<NiParticleColorModifier>(block))
            {
                resolve_reference(particle_color->data, objects_);
            }
            if (auto particle_mesh = std::dynamic_pointer_cast<NiParticleMeshModifier>(block))
            {
                resolve_references(particle_mesh->particle_meshes, objects_);
            }
            if (auto particle_mesh_data = std::dynamic_pointer_cast<NiParticleMeshesData>(block))
            {
                resolve_reference(particle_mesh_data->unknown_link, objects_);
            }
        }

        for (auto& root : footer_.root_nodes)
        {
            resolve_reference(root, objects_);
        }
    }
}