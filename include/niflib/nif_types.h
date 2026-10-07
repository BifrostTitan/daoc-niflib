// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include <array>
#include <cstdint>

namespace niflib
{
    struct Vec3
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    struct Vec4
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;
    };

    struct Vec2
    {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct Color4
    {
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        float a = 0.0f;
    };

    enum class eAlphaFormat : std::uint32_t
    {
        ALPHA_NONE,
        ALPHA_BINARY,
        ALPHA_SMOOTH,
        ALPHA_DEFAULT
    };

    enum class eBillboardMode : std::uint16_t
    {
        ALWAYS_FACE_CAMERA,
        ROTATE_ABOUT_UP,
        RIGID_FACE_CAMERA,
        ALWAYS_FACE_CENTER,
        RIGID_FACE_CENTER,
        ROTATE_ABOUT_UP2 = 9
    };

    enum class eCoordGenType : std::uint32_t
    {
        CG_WORLD_PARALLEL,
        CG_WORLD_PERSPECTIVE,
        CG_SPHERE_MAP,
        CG_SPECULAR_CUBE_MAP,
        CG_DIFFUSE_CUBE_MAP
    };

    enum class eDecayType : std::uint32_t
    {
        DECAY_NONE,
        DECAY_LINEAR,
        DECAY_EXPONENTIAL
    };

    enum class eEffectType : std::uint32_t
    {
        EFFECT_PROJECTED_LIGHT,
        EFFECT_PROJECTED_SHADOW,
        EFFECT_ENVIRONMENT_MAP,
        EFFECT_FOG_MAP
    };

    enum class eFaceDrawMode : std::uint32_t
    {
        DRAW_CCW_OR_BOTH,
        DRAW_CCW,
        DRAW_CW,
        DRAW_BOTH
    };

    enum class ePixelFormat : std::uint32_t
    {
        PX_FMT_RGB8,
        PX_FMT_RGBA8,
        PX_FMT_PAL8,
        PX_FMT_DXT1 = 4,
        PX_FMT_DXT5,
        PX_FMT_DXT5_ALT
    };

    enum class ePixelLayout : std::uint32_t
    {
        PIX_LAY_PALETTISED,
        PIX_LAY_HIGH_COLOR_16,
        PIX_LAY_TRUE_COLOR_32,
        PIX_LAY_COMPRESSED,
        PIX_LAY_BUMPMAP,
        PIX_LAY_PALETTISED_4,
        PIX_LAY_DEFAULT
    };

    enum class eMipMapFormat : std::uint32_t
    {
        MIP_FMT_NO,
        MIP_FMT_YES,
        MIP_FMT_DEFAULT
    };

    enum class eKeyType : std::uint32_t
    {
        LINEAR_KEY = 1,
        QUADRATIC_KEY,
        TBC_KEY,
        XYZ_ROTATION_KEY,
        CONST_KEY
    };

    enum class eChannelType : std::uint32_t
    {
        CHNL_RED,
        CHNL_GREEN,
        CHNL_BLUE,
        CHNL_ALPHA,
        CHNL_COMPRESSED,
        CHNL_INDEX = 16,
        CHNL_EMPTY = 19
    };

    enum class eChannelConvention : std::uint32_t
    {
        CC_FIXED,
        CC_INDEX = 3,
        CC_COMPRESSED,
        CC_EMPTY
    };

    enum class eNifVersion : std::uint32_t
    {
        VER_2_3 = 0x02030000,
        VER_3_0 = 0x03000000,
        VER_3_03 = 0x03000300,
        VER_3_1 = 0x03010000,
        VER_3_3_0_13 = 0x0303000D,
        VER_4_0_0_0 = 0x04000000,
        VER_4_0_0_2 = 0x04000002,
        VER_4_1_0_1 = 0x04010001,
        VER_4_1_0_12 = 0x0401000C,
        VER_4_2_0_2 = 0x04020002,
        VER_4_2_1_0 = 0x04020100,
        VER_4_2_2_0 = 0x04020200,
        VER_5_0_0_1 = 0x05000001,
        VER_10_0_1_0 = 0x0A000100,
        VER_10_0_1_2 = 0x0A000102,
        VER_10_0_1_3 = 0x0A000103,
        VER_10_1_0_0 = 0x0A010000,
        VER_10_1_0_101 = 0x0A010065,
        VER_10_1_0_106 = 0x0A01006A,
        VER_10_2_0_0 = 0x0A020000,
        VER_10_4_0_1 = 0x0A040001,
        VER_20_0_0_4 = 0x14000004,
        VER_20_0_0_5 = 0x14000005,
        VER_20_1_0_3 = 0x14010003,
        VER_20_2_0_7 = 0x14020007,
        VER_20_2_0_8 = 0x14020008,
        VER_20_3_0_1 = 0x14030001,
        VER_20_3_0_2 = 0x14030002,
        VER_20_3_0_3 = 0x14030003,
        VER_20_3_0_6 = 0x14030006,
        VER_20_3_0_9 = 0x14030009,
        VER_UNSUPPORTED = 0xFFFFFFFF,
        VER_INVALID = 0xFFFFFFFE
    };

    enum class eStencilAction : std::uint32_t
    {
        ACTION_KEEP,
        ACTION_ZERO,
        ACTION_REPLACE,
        ACTION_INCREMENT,
        ACTION_DECREMENT,
        ACTION_INVERT
    };

    enum class eStencilCompareMode : std::uint32_t
    {
        TEST_NEVER,
        TEST_LESS,
        TEST_EQUAL,
        TEST_LESS_EQUAL,
        TEST_GREATER,
        TEST_NOT_EQUAL,
        TEST_GREATER_EQUAL,
        TEST_ALWAYS
    };

    enum class eSymmetryType : std::uint32_t
    {
        SPHERICAL_SYMMETRY,
        CYLINDRICAL_SYMMETRY,
        PLANAR_SYMMETRY
    };

    enum class eTargetColor : std::uint16_t
    {
        TC_AMBIENT,
        TC_DIFFUSE,
        TC_SPECULAR,
        TC_SELF_ILLUM
    };

    enum class eTexClampMode : std::uint32_t
    {
        CLAMP_S_CLAMP_T,
        CLAMP_S_WRAP_T,
        WRAP_S_CLAMP_T,
        WRAP_S_WRAP_T
    };

    enum class eTexFilterMode : std::uint32_t
    {
        FILTER_NEAREST,
        FILTER_BILERP,
        FILTER_TRILERP,
        FILTER_NEAREST_MIPNEAREST,
        FILTER_NEAREST_MIPLERP,
        FILTER_BILERP_MIPNEAREST
    };

    enum class eTexTransform : std::uint32_t
    {
        TT_TRANSLATE_U,
        TT_TRANSLATE_V,
        TT_ROTATE,
        TT_SCALE_U,
        TT_SCALE_V
    };

    enum class eTexType : std::uint32_t
    {
        BASE_MAP,
        DARK_MAP,
        DETAIL_MAP,
        GLOSS_MAP,
        GLOW_MAP,
        BUMP_MAP,
        NORMAL_MAP,
        UNKNOWN2_MAP,
        DECAL_0_MAP,
        DECAL_1_MAP,
        DECAL_2_MAP,
        DECAL_3_MAP
    };

    constexpr std::uint32_t version_value(eNifVersion version) noexcept
    {
        return static_cast<std::uint32_t>(version);
    }
}