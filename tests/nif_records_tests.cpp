#include "niflib/nif_binary_reader.h"
#include "niflib/nif_header.h"
#include "niflib/nif_records.h"
#include "niflib/ni_geometry.h"
#include "niflib/ni_file.h"
#include "niflib/ni_extra_data.h"
#include "niflib/ni_animation.h"
#include "niflib/ni_properties.h"
#include "niflib/ni_lights.h"
#include "niflib/ni_particles.h"
#include "niflib/ni_textures.h"
#include "niflib/ni_skinning.h"
#include "niflib/ni_morph.h"
#include "niflib/ni_collision.h"
#include "niflib/ni_string.h"
#include "niflib/ni_scene.h"
#include "niflib/ni_scene_variants.h"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

static_assert(sizeof(niflib::eBillboardMode) == sizeof(std::uint16_t));
static_assert(static_cast<std::uint16_t>(niflib::eBillboardMode::ROTATE_ABOUT_UP2) == 9);
static_assert(static_cast<std::uint32_t>(niflib::eChannelConvention::CC_INDEX) == 3);
static_assert(static_cast<std::uint32_t>(niflib::eChannelConvention::CC_EMPTY) == 5);
static_assert(static_cast<std::uint32_t>(niflib::eMipMapFormat::MIP_FMT_DEFAULT) == 2);
static_assert(static_cast<std::uint32_t>(niflib::ePixelLayout::PIX_LAY_DEFAULT) == 6);
static_assert(static_cast<std::uint32_t>(niflib::eStencilAction::ACTION_INVERT) == 5);
static_assert(static_cast<std::uint32_t>(niflib::eStencilCompareMode::TEST_ALWAYS) == 7);
static_assert(static_cast<std::uint32_t>(niflib::eSymmetryType::PLANAR_SYMMETRY) == 2);
static_assert(sizeof(niflib::eTargetColor) == sizeof(std::uint16_t));
static_assert(static_cast<std::uint32_t>(niflib::eTexFilterMode::FILTER_BILERP_MIPNEAREST) == 5);
static_assert(static_cast<std::uint32_t>(niflib::eTexType::DECAL_3_MAP) == 11);
static_assert(sizeof(niflib::NiHeader) == sizeof(niflib::NifHeader));
static_assert(sizeof(niflib::SkinPartitionUnkownItem1) == sizeof(niflib::SkinPartitionUnknownItem));

