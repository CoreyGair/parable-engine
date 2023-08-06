#include "EffectLoadInfo.h"

#include "pblpch.h"

#include <rapidjson/document.h>

#include "Renderer/Effect.h"

namespace Parable
{


/**
 * Converts a string to the ShaderStage enum value.
 */
ShaderStage shader_stage_from_string(std::string_view str)
{
    if (str == "vertex") return ShaderStage::Vertex;
    if (str == "fragment") return ShaderStage::Fragment;
    if (str == "compute") return ShaderStage::Compute;
    throw std::exception("ShaderStage not a valid value.");
}

EffectLoadInfo::EffectPassInfo::EffectPassInfo(const rapidjson::Value& source)
{
    if (!source.IsObject())
    {
        throw std::exception("EffectPassInfo source is not an object.");
    }

    auto json_info = source.GetObject();

    if (!json_info.HasMember("shader_stages") || !json_info["shader_stages"].IsArray())
    {
        throw std::exception("EffectPassInfo missing shader_stages array");
    }

    // extract ShaderStageInfos into m_shader_stages
    {
        // the json array of ShaderStageInfo's
        auto json_shader_stages = json_info["shader_stages"].GetArray();

        m_shader_stages.reserve(json_shader_stages.Size());

        for (rapidjson::SizeType i = 0; i < json_shader_stages.Size(); ++i)
        {
            if (!json_shader_stages[i].IsObject())
            {
                throw std::exception("EffectPassInfo shader_stages contains non-object item.");
            }

            auto json_shader_stage = json_shader_stages[i].GetObject();

            // ShaderStageInfo.shader - descriptor 
            auto shader = json_shader_stage.FindMember("shader");
            if (shader == json_shader_stage.MemberEnd() || !shader->value.IsUint64())
            {
                throw std::exception("ShaderStageInfo missing uint64 shader member.");
            }

            // ShaderStageInfo.stage - ShaderStage enum
            auto stage = json_shader_stage.FindMember("stage");
            if (shader == json_shader_stage.MemberEnd() || !shader->value.IsString())
            {
                throw std::exception("ShaderStageInfo missing string stage member.");
            }

            m_shader_stages.emplace_back(shader->value.GetUint64(),shader_stage_from_string(shader->value.GetString()));
        }
    }
}

/**
 * Builds the list of EffectPassInfo objects from the relevant json document.
 * 
 * @param source A document containing the list of JSON objects describing EffectPassInfo objects.
 * @return std::vector<EffectLoadInfo::EffectPassInfo> The extracted list of EffectPassInfo objects.
 */
std::vector<EffectLoadInfo::EffectPassInfo> extract_effect_passes(const rapidjson::GenericArray<1,rapidjson::Value>& source_array)
{
    std::vector<EffectLoadInfo::EffectPassInfo> passes;
    passes.reserve(source_array.Size());

    for (rapidjson::SizeType i = 0; i < source_array.Size(); i++)
    {
        if (!source_array[i].IsObject())
        {
            throw std::exception("EffectPassInfo array contains a non-object item.");
        }

        passes.emplace_back(source_array[i].GetObject());
    }

    return passes;
}

/**
 * Converts a string to the EffectParameterType enum value.
 */
EffectParameterType parameter_type_from_string(std::string_view str)
{
    if (str == "texture2d") return EffectParameterType::Texture2D;
    if (str == "float") return EffectParameterType::Float;
    if (str == "vec2") return EffectParameterType::Vec2;
    if (str == "vec3") return EffectParameterType::Vec3;
    if (str == "vec4") return EffectParameterType::Vec4;
    throw std::exception("EffectParameterType not a valid value.");
}

/**
 * Builds the list of ParameterBufferObjectBinding structs from the relevant json document.
 * 
 * @param source A document containing the list of JSON objects describing ParameterBufferObjectBinding objects.
 * @return std::vector<EffectLoadInfo::ParameterBufferObjectBinding> The extracted list of ParameterBufferObjectBinding objects.
 */
std::vector<EffectLoadInfo::ParameterBufferObjectBinding> extract_parameter_bindings(const rapidjson::GenericArray<1,rapidjson::Value>& source_array)
{
    std::vector<EffectLoadInfo::ParameterBufferObjectBinding> bindings;
    bindings.reserve(source_array.Size());

    for (rapidjson::SizeType i = 0; i < source_array.Size(); i++)
    {
        if (!source_array[i].IsObject())
        {
            throw std::exception("ParameterBufferObjectBinding array contains a non-object item.");
        }

        auto json_binding = source_array[i].GetObject();

        // ParameterBufferObjectBinding.type - str (EffectParameterType enum)
        auto type = json_binding.FindMember("type");
        if (type == json_binding.MemberEnd() || !type->value.IsUint())
        {
            throw std::exception("ParameterBufferObjectBinding missing string member type.");
        }

        // ParameterBufferObjectBinding.offset - uint
        auto offset = json_binding.FindMember("offset");
        if (offset == json_binding.MemberEnd() || !offset->value.IsUint())
        {
            throw std::exception("ParameterBufferObjectBinding missing uint member offset.");
        }

        bindings.emplace_back(parameter_type_from_string(type->value.GetString()),offset->value.GetUint());
    }

    return bindings;
}

EffectLoadInfo::EffectLoadInfo(const rapidjson::Value& source)
{
    if (!source.IsObject())
    {
        throw std::exception("EffectLoadInfo source is not an object.");
    }

    auto json_info = source.GetObject();

    auto json_passes = json_info.FindMember("passes");
    if (json_passes == json_info.MemberEnd() || !json_passes->value.IsArray())
    {
        throw std::exception("EffectLoadInfo missing passes array.");
    }

    m_effect_passes = extract_effect_passes(json_passes->value.GetArray());

    auto json_bindings = json_info.FindMember("parameter_uniform_bindings");
    if (json_bindings == json_info.MemberEnd() || !json_bindings->value.IsArray())
    {
        throw std::exception("EffectLoadInfo missing parameter_uniform_bindings array.");
    }

    m_parameter_bindings = extract_parameter_bindings(json_bindings->value.GetArray());
}


}