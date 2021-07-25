
// TEMP : remove when implementing new filesystem stuff
#include <fstream>

#include "Input/InputContextLoader.h"

#include "Input/ButtonMap.h"

#include "Input/InputContext.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

namespace Parable::Input
{

InputContext InputContextLoader::load_context()
{
    std::ifstream t(m_file);
    std::stringstream ss;
    ss << t.rdbuf();
    std::string json_string (std::move(ss.str()));
    char* json_chars = const_cast<char*>(json_string.c_str());

    rapidjson::Document document;
    document.ParseInsitu(json_chars);
    PBL_ASSERT_MSG(!document.HasParseError(), "Error '{}' parsing {}!", GetParseError_En(document.GetParseError()), m_file)

    PBL_ASSERT_MSG(document.IsObject(), "JSON root is not an object while parsing {} for input context.", m_file)

    PBL_ASSERT_MSG(document.HasMember("context") && document["context"].IsObject(), "JSON does not describe a context object while parsing {} for input context.", m_file)

    const rapidjson::Value& context = document["context"];

    PBL_ASSERT_MSG(context.HasMember("name"), "Missing name for the context while parsing {} for input context.", m_file)

    PBL_ASSERT_MSG(context.HasMember("button_maps") && context["button_maps"].IsArray(), "JSON does not contain a list of controls while parsing {} for input context.", m_file)

    const rapidjson::Value& maps = context["button_maps"]; 

    std::vector<ButtonMap> loaded_maps;

    for (auto& v : maps.GetArray())
    {   
        loaded_maps.emplace_back(std::move(parse_button_map(v)));
    }
    
    std::string context_name (context["name"].GetString());
    return std::move(InputContext(context_name, loaded_maps));
}

ButtonMap InputContextLoader::parse_button_map(const rapidjson::Value& val)
{
    PBL_CORE_ASSERT_MSG(val.HasMember("name"), "ButtonMap does not contain a name while parsing {} for input context.", m_file)
    std::string map_name (val["name"].GetString());
    
    PBL_CORE_ASSERT_MSG(val.HasMember("inputs") && val["inputs"].IsArray(), "ButtonMap {} does not contain a name while parsing {} for input context.", map_name, m_file);
    std::vector<InputCode> mapped_inputs;
    for(auto& v : val["inputs"].GetArray())
    {
        mapped_inputs.push_back(v.GetUint());
    }

    return std::move(ButtonMap(map_name, mapped_inputs));
}


}