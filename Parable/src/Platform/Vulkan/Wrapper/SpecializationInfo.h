#pragma once

#include "pblpch.h"

#include <vulkan/vulkan.hpp>

#include "Util/Buffer.h"

namespace Parable::Vulkan
{


/**
 * Convenient builder class for vk::SpecializationInfo structs.
 */
class SpecializationInfoBuilder
{
public:
    /**
     * Builds the SpecializationInfo struct.
     * 
     * The returned buffer must be kept alive until the SpecializationInfo struct is no longer needed.
     * 
     * @return std::pair<vk::SpecializationInfo,Parable::Util::Buffer> The filled SpecializationInfo struct,
     * as well as an RAII buffer containing the specialization values.
     */
    std::pair<vk::SpecializationInfo,Parable::Util::Buffer> build()
    {
        std::vector<vk::SpecializationMapEntry> map_entries;
        map_entries.reserve(m_specialisation_entries.size());

        uint32_t data_size = 0;

        // traverse the entries, populating the map entries vector and calculating total size of all values
        for (const auto& entry : m_specialisation_entries)
        {
            map_entries.push_back(vk::SpecializationMapEntry(
                entry.get_constant_id(),
                data_size,
                entry.get_size()
            ));

            data_size += entry.get_size();
        }

        Parable::Util::Buffer data_buffer(data_size);

        size_t curr_offset = 0;

        // traverse entries again, copying values into the buffer
        for (const auto& entry : m_specialisation_entries)
        {
            entry.copy_value(data_buffer.get_offset(curr_offset));
            curr_offset + entry.get_size();
        }

        return {{map_entries.size(), map_entries.data(), data_size, data_buffer.get()},std::move(data_buffer)};
    }

private:
    class SpecializationEntry
    {
    public:
        virtual ~SpecializationEntry() = 0;

        virtual void copy_value(void* target) const = 0;

        virtual size_t get_size() const = 0;

        uint32_t get_constant_id() const { return m_constant_id; }

    private:
        uint32_t m_constant_id;
    };

    template<typename T>
    class TypedSpecializationEntry : SpecializationEntry
    {
    public:
        TypedSpecializationEntry(uint32_t constant_id, T value) : m_constant_id(constant_id), m_value(value) {}

        void copy_value(void* dest) const
        {
            std::memcpy(dest, &m_value, get_size());
        }

        size_t get_size() const override { return sizeof(T); }

    private:
        T m_value;
    };

    std::vector<std::unique_ptr<SpecializationEntry>> m_specialisation_entries;
};


}