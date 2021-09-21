#pragma once

#include <array>

namespace Parable::Util
{

/** 
 *  Static size implementation of std::bitset.
 *
 *  Holds a set of boolean values in an array of 8 bit allocations.
 * 
 *  Implements most of the functions from std::bitset, size set in template
 * 
 * Invariant: any unused bits in the last segment will always be 0
 * 
 * @tparam size the number of bits to hold in the bitset
 */
template<size_t size>
class StaticBitset
{
    using Segment = unsigned int;

public:
    StaticBitset();
    StaticBitset(const StaticBitset& other) = default;
    ~StaticBitset();

    // element access

    bool at(size_t bit) const;
    bool operator[](size_t bit) const;

    bool all() const;
    bool any() const;
    bool none() const;
    size_t count() const;

    /**
     * Return the number of boolean values stored
     */
    size_t get_size() const { return size; }

    // comparison

    bool operator==(const StaticBitset& rhs) const;
    bool operator!=(const StaticBitset& rhs) const;

    bool is_subset_of(const StaticBitset& other) const;
    bool is_superset_of(const StaticBitset& other) const;

    // modifiers

    StaticBitset& operator&=(const StaticBitset& other);
    StaticBitset& operator|=(const StaticBitset& other);
    StaticBitset& operator^=(const StaticBitset& other);
    void flip_all();
    void flip(size_t bit);

    void set_all();
    void set(size_t bit);

    void reset_all();
    void reset(size_t bit);

    // other

    size_t find_first_set();
    size_t find_first_unset();

    std::vector<size_t> get_set_bits();
    std::vector<size_t> get_unset_bits();

    // debug

    std::string to_string();

private:
    static const size_t segment_bitwidth = sizeof(Segment) * 8;
    static const size_t last_segment_bits = size % segment_bitwidth;
    static const size_t num_segments = size/segment_bitwidth + (last_segment_bits > 0 ? 1 : 0);

    std::array<Segment, num_segments> m_segments;
};


}