namespace
{
    void check(bool condition, const char *message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    void append_u8(std::string &bytes, std::uint8_t value)
    {
        bytes.push_back(static_cast<char>(value));
    }

    void append_u16(std::string &bytes, std::uint16_t value)
    {
        append_u8(bytes, static_cast<std::uint8_t>(value));
        append_u8(bytes, static_cast<std::uint8_t>(value >> 8));
    }

    void append_u32(std::string &bytes, std::uint32_t value)
    {
        append_u8(bytes, static_cast<std::uint8_t>(value));
        append_u8(bytes, static_cast<std::uint8_t>(value >> 8));
        append_u8(bytes, static_cast<std::uint8_t>(value >> 16));
        append_u8(bytes, static_cast<std::uint8_t>(value >> 24));
    }

    void append_f32(std::string &bytes, float value)
    {
        std::uint32_t bits = 0;
        std::memcpy(&bits, &value, sizeof(bits));
        append_u32(bytes, bits);
    }

    void append_color(std::string &bytes, float red, float green, float blue, float alpha)
    {
        append_f32(bytes, red);
        append_f32(bytes, green);
        append_f32(bytes, blue);
        append_f32(bytes, alpha);
    }

    void test_header_block_table()
    {
        std::string bytes = "Gamebryo File Format, Version 10.0.1.0\n";
        append_u32(bytes, niflib::version_value(niflib::eNifVersion::VER_10_0_1_0));
        append_u32(bytes, 1);
        append_u16(bytes, 1);
        append_u32(bytes, 6);
        bytes += "NiNode";
        append_u16(bytes, 0);
        append_u32(bytes, 0);

        std::istringstream input(bytes);
        const niflib::NifHeader header = niflib::NifHeader::read(input);
        check(header.block_count == 1, "header block count mismatch");
        check(header.block_types.size() == 1 && header.block_types[0] == "NiNode", "header block type mismatch");
        check(header.block_type_indices.size() == 1 && header.block_type_indices[0] == 0, "header block index mismatch");
    }

    void test_linear_byte_key()
    {
        std::string bytes;
        append_f32(bytes, 1.5f);
        append_u8(bytes, 42);
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::ByteKey key(reader, niflib::eKeyType::LINEAR_KEY);
        check(key.time == 1.5f && key.value == 42, "linear byte key mismatch");
    }

    void test_quadratic_color_key()
    {
        std::string bytes;
        append_f32(bytes, 2.0f);
        append_color(bytes, 0.1f, 0.2f, 0.3f, 0.4f);
        append_color(bytes, 1.1f, 1.2f, 1.3f, 1.4f);
        append_color(bytes, 2.1f, 2.2f, 2.3f, 2.4f);
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::Color4Key key(reader, niflib::eKeyType::QUADRATIC_KEY);
        check(key.time == 2.0f && key.value.g == 0.2f, "quadratic color key value mismatch");
        check(key.forward.r == 1.1f && key.backward.a == 2.4f, "quadratic color tangents mismatch");
    }

    void test_float_key_group()
    {
        std::string bytes;
        append_u32(bytes, 2);
        append_u32(bytes, static_cast<std::uint32_t>(niflib::eKeyType::QUADRATIC_KEY));
        append_f32(bytes, 1.0f);
        append_f32(bytes, 2.0f);
        append_f32(bytes, 3.0f);
        append_f32(bytes, 4.0f);
        append_f32(bytes, 5.0f);
        append_f32(bytes, 6.0f);
        append_f32(bytes, 7.0f);
        append_f32(bytes, 8.0f);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::KeyGroup<niflib::FloatKey> keys(reader);
        check(keys.interpolation == niflib::eKeyType::QUADRATIC_KEY && keys.values.size() == 2,
              "float key group header mismatch");
        check(keys.values[0].time == 1.0f && keys.values[0].backward == 4.0f,
              "first float key mismatch");
        check(keys.values[1].time == 5.0f && keys.values[1].forward == 7.0f,
              "second float key mismatch");
    }

    void test_legacy_lod_range()
    {
        std::string bytes;
        append_f32(bytes, 10.0f);
        append_f32(bytes, 100.0f);
        append_u32(bytes, 1);
        append_u32(bytes, 2);
        append_u32(bytes, 3);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::LODRange range(niflib::version_value(niflib::eNifVersion::VER_3_1), reader);
        check(range.near_extent == 10.0f && range.far_extent == 100.0f, "LOD extents mismatch");
        check(range.unknown_ints && (*range.unknown_ints)[2] == 3, "legacy LOD data mismatch");
    }

    void test_minimal_node()
    {
        std::string bytes;
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiTimeController>::invalid_ref);
        append_u16(bytes, 0x1234);
        for (int index = 0; index < 3; ++index)
        {
            append_f32(bytes, 0.0f);
        }
        for (int index = 0; index < 9; ++index)
        {
            append_f32(bytes, index == 0 || index == 4 || index == 8 ? 1.0f : 0.0f);
        }
        append_f32(bytes, 1.0f);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiCollisionObject>::invalid_ref);
        append_u32(bytes, 0);
        append_u32(bytes, 0);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiNode node(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(node.flags == 0x1234 && node.scale == 1.0f, "node transform mismatch");
        check(node.children.empty() && node.effects.empty(), "empty node references mismatch");
        check(node.controller.ref_id() == niflib::NiRef<niflib::NiTimeController>::invalid_ref,
              "node controller reference mismatch");
    }

