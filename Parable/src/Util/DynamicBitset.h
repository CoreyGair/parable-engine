#pragma once

namespace Parable
{
    class Allocator;
}

namespace Parable::Util
{

using Segment = uint8_t;

/** 
 *  Dynamic size implementation of std::bitset.
 *
 *  Holds a set of boolean values in an array of 8 bit allocations.
 * 
 *  Implements most of the functions from std::bitset, but size is set in ctor instead of by template.
 */
class DynamicBitset
{
public:
    DynamicBitset(size_t size, Allocator& allocator);
    ~DynamicBitset();

    // element access

    bool at(size_t bit);
    bool operator[](size_t bit);

    bool all() const;
    bool any() const;
    bool none() const;
    size_t count() const;

    /**
     * Return the number of boolean values stored
     */
    size_t get_size() const { return m_size; }

    // comparison

    bool operator==(const DynamicBitset& rhs) const;
    bool operator!=(const DynamicBitset& rhs) const;

    // modifiers

    DynamicBitset& operator&=(const DynamicBitset& other);
    DynamicBitset& operator|=(const DynamicBitset& other);
    DynamicBitset& operator^=(const DynamicBitset& other);
    void flip_all();
    void flip(size_t bit);

    void set_all();
    void set(size_t bit);

    void reset_all();
    void reset(size_t bit);

    // debug

    std::string to_string();

private:
    static const size_t segment_bitwidth = sizeof(Segment) * 8;

    size_t m_size;

    size_t m_num_segments;
    Segment* m_segments;

    size_t m_last_segment_bits;

    Allocator& m_allocator;

};


}