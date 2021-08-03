#pragma once

#include "rapidjson/fwd.h"

namespace Parable::Input
{


class InputContext;
class ButtonMap;


/**
 * Loads InputContexts from a file.s
 * 
 */
class InputContextLoader
{
public:
    /**
     * @brief Construct a new Input Context Loader object
     * 
     * @param file the JSON file from which to read the InputContext data
     */
    InputContextLoader(std::string& file) : m_file(file) {}

    InputContext load_context();

private:
    std::string m_file;

    ButtonMap parse_button_map(const rapidjson::Value& val);
};


}