    void test_triangle_geometry_data()
    {
        std::string bytes;
        append_u16(bytes, 1);
        append_u8(bytes, 1);
        append_u8(bytes, 0);
        append_u8(bytes, 1);
        append_f32(bytes, 1.0f);
        append_f32(bytes, 2.0f);
        append_f32(bytes, 3.0f);
        append_u8(bytes, 0);
        append_u8(bytes, 0);
        append_u8(bytes, 0);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 10.0f);
        append_u8(bytes, 0);
        append_u16(bytes, 0);
        append_u16(bytes, 1);
        append_u32(bytes, 3);
        append_u8(bytes, 1);
        append_u16(bytes, 0);
        append_u16(bytes, 0);
        append_u16(bytes, 0);
        append_u16(bytes, 0);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiTriShapeData data(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), 0, reader);
        check(data.num_vertices == 1 && data.vertices.size() == 1, "geometry vertex count mismatch");
        check(data.vertices[0].x == 1.0f && data.vertices[0].z == 3.0f, "geometry vertex mismatch");
        check(data.num_triangles == 1 && data.triangles.size() == 1, "triangle count mismatch");
        check(data.has_triangles && data.num_triangle_points == 3, "triangle metadata mismatch");
    }

    void test_minimal_triangle_shape()
    {
        std::string bytes;
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiTimeController>::invalid_ref);
        append_u16(bytes, 0x0042);
        for (int index = 0; index < 3; ++index)
        {
            append_f32(bytes, 0.0f);
        }
        for (int index = 0; index < 9; ++index)
        {
            append_f32(bytes, index == 0 || index == 4 || index == 8 ? 1.0f : 0.0f);
        }
        append_f32(bytes, 1.0f);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiCollisionObject>::invalid_ref);
        append_u32(bytes, 12);
        append_u32(bytes, 13);
        append_u8(bytes, 0);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiTriShape shape(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), 0, reader);
        check(shape.flags == 0x0042 && shape.data.ref_id() == 12, "triangle shape base/data mismatch");
        check(shape.skin_instance.ref_id() == 13 && !shape.has_shader, "triangle shape references mismatch");
    }

    void test_complete_node_file()
    {
        std::string bytes = "Gamebryo File Format, Version 10.1.0.0\n";
        append_u32(bytes, niflib::version_value(niflib::eNifVersion::VER_10_1_0_0));
        append_u32(bytes, 0);
        append_u32(bytes, 1);
        append_u16(bytes, 1);
        append_u32(bytes, 6);
        bytes += "NiNode";
        append_u16(bytes, 0);
        append_u32(bytes, 0);

        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiTimeController>::invalid_ref);
        append_u16(bytes, 0x0010);
        for (int index = 0; index < 3; ++index)
        {
            append_f32(bytes, 0.0f);
        }
        for (int index = 0; index < 9; ++index)
        {
            append_f32(bytes, index == 0 || index == 4 || index == 8 ? 1.0f : 0.0f);
        }
        append_f32(bytes, 1.0f);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiCollisionObject>::invalid_ref);
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, 1);
        append_u32(bytes, 0);

        std::istringstream input(bytes);
        const niflib::NiFile file(input);
        check(file.objects().size() == 1, "complete file block count mismatch");
        check(file.footer().root_nodes.size() == 1, "complete file footer mismatch");
        check(file.root_nodes().size() == 1 && file.root_nodes()[0].object(), "complete file root resolution failed");
        check(file.root_nodes()[0].object()->version() == niflib::version_value(niflib::eNifVersion::VER_10_1_0_0),
              "complete file root version mismatch");
    }

    void append_empty_node_block(std::string &bytes, std::uint32_t child_ref)
    {
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiTimeController>::invalid_ref);
        append_u16(bytes, 0x0010);
        for (int index = 0; index < 3; ++index)
        {
            append_f32(bytes, 0.0f);
        }
        for (int index = 0; index < 9; ++index)
        {
            append_f32(bytes, index == 0 || index == 4 || index == 8 ? 1.0f : 0.0f);
        }
        append_f32(bytes, 1.0f);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiCollisionObject>::invalid_ref);
        append_u32(bytes, child_ref == niflib::NiRef<niflib::NiAVObject>::invalid_ref ? 0 : 1);
        if (child_ref != niflib::NiRef<niflib::NiAVObject>::invalid_ref)
        {
            append_u32(bytes, child_ref);
        }
        append_u32(bytes, 0);
    }

    void test_child_reference_resolution()
    {
        std::string bytes = "Gamebryo File Format, Version 10.1.0.0\n";
        append_u32(bytes, niflib::version_value(niflib::eNifVersion::VER_10_1_0_0));
        append_u32(bytes, 0);
        append_u32(bytes, 2);
        append_u16(bytes, 1);
        append_u32(bytes, 6);
        bytes += "NiNode";
        append_u16(bytes, 0);
        append_u16(bytes, 0);
        append_u32(bytes, 0);

        append_empty_node_block(bytes, 1);
        append_empty_node_block(bytes, niflib::NiRef<niflib::NiAVObject>::invalid_ref);
        append_u32(bytes, 1);
        append_u32(bytes, 0);

        std::istringstream input(bytes);
        const niflib::NiFile file(input);
        const auto root = std::dynamic_pointer_cast<niflib::NiNode>(file.object(0));
        const auto child = std::dynamic_pointer_cast<niflib::NiNode>(file.object(1));
        check(root && child && root->children.size() == 1, "child object lookup failed");
        check(root->children[0].object() == child, "child reference resolution failed");
        check(child->parent.lock() == root, "child parent relationship failed");
    }

    void test_billboard_node()
    {
        std::string bytes;
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiTimeController>::invalid_ref);
        append_u16(bytes, 0x0010);
        for (int index = 0; index < 3; ++index)
        {
            append_f32(bytes, 0.0f);
        }
        for (int index = 0; index < 9; ++index)
        {
            append_f32(bytes, index == 0 || index == 4 || index == 8 ? 1.0f : 0.0f);
        }
        append_f32(bytes, 1.0f);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiCollisionObject>::invalid_ref);
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u16(bytes, 9);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiBillboardNode node(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(node.billboard_mode == niflib::eBillboardMode::ROTATE_ABOUT_UP2,
              "billboard mode mismatch");
    }

    void test_camera_frustum()
    {
        std::string bytes;
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiTimeController>::invalid_ref);
        append_u16(bytes, 0);
        for (int index = 0; index < 3; ++index)
        {
            append_f32(bytes, 0.0f);
        }
        for (int index = 0; index < 9; ++index)
        {
            append_f32(bytes, index == 0 || index == 4 || index == 8 ? 1.0f : 0.0f);
        }
        append_f32(bytes, 1.0f);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiCollisionObject>::invalid_ref);
        append_u16(bytes, 0);
        for (int index = 0; index < 6; ++index)
        {
            append_f32(bytes, static_cast<float>(index + 1));
        }
        append_u8(bytes, 1);
        for (int index = 0; index < 5; ++index)
        {
            append_f32(bytes, static_cast<float>(index + 7));
        }
        append_u32(bytes, 0xFFFFFFFF);
        append_u32(bytes, 42);
        append_u32(bytes, 43);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiCamera camera(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(camera.frustum_left == 1.0f && camera.frustum_far == 6.0f &&
                  camera.use_orthographic_projection,
              "camera frustum fields mismatch");
        check(camera.viewport_left == 7.0f && camera.lod_adjust == 11.0f && camera.unknown_3 == 43,
              "camera viewport fields mismatch");
    }

    void test_triangle_strip_data()
    {
        std::string bytes;
        append_u16(bytes, 0);
        append_u8(bytes, 0);
        append_u8(bytes, 0);
        append_u8(bytes, 0);
        append_u8(bytes, 0);
        append_u8(bytes, 0);
        append_u8(bytes, 0);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 0.0f);
        append_u8(bytes, 0);
        append_u16(bytes, 0);
        append_u16(bytes, 0);
        append_u16(bytes, 1);
        append_u16(bytes, 3);
        append_u8(bytes, 1);
        append_u16(bytes, 4);
        append_u16(bytes, 5);
        append_u16(bytes, 6);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiTriStripsData data(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), 0, reader);
        check(data.has_points && data.points.size() == 1 && data.points[0].size() == 3,
              "strip point array mismatch");
        check(data.points[0][0] == 4 && data.points[0][2] == 6, "strip indices mismatch");
    }

    void test_named_integer_extra_data()
    {
        std::string bytes;
        append_u32(bytes, 4);
        bytes += "Test";
        append_u32(bytes, 0x12345678);
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiIntegerExtraData data(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(data.name == "Test" && data.data == 0x12345678, "named integer extra data mismatch");
    }

    void test_integer_array_extra_data()
    {
        std::string bytes;
        append_u32(bytes, 0);
        append_u32(bytes, 2);
        append_u32(bytes, 17);
        append_u32(bytes, 29);
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiIntegersExtraData data(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(data.extra_int_data.size() == 2 && data.extra_int_data[1] == 29,
              "integer array extra data mismatch");
    }

    void test_binary_extra_data()
    {
        std::string bytes;
        append_u32(bytes, 0);
        append_u32(bytes, 3);
        append_u8(bytes, 0x10);
        append_u8(bytes, 0x20);
        append_u8(bytes, 0x30);
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiBinaryExtraData data(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(data.data.size() == 3 && data.data[0] == 0x10 && data.data[2] == 0x30,
              "binary extra data payload mismatch");
    }

    void test_text_key_extra_data()
    {
        std::string bytes;
        append_u32(bytes, 0);
        append_u32(bytes, 1);
        append_f32(bytes, 2.5f);
        append_u32(bytes, 4);
        bytes += "open";
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiTextKeyExtraData data(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(data.text_keys.size() == 1 && data.text_keys[0].time == 2.5f &&
                  data.text_keys[0].value == "open",
              "text-key extra data mismatch");
    }

    void test_chained_string_extra_data()
    {
        std::string bytes;
        append_u32(bytes, 7);
        append_u32(bytes, 0xFFFFFFFF);
        append_u32(bytes, 3);
        bytes += "old";
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiStringExtraData data(
            niflib::version_value(niflib::eNifVersion::VER_4_2_2_0), reader);
        check(data.next_extra_data.ref_id() == 7, "old extra-data link mismatch");
        check(data.bytes_remaining == 0xFFFFFFFF && data.string_data == "old",
              "old string extra data mismatch");
    }

    void test_material_property()
    {
        std::string bytes;
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiTimeController>::invalid_ref);
        append_f32(bytes, 0.1f);
        append_f32(bytes, 0.2f);
        append_f32(bytes, 0.3f);
        append_f32(bytes, 0.4f);
        append_f32(bytes, 0.5f);
        append_f32(bytes, 0.6f);
        append_f32(bytes, 0.7f);
        append_f32(bytes, 0.8f);
        append_f32(bytes, 0.9f);
        append_f32(bytes, 1.0f);
        append_f32(bytes, 0.9f);
        append_f32(bytes, 0.8f);
        append_f32(bytes, 32.0f);
        append_f32(bytes, 0.75f);
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiMaterialProperty property(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(property.ambient_color.x == 0.1f && property.diffuse_color.z == 0.6f,
              "material colors mismatch");
        check(property.glossiness == 32.0f && property.alpha == 0.75f,
              "material scalar properties mismatch");
    }

    void test_alpha_controller()
    {
        std::string bytes;
        append_u32(bytes, 0xFFFFFFFF);
        append_u16(bytes, 0x0002);
        append_f32(bytes, 1.0f);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 2.0f);
        append_f32(bytes, 3.0f);
        append_u32(bytes, 0xFFFFFFFF);
        append_u32(bytes, 42);
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiAlphaController controller(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(controller.flags == 2 && controller.start_time == 2.0f && controller.stop_time == 3.0f,
              "alpha controller time fields mismatch");
        check(controller.data.ref_id() == 42 && !controller.interpolator.is_valid(),
              "alpha controller versioned references mismatch");
    }

    void test_vector_and_visibility_tracks()
    {
        std::string vector_bytes;
        append_u32(vector_bytes, 1);
        append_u32(vector_bytes, static_cast<std::uint32_t>(niflib::eKeyType::TBC_KEY));
        append_f32(vector_bytes, 0.5f);
        append_f32(vector_bytes, 1.0f);
        append_f32(vector_bytes, 2.0f);
        append_f32(vector_bytes, 3.0f);
        append_f32(vector_bytes, 4.0f);
        append_f32(vector_bytes, 5.0f);
        append_f32(vector_bytes, 6.0f);
        std::istringstream vector_input(vector_bytes);
        niflib::NifBinaryReader vector_reader(vector_input);
        const niflib::NiPosData position(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), vector_reader);
        check(position.data.values.size() == 1 && position.data.values[0].value.z == 3.0f &&
                  position.data.values[0].tbc.x == 4.0f,
              "TBC vector key mismatch");

        std::string visibility_bytes;
        append_u32(visibility_bytes, 1);
        append_f32(visibility_bytes, 2.0f);
        append_u8(visibility_bytes, 1);
        std::istringstream visibility_input(visibility_bytes);
        niflib::NifBinaryReader visibility_reader(visibility_input);
        const niflib::NiVisData visibility(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), visibility_reader);
        check(visibility.keys.size() == 1 && visibility.keys[0].time == 2.0f &&
                  visibility.keys[0].value == 1,
              "visibility key mismatch");
    }

    void test_keyframe_track_order()
    {
        std::string bytes;
        append_u32(bytes, 1);
        append_u32(bytes, static_cast<std::uint32_t>(niflib::eKeyType::LINEAR_KEY));
        append_f32(bytes, 0.25f);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 1.0f);
        append_u32(bytes, 1);
        append_u32(bytes, static_cast<std::uint32_t>(niflib::eKeyType::LINEAR_KEY));
        append_f32(bytes, 1.0f);
        append_f32(bytes, 2.0f);
        append_f32(bytes, 3.0f);
        append_f32(bytes, 4.0f);
        append_u32(bytes, 1);
        append_u32(bytes, static_cast<std::uint32_t>(niflib::eKeyType::LINEAR_KEY));
        append_f32(bytes, 2.0f);
        append_f32(bytes, 3.0f);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiKeyframeData data(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(data.quaternion_keys.size() == 1 && data.quaternion_keys[0].value.w == 1.0f,
              "keyframe quaternion track mismatch");
        check(data.translations.values.size() == 1 && data.translations.values[0].value.z == 4.0f,
              "keyframe translation track mismatch");
        check(data.scales.values.size() == 1 && data.scales.values[0].value == 3.0f,
              "keyframe scale track mismatch");
    }

    void test_point3_controller()
    {
        std::string bytes;
        append_u32(bytes, 0xFFFFFFFF);
        append_u16(bytes, 1);
        append_f32(bytes, 1.0f);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 5.0f);
        append_u32(bytes, 0xFFFFFFFF);
        append_u16(bytes, static_cast<std::uint16_t>(niflib::eTargetColor::TC_SPECULAR));
        append_u32(bytes, 31);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiPoint3InterpController controller(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(controller.target_color == niflib::eTargetColor::TC_SPECULAR &&
                  controller.data.ref_id() == 31,
              "point controller versioned fields mismatch");
    }

    void test_texture_transform_controller()
    {
        std::string bytes;
        append_u32(bytes, 0xFFFFFFFF);
        append_u16(bytes, 1);
        append_f32(bytes, 1.0f);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 1.0f);
        append_u32(bytes, 0xFFFFFFFF);
        append_u8(bytes, 7);
        append_u32(bytes, static_cast<std::uint32_t>(niflib::eTexType::BUMP_MAP));
        append_u32(bytes, static_cast<std::uint32_t>(niflib::eTexTransform::TT_ROTATE));
        append_u32(bytes, 55);
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiTextureTransformController controller(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(controller.unknown_2 == 7 && controller.texture_slot == niflib::eTexType::BUMP_MAP &&
                  controller.operation == niflib::eTexTransform::TT_ROTATE && controller.data.ref_id() == 55,
              "texture transform controller fields mismatch");
    }

    void test_dynamic_effect()
    {
        std::string bytes;
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiTimeController>::invalid_ref);
        append_u16(bytes, 1);
        for (int index = 0; index < 3; ++index)
        {
            append_f32(bytes, 0.0f);
        }
        for (int index = 0; index < 9; ++index)
        {
            append_f32(bytes, index == 0 || index == 4 || index == 8 ? 1.0f : 0.0f);
        }
        append_f32(bytes, 1.0f);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiCollisionObject>::invalid_ref);
        append_u32(bytes, 1);
        append_u32(bytes, 12);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiDynamicEffect effect(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(effect.switch_state && effect.affected_nodes.size() == 1 &&
                  effect.affected_nodes[0].ref_id() == 12,
              "dynamic effect fields mismatch");
    }

    void test_particle_record()
    {
        std::string bytes;
        append_f32(bytes, 1.0f);
        append_f32(bytes, 2.0f);
        append_f32(bytes, 3.0f);
        append_f32(bytes, 4.0f);
        append_f32(bytes, 5.0f);
        append_f32(bytes, 6.0f);
        append_f32(bytes, 7.0f);
        append_f32(bytes, 8.0f);
        append_f32(bytes, 9.0f);
        append_u16(bytes, 10);
        append_u16(bytes, 11);
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::Particle particle(reader);
        check(particle.velocity.x == 1.0f && particle.unknown_vector.z == 6.0f,
              "particle vector fields mismatch");
        check(particle.timestamp == 9.0f && particle.unknown_short == 10 && particle.vertex_id == 11,
              "particle scalar fields mismatch");
    }

    void test_external_source_texture()
    {
        std::string bytes;
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiTimeController>::invalid_ref);
        append_u8(bytes, 1);
        append_u32(bytes, 11);
        bytes += "diffuse.dds";
        append_u32(bytes, 0);
        append_u32(bytes, static_cast<std::uint32_t>(niflib::ePixelLayout::PIX_LAY_COMPRESSED));
        append_u32(bytes, static_cast<std::uint32_t>(niflib::eMipMapFormat::MIP_FMT_YES));
        append_u32(bytes, static_cast<std::uint32_t>(niflib::eAlphaFormat::ALPHA_SMOOTH));
        append_u8(bytes, 1);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiSourceTexture texture(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(texture.use_external && texture.file_name == "diffuse.dds", "external texture path mismatch");
        check(texture.pixel_layout == niflib::ePixelLayout::PIX_LAY_COMPRESSED &&
                  texture.use_mipmaps == niflib::eMipMapFormat::MIP_FMT_YES && texture.is_static,
              "external texture settings mismatch");
    }

    void test_palette_bytes()
    {
        std::string bytes;
        append_u8(bytes, 7);
        append_u32(bytes, 1);
        append_u8(bytes, 255);
        append_u8(bytes, 128);
        append_u8(bytes, 0);
        append_u8(bytes, 64);
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiPalette palette(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(palette.unknown_byte == 7 && palette.palette.size() == 1, "palette header mismatch");
        check(palette.palette[0].r == 1.0f && palette.palette[0].b == 0.0f,
              "palette byte color mismatch");
    }

    void test_pixel_data_payload()
    {
        std::string bytes;
        append_u32(bytes, static_cast<std::uint32_t>(niflib::ePixelFormat::PX_FMT_RGBA8));
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u8(bytes, 32);
        for (int index = 0; index < 11; ++index)
        {
            append_u8(bytes, 0);
        }
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiPalette>::invalid_ref);
        append_u32(bytes, 0);
        append_u32(bytes, 4);
        append_u32(bytes, 3);
        append_u8(bytes, 10);
        append_u8(bytes, 20);
        append_u8(bytes, 30);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiPixelData data(
            niflib::version_value(niflib::eNifVersion::VER_10_2_0_0), reader);
        check(data.num_pixels == 3 && data.num_faces == 1 && data.pixel_data.size() == 1,
              "pixel data dimensions mismatch");
        check(data.pixel_data[0][0] == 10 && data.pixel_data[0][2] == 30,
              "pixel data payload mismatch");
    }

    void test_skin_instance_refs()
    {
        std::string bytes;
        append_u32(bytes, 10);
        append_u32(bytes, 11);
        append_u32(bytes, 12);
        append_u32(bytes, 2);
        append_u32(bytes, 13);
        append_u32(bytes, 14);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiSkinInstance instance(
            niflib::version_value(niflib::eNifVersion::VER_10_2_0_0), reader);
        check(instance.data.ref_id() == 10 && instance.partition.ref_id() == 11 &&
                  instance.skeleton_root.ref_id() == 12,
              "skin instance references mismatch");
        check(instance.bones.size() == 2 && instance.bones[1].ref_id() == 14,
              "skin instance bone references mismatch");
    }

    void test_empty_skin_partition()
    {
        std::string bytes;
        append_u32(bytes, 1);
        for (int index = 0; index < 5; ++index)
        {
            append_u16(bytes, 0);
        }
        append_u8(bytes, 0);
        append_u8(bytes, 0);
        append_u16(bytes, 0);
        append_u8(bytes, 0);
        append_u8(bytes, 0);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiSkinPartition partition(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), 0, reader);
        check(partition.partitions.size() == 1 && partition.partitions[0].bones.empty() &&
                  !partition.partitions[0].has_faces,
              "empty skin partition mismatch");
    }

    void test_morph_data()
    {
        std::string bytes;
        append_u32(bytes, 1);
        append_u32(bytes, 2);
        append_u8(bytes, 1);
        append_u32(bytes, 0);
        append_f32(bytes, 0.1f);
        append_f32(bytes, 0.2f);
        append_f32(bytes, 0.3f);
        append_f32(bytes, 0.4f);
        append_f32(bytes, 0.5f);
        append_f32(bytes, 0.6f);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiMorphData data(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(data.num_morphs == 1 && data.num_vertices == 2 && data.relative_targets == 1,
              "morph data header mismatch");
        check(data.morphs[0].vectors.size() == 2 && data.morphs[0].vectors[1].z == 0.6f,
              "morph vertex vectors mismatch");
    }

    void test_gravity_modifier()
    {
        std::string bytes;
        append_u32(bytes, 0xFFFFFFFF);
        append_u32(bytes, 0xFFFFFFFF);
        append_f32(bytes, 0.5f);
        append_f32(bytes, 9.0f);
        append_u32(bytes, 2);
        append_f32(bytes, 1.0f);
        append_f32(bytes, 2.0f);
        append_f32(bytes, 3.0f);
        append_f32(bytes, 0.0f);
        append_f32(bytes, 1.0f);
        append_f32(bytes, 0.0f);
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiGravity gravity(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(gravity.force == 9.0f && gravity.type == 2 && gravity.position.y == 2.0f,
              "gravity payload mismatch");
        check(!gravity.next.is_valid() && !gravity.controller.is_valid(), "gravity references mismatch");
    }

    void test_particle_grow_fade()
    {
        std::string bytes;
        append_u32(bytes, 0xFFFFFFFF);
        append_u32(bytes, 0xFFFFFFFF);
        append_f32(bytes, 2.0f);
        append_f32(bytes, 0.5f);
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiParticleGrowFade modifier(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(modifier.grow == 2.0f && modifier.fade == 0.5f,
              "particle grow/fade fields mismatch");
    }

    void test_particle_bomb()
    {
        std::string bytes;
        append_u32(bytes, 0xFFFFFFFF);
        append_u32(bytes, 0xFFFFFFFF);
        append_f32(bytes, 1.0f);
        append_f32(bytes, 2.0f);
        append_f32(bytes, 3.0f);
        append_f32(bytes, 4.0f);
        append_u32(bytes, static_cast<std::uint32_t>(niflib::eDecayType::DECAY_LINEAR));
        append_u32(bytes, static_cast<std::uint32_t>(niflib::eSymmetryType::PLANAR_SYMMETRY));
        for (int index = 0; index < 6; ++index)
        {
            append_f32(bytes, static_cast<float>(index + 5));
        }
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiParticleBomb bomb(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(bomb.decay == 1.0f && bomb.start == 4.0f &&
                  bomb.symmetry_type == niflib::eSymmetryType::PLANAR_SYMMETRY &&
                  bomb.direction.z == 10.0f,
              "particle bomb fields mismatch");
    }

    void test_nif_string()
    {
        std::string bytes;
        append_u32(bytes, 3);
        bytes += "nif";
        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiString value(reader);
        check(value.value == "nif", "NiString value mismatch");
    }

    void test_empty_texturing_property()
    {
        std::string bytes;
        append_u32(bytes, 0);
        append_u32(bytes, 0);
        append_u32(bytes, niflib::NiRef<niflib::NiTimeController>::invalid_ref);
        append_u32(bytes, 2);
        append_u32(bytes, 0);
        for (int slot = 0; slot < 7; ++slot)
        {
            append_u8(bytes, 0);
        }
        append_u32(bytes, 0);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::NiTexturingProperty property(
            niflib::version_value(niflib::eNifVersion::VER_10_1_0_0), reader);
        check(property.apply_mode == 2 && property.texture_count == 0 && property.num_shader_textures == 0,
              "empty texturing property metadata mismatch");
        check(!property.base_texture && !property.bump_map_texture, "empty texture slots mismatch");
    }

    void test_texture_render_data_10_2()
    {
        std::string bytes;
        append_u32(bytes, static_cast<std::uint32_t>(niflib::ePixelFormat::PX_FMT_DXT1));
        append_u32(bytes, 1);
        append_u32(bytes, 2);
        append_u32(bytes, 3);
        append_u32(bytes, 4);
        append_u8(bytes, 32);
        for (std::uint8_t value = 5; value < 8; ++value)
        {
            append_u8(bytes, value);
        }
        for (std::uint8_t value = 8; value < 16; ++value)
        {
            append_u8(bytes, value);
        }
        append_u32(bytes, 16);
        append_u32(bytes, 2);
        append_u32(bytes, 1);
        append_u32(bytes, 4);
        append_u32(bytes, 64);
        append_u32(bytes, 32);
        append_u32(bytes, 20);

        std::istringstream input(bytes);
        niflib::NifBinaryReader reader(input);
        const niflib::ATextureRenderData data(
            niflib::version_value(niflib::eNifVersion::VER_10_2_0_0), reader);
        check(data.pixel_format == niflib::ePixelFormat::PX_FMT_DXT1, "texture pixel format mismatch");
        check(data.red_mask == 1 && data.alpha_mask == 4 && data.bits_per_pixel == 32, "texture masks mismatch");
        check(data.unknown_3_bytes[2] == 7 && data.unknown_8_bytes[7] == 15, "texture reserved bytes mismatch");
        check(data.unknown_int == 16 && data.palette.ref_id() == 2, "texture metadata mismatch");
        check(data.mip_maps.size() == 1 && data.mip_maps[0].width == 64 && data.mip_maps[0].offset == 20,
              "texture mip-map mismatch");
    }
}

int main()
{
    try
    {
        test_header_block_table();
        test_linear_byte_key();
        test_quadratic_color_key();
        test_float_key_group();
        test_legacy_lod_range();
        test_minimal_node();
        test_triangle_geometry_data();
        test_minimal_triangle_shape();
        test_complete_node_file();
        test_child_reference_resolution();
        test_billboard_node();
        test_camera_frustum();
        test_triangle_strip_data();
        test_named_integer_extra_data();
        test_integer_array_extra_data();
        test_binary_extra_data();
        test_text_key_extra_data();
        test_chained_string_extra_data();
        test_material_property();
        test_alpha_controller();
        test_vector_and_visibility_tracks();
        test_keyframe_track_order();
        test_point3_controller();
        test_texture_transform_controller();
        test_dynamic_effect();
        test_particle_record();
        test_external_source_texture();
        test_palette_bytes();
        test_pixel_data_payload();
        test_skin_instance_refs();
        test_empty_skin_partition();
        test_morph_data();
        test_gravity_modifier();
        test_particle_grow_fade();
        test_particle_bomb();
        test_nif_string();
        test_empty_texturing_property();
        test_texture_render_data_10_2();
    }
    catch (const std::exception &error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}