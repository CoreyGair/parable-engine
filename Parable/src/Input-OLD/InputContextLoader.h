#pragma once

#include "rapidjson/fwd.h"

namespace Parable::Input
{


class InputContext;
class Control;
template<class T> class Axis;

class InputContextLoader
{
public:
    InputContextLoader(std::string& file) : m_file(file) {}

    std::unique_ptr<InputContext> load_context();

private:
    std::string m_file;

    std::unique_ptr<Control> parse_control(const rapidjson::Value& val);
    std::unique_ptr<Axis<double>> parse_axis_double(const rapidjson::Value& val);
    std::unique_ptr<Axis<int>> parse_axis_int(const rapidjson::Value& val);
};


}