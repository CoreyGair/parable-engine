
// TEMP : remove when implementing new filesystem stuff
#include <fstream>

#include "Input/InputContextLoader.h"

#include "Input/Control.h"

#include "Input/InputContext.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

namespace Parable::Input
{


std::unique_ptr<InputContext> InputContextLoader::load_context()
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

    PBL_ASSERT_MSG(document["context"].HasMember("name"), "Missing name for the context while parsing {} for input context.", m_file)

    PBL_ASSERT_MSG(document["context"].HasMember("controls") || document["context"]["controls"].IsArray(), "JSON does not contain a list of controls while parsing {} for input context.", m_file)

    const rapidjson::Value& controls = document["context"]["controls"]; 

    std::vector<std::unique_ptr<Control>> loaded_controls;

    for (auto& v : controls.GetArray())
    {
        PBL_ASSERT_MSG(v.HasMember("control_type"), "Control object missing control type while parsing {} for input context.", m_file)
        PBL_ASSERT_MSG(v.HasMember("name"), "Control object missing name while parsing {} for input context.", m_file)
        
        loaded_controls.push_back(std::move(parse_control(v)));
    }
    
    std::string context_name (document["context"]["name"].GetString());
    return std::make_unique<InputContext>(context_name, loaded_controls);
}


//
//  TODO: GET RID OF ALL THE MACRO REPETITION HERE
//
std::unique_ptr<Control> InputContextLoader::parse_control(const rapidjson::Value& val)
{
    std::string control_name (val["name"].GetString());
    switch (val["control_type"].GetUint())
    {
    case ControlType::Button:
    {
        PBL_CORE_ASSERT_MSG(val.HasMember("inputs"), "Button control {} missing inputs list while parsing {} for input context.", control_name, m_file)

        std::vector<InputCode> inputs;
        for(auto& i : val["inputs"].GetArray()) inputs.push_back(i.GetUint());
        return std::make_unique<ButtonControl>(control_name, inputs);
        break;
    }

    case ControlType::Axis1D:
    {
        PBL_CORE_ASSERT_MSG(val.HasMember("axis_x") && val["axis_x"].IsObject(), "Axis1D control {} missing x axis while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(val["axis_x"].HasMember("axis_id"), "Axis1D control {}: x axis missing axis_id while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(IS_AXIS_DOUBLE(val["axis_x"]["axis_id"].GetUint()), "Axis1D control {}: x axis is not a 'double' axis while parsing {} for input context.", control_name, m_file)

        std::unique_ptr<Axis<double>> x_axis = parse_axis_double(val["axis_x"]);

        return std::make_unique<Axis1DControl>(control_name, x_axis);

        break;
    }

    case ControlType::Axis1DInt:
    {
        PBL_CORE_ASSERT_MSG(val.HasMember("axis_x") && val["axis_x"].IsObject(), "Axis1DInt control {} missing x axis while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(val["axis_x"].HasMember("axis_id"), "Axis1DInt control {}: x axis missing axis_id while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(IS_AXIS_INT(val["axis_x"]["axis_id"].GetUint()), "Axis1DInt control {}: x axis is not an 'int' axis while parsing {} for input context.", control_name, m_file)
    
        std::unique_ptr<Axis<int>> x_axis = parse_axis_int(val["axis_x"]);

        return std::make_unique<Axis1DIntControl>(control_name, x_axis);

        break;
    }

    case ControlType::Axis2D:
    {
        PBL_CORE_ASSERT_MSG(val.HasMember("axis_x") && val["axis_x"].IsObject(), "Axis1D control {} missing x axis while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(val["axis_x"].HasMember("axis_id"), "Axis1D control {}: x axis missing axis_id while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(IS_AXIS_DOUBLE(val["axis_x"]["axis_id"].GetUint()), "Axis1D control {}: x axis is not a 'double' axis while parsing {} for input context.", control_name, m_file)

        PBL_CORE_ASSERT_MSG(val.HasMember("axis_y") && val["axis_y"].IsObject(), "Axis2D control {} missing y axis while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(val["axis_y"].HasMember("axis_id"), "Axis2D control {}: y axis missing axis_id while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(IS_AXIS_DOUBLE(val["axis_y"]["axis_id"].GetUint()), "Axis2D control {}: y axis is not a 'double' axis while parsing {} for input context.", control_name, m_file)

        std::unique_ptr<Axis<double>> x_axis = parse_axis_double(val["axis_x"]);
        std::unique_ptr<Axis<double>> y_axis = parse_axis_double(val["axis_y"]);

        return std::make_unique<Axis2DControl>(control_name, x_axis, y_axis);

        break;
    }

    case ControlType::Axis2DInt:
    {
        PBL_CORE_ASSERT_MSG(val.HasMember("axis_x") && val["axis_x"].IsObject(), "Axis2DInt control {} missing x axis while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(val["axis_x"].HasMember("axis_id"), "Axis2DInt control {}: x axis missing axis_id while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(IS_AXIS_INT(val["axis_x"]["axis_id"].GetUint()), "Axis2DInt control {}: x axis is not an 'int' axis while parsing {} for input context.", control_name, m_file)

        PBL_CORE_ASSERT_MSG(val.HasMember("axis_y") && val["axis_y"].IsObject(), "Axis2DInt control {} missing y axis while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(val["axis_y"].HasMember("axis_id"), "Axis2DInt control {}: y axis missing axis_id while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(IS_AXIS_INT(val["axis_y"]["axis_id"].GetUint()), "Axis2DInt control {}: y axis is not a 'int' axis while parsing {} for input context.", control_name, m_file)

        std::unique_ptr<Axis<int>> x_axis = parse_axis_int(val["axis_x"]);
        std::unique_ptr<Axis<int>> y_axis = parse_axis_int(val["axis_y"]);

        return std::make_unique<Axis2DIntControl>(control_name, x_axis, y_axis);

        break;
    }

    case ControlType::Axis3D:
    {
        PBL_CORE_ASSERT_MSG(val.HasMember("axis_x") && val["axis_x"].IsObject(), "Axis3D control {} missing x axis while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(val["axis_x"].HasMember("axis_id"), "Axis3D control {}: x axis missing axis_id while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(IS_AXIS_DOUBLE(val["axis_x"]["axis_id"].GetUint()), "Axis3D control {}: x axis is not a 'double' axis while parsing {} for input context.", control_name, m_file)

        PBL_CORE_ASSERT_MSG(val.HasMember("axis_y") && val["axis_y"].IsObject(), "Axis3D control {} missing y axis while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(val["axis_y"].HasMember("axis_id"), "Axis3D control {}: y axis missing axis_id while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(IS_AXIS_DOUBLE(val["axis_y"]["axis_id"].GetUint()), "Axis2D control {}: y axis is not a 'double' axis while parsing {} for input context.", control_name, m_file)

        PBL_CORE_ASSERT_MSG(val.HasMember("axis_z") && val["axis_z"].IsObject(), "Axis3D control {} missing z axis while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(val["axis_z"].HasMember("axis_id"), "Axis3D control {}: z axis missing axis_id while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(IS_AXIS_DOUBLE(val["axis_z"]["axis_id"].GetUint()), "Axis3D control {}: z axis is not a 'double' axis while parsing {} for input context.", control_name, m_file)

        std::unique_ptr<Axis<double>> x_axis = parse_axis_double(val["axis_x"]);
        std::unique_ptr<Axis<double>> y_axis = parse_axis_double(val["axis_y"]);
        std::unique_ptr<Axis<double>> z_axis = parse_axis_double(val["axis_z"]);

        return std::make_unique<Axis3DControl>(control_name, x_axis, y_axis, z_axis);

        break;
    }

    case ControlType::Axis3DInt:
    {
        PBL_CORE_ASSERT_MSG(val.HasMember("axis_x") && val["axis_x"].IsObject(), "Axis3DInt control {} missing x axis while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(val["axis_x"].HasMember("axis_id"), "Axis3DInt control {}: x axis missing axis_id while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(IS_AXIS_INT(val["axis_x"]["axis_id"].GetUint()), "Axis3DInt control {}: x axis is not an 'int' axis while parsing {} for input context.", control_name, m_file)

        PBL_CORE_ASSERT_MSG(val.HasMember("axis_y") && val["axis_y"].IsObject(), "Axis3DInt control {} missing y axis while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(val["axis_y"].HasMember("axis_id"), "Axis3DInt control {}: y axis missing axis_id while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(IS_AXIS_INT(val["axis_y"]["axis_id"].GetUint()), "Axis3DInt control {}: y axis is not a 'int' axis while parsing {} for input context.", control_name, m_file)

        PBL_CORE_ASSERT_MSG(val.HasMember("axis_z") && val["axis_z"].IsObject(), "Axis3DInt control {} missing z axis while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(val["axis_z"].HasMember("axis_id"), "Axis3DInt control {}: z axis missing axis_id while parsing {} for input context.", control_name, m_file)
        PBL_CORE_ASSERT_MSG(IS_AXIS_INT(val["axis_z"]["axis_id"].GetUint()), "Axis3DInt control {}: z axis is not a 'int' axis while parsing {} for input context.", control_name, m_file)

        std::unique_ptr<Axis<int>> x_axis = parse_axis_int(val["axis_x"]);
        std::unique_ptr<Axis<int>> y_axis = parse_axis_int(val["axis_y"]);
        std::unique_ptr<Axis<int>> z_axis = parse_axis_int(val["axis_z"]);

        return std::make_unique<Axis3DIntControl>(control_name, x_axis, y_axis, z_axis);

        break;
    }
    
    default:
        PBL_ASSERT_MSG(false, "Unrecognised ControlType while parsing {} for input context.", m_file)
        break;
    }
    return nullptr;
}

std::unique_ptr<Axis<double>> InputContextLoader::parse_axis_double(const rapidjson::Value& val)
{
    switch (val["axis_id"].GetUint())
    {
    case AxisID::MouseX:
        return std::make_unique<MouseXAxis>();
        break;

    case AxisID::MouseY:
        return std::make_unique<MouseYAxis>();
        break;

    case AxisID::MouseScroll:
        return std::make_unique<MouseScrollAxis>();
        break;

    default:
        PBL_ASSERT_MSG(false, "Unrecognised ControlType while parsing {} for input context.", m_file)
        break;
    }
    return nullptr;
}

std::unique_ptr<Axis<int>> InputContextLoader::parse_axis_int(const rapidjson::Value& val)
{
    switch (val["axis_id"].GetUint())
    {
    case AxisID::Button:
    {
        PBL_CORE_ASSERT_MSG(val.HasMember("positive_inputs") && val["positive_inputs"].IsArray(), "Button axis missing positive inputs list while parsing {} for input context.", m_file)
        PBL_CORE_ASSERT_MSG(val.HasMember("negative_inputs") && val["negative_inputs"].IsArray(), "Button axis missing negative inputs list while parsing {} for input context.", m_file)

        std::vector<InputCode> positive_inputs;
        std::vector<InputCode> negative_inputs;

        for(auto& i : val["positive_inputs"].GetArray()) positive_inputs.push_back(i.GetUint());
        for(auto& i : val["negative_inputs"].GetArray()) negative_inputs.push_back(i.GetUint());

        return std::make_unique<ButtonAxis>(positive_inputs, negative_inputs);
        break;
    }

    default:
    
        PBL_ASSERT_MSG(false, "Unrecognised ControlType while parsing {} for input context.", m_file)
        break;
    }
    return nullptr;
}


}