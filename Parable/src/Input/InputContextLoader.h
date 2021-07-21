#pragma once

#include "rapidjson/fwd.h"

namespace Parable::Input
{


class InputContext;

class InputContextLoader
{
public:
    InputContextLoader(std::string& file) : m_file(file) {}

    std::unique_ptr<InputContext> load_context();

private:
    std::string m_file;

    ButtonMap parse_button_map(const rapidjson::Value& val);
};


}