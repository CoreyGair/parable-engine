#pragma once

namespace Parable
{


/**
 * Describes where in the pipeline a shader is to be executed.
 */
enum class ShaderStage
{
    Vertex,
    Fragment,
    Compute
};

/**
 * Describes the type of effect pass, and therefore where it should be used in rendering.
 */
enum class EffectPassType
{
    Opaque,
    Shadow,
    Transparent
};

/**
 * Describes the type of an effect parameter.
 */
enum class EffectParameterType
{
    Texture2D,
    Float,
    Vec2,
    Vec3,
    Vec4
};

/**
 * Engine interface for efects, which describe how parameters provided by materials are used to render meshes.
 */
class Effect
{
public:
    virtual ~Effect() = 0;
};


}




// struct Material
// {
//     Effect& effect;
//     EffectParams params;
// }
// struct Effect
// {
//     vector<EffectPass> passes; // per-pass-type passes e.g. opaque, shadow, transparent, etc
//     EffectParameterUniformLayout param_layout; // describes how params are packed into UBO
// }
// struct EffectParameterUniformLayout
// {
//     struct ParameterBinding
//     {
//         uint index;
//         size_t offset;
//     }

//     vector<ParameterBinding> bindings;

//     void write_ubo_data(void* ubo, EffectParams& params)
//     {
//         for (auto& binding : bindings)
//         {
//             params[binding.index].write_to_ubo(ubo + binding.offset)
//         }
//     }
// }
// struct EffectParams
// {
//     class ParameterValue
//     {
//         virtual ~ParameterValue() = 0;
//         virtual write_to_ubo(void* pos) = 0;
//     }

//     template<class T>
//     class TypedParameterValue : public ParameterValue
//     {
//         T value;
//         ~TypedParameterValue() = default;
//         write_to_ubo(void* pos) override
//         {
//             memcpy(pos, &value);
//         }
//     }

//     vector<unique_ptr<ParameterValue>> values;

//     operator[](int i) { return values[i]; }

//     // could have map<string,size_t> to map param names to index in above
// }