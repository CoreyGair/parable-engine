#include "StaticBitset.h"

#include "pblpch.h"

#include "Core/Base.h"

#include "Exception/LogicExceptions.h"


namespace Parable::Util
{

/**
 * Constructs the bitset, initialising all bits to 0
 */
template<size_t size>
StaticBitset<size>::StaticBitset()
{
    reset_all();
}

template<size_t size>                            
StaticBitset<size>::~StaticBitset()
{
}

// element access

/**
 * Return the value of a single bit.
 *
 * @param bit the position of the bit to check
 */
template<size_t size>
bool StaticBitset<size>::operator[](size_t bit) const
{
    size_t segment = bit / segment_bitwidth;
    size_t pos = bit - (segment * segment_bitwidth);

    return m_segments[segment] & BIT(pos);
}

/**
 * Return the value of a single bit with bounds checking.
 *
 * @param bit the position of the bit to check
 * 
 * @throws Parable::OutOfRangeException if bit >= size of the bitset
 */
template<size_t size>
bool StaticBitset<size>::at(size_t bit) const
{
    if (bit >= size)
    {
        std::ostringstream ss;
        ss << "Cannot get bit " << bit << " from a bitset of size " << size;
        throw OutOfRangeException(ss.str().c_str());
    }

    return (*this)[bit];
}

/**
 * Check if all the bits are set.
 */
template<size_t size>
bool StaticBitset<size>::all() const
{
    for(size_t i = 0; i < num_segments-1; ++i)
    {
        if (~m_segments[i] != 0) return false;
    }

    // for last segment, check only the used bits
    if (last_segment_bits > 0)
    {
        size_t mask = BITS(last_segment_bits);
        if ((~m_segments[num_segments-1]) & mask != 0) return false;
    }
    else
    {
        if (~m_segments[num_segments-1] != 0) return false;
    }

    return true;
}

/**
 * Check if at least one of the bits are set.
 */
template<size_t size>
bool StaticBitset<size>::any() const
{
    for(size_t i = 0; i < num_segments; ++i)
    {
        if (m_segments[i] != 0) return true;
    }

    return false;
}

/**
 * Check if none of the bits are set.
 */
template<size_t size>
bool StaticBitset<size>::none() const
{
    return !any();
}

/**
 * Return the number of bits that are set.
 */
template<size_t size>
size_t StaticBitset<size>::count() const
{
    size_t count = 0;
    for(size_t i = 0; i < num_segments; ++i)
    {
        // brian kernighans algorithm
        Segment n = m_segments[i];
        while (n)
        {
            n = n & (n - 1);    // clear the least significant bit set
            ++count;
        }
    }
    return count;
}

// comparison

/**
 * Returns true if the values held by 2 equal size bitsets are equal
 */
template<size_t size>
bool StaticBitset<size>::operator==(const StaticBitset<size>& rhs) const
{
    for(size_t i = 0; i < num_segments; ++i)
    {
        if (m_segments[i] != rhs.m_segments[i]) return false;
    }
    return true;
}

/**
 * Returns false if the values held by 2 equal size bitsets are equal
 */
template<size_t size>
bool StaticBitset<size>::operator!=(const StaticBitset<size>& rhs) const
{
    return !((*this) == rhs);
}

/**
 * Checks if all the set bits in a bitset are also set in another. 
 * 
 * Equivalent to this & other == this.
 * 
 * @param other the other bitset to compare with 
 */
template<size_t size>
bool StaticBitset<size>::is_subset_of(const StaticBitset<size>& other) const
{
    for(size_t i = 0; i < num_segments; ++i)
    {
        Segment s = m_segments[i] & other.m_segments[i];
        if (s != m_segments[i]) return false;
    }

    return true;
}

/**
 * Checks if all the set bits in another bitset are also set in this one.
 * 
 * Equivalent to this & other == other.
 * 
 * @param other the other bitset to compare with 
 */
template<size_t size>
bool StaticBitset<size>::is_superset_of(const StaticBitset<size>& other) const
{
    return other.is_subset_of(*this);
}


// modifiers

/**
 * Perfoms bitwise AND on 2 bitsets of the same size.
 * 
 * @param other the other bitset to AND with
 */
template<size_t size>
StaticBitset<size>& StaticBitset<size>::operator&=(const StaticBitset<size>& other)
{
    for(size_t i = 0; i < num_segments; ++i)
    {
        m_segments[i] &= other.m_segments[i];
    }

    return (*this);
}

/**
 * Perfoms bitwise OR on 2 bitsets of the same size.
 * 
 * @param other the other bitset to OR with
 */
template<size_t size>
StaticBitset<size>& StaticBitset<size>::operator|=(const StaticBitset<size>& other)
{
    for(size_t i = 0; i < num_segments; ++i)
    {
        m_segments[i] |= other.m_segments[i];
    }

    return (*this);
}

/**
 * Perfoms bitwise XOR on 2 bitsets of the same size.
 * 
 * @param other the other bitset to XOR with
 */
template<size_t size>
StaticBitset<size>& StaticBitset<size>::operator^=(const StaticBitset<size>& other)
{
    for(size_t i = 0; i < num_segments; ++i)
    {
        m_segments[i] ^= other.m_segments[i];
    }

    return (*this);
}

/**
 * Inverts all the bits in the bitset.
 */
template<size_t size>
void StaticBitset<size>::flip_all()
{
    for(size_t i = 0; i < num_segments; ++i)
    {
        m_segments[i] = ~m_segments[i];
    }
}

/**
 * Inverts a specific bit in the bitset.
 * 
 * @param bit the bit to flip
 * 
 * @throws Parable::OutOfRangeException if bit >= size of the bitset
 */
template<size_t size>
void StaticBitset<size>::flip(size_t bit)
{
    if (bit >= size)
    {
        std::ostringstream ss;
        ss << "Cannot get bit " << bit << " from a bitset of size " << size;
        throw OutOfRangeException(ss.str().c_str());
    }

    size_t segment = bit / segment_bitwidth;
    size_t pos = bit - (segment * segment_bitwidth);

    m_segments[segment] ^= BIT(pos);
}

/**
 * Sets all the bits in the bitset.
 */
template<size_t size>
void StaticBitset<size>::set_all()
{
    for(size_t i = 0; i < num_segments; ++i)
    {
        m_segments[i] = ~((Segment)0);
    }
}

/**
 * Sets a specific bit in the bitset.
 * 
 * @param bit the bit to set
 * 
 * @throws Parable::OutOfRangeException if bit >= size of the bitset
 */
template<size_t size>
void StaticBitset<size>::set(size_t bit)
{
    if (bit >= size)
    {
        std::ostringstream ss;
        ss << "Cannot get bit " << bit << " from a bitset of size " << size;
        throw OutOfRangeException(ss.str().c_str());
    }

    size_t segment = bit / segment_bitwidth;
    size_t pos = bit - (segment * segment_bitwidth);

    m_segments[segment] |= BIT(pos);
}

/**
 * Reset all the bits in the bitset.
 */
template<size_t size>
void StaticBitset<size>::reset_all()
{
    for(size_t i = 0; i < num_segments; ++i)
    {
        m_segments[i] = 0;
    }
}

/**
 * Resets a specific bit in the bitset.
 * 
 * @param bit the bit to reset
 * 
 * @throws Parable::OutOfRangeException if bit >= size of the bitset
 */
template<size_t size>
void StaticBitset<size>::reset(size_t bit)
{
    if (bit >= size)
    {
        std::ostringstream ss;
        ss << "Cannot get bit " << bit << " from a bitset of size " << size;
        throw OutOfRangeException(ss.str().c_str());
    }

    size_t segment = bit / segment_bitwidth;
    size_t pos = bit - (segment * segment_bitwidth);

    m_segments[segment] &= ~BIT(pos);
}

/**
 * Outputs a string containing the bit pattern in the bitset.
 */
template<size_t size>
std::string StaticBitset<size>::to_string()
{
    std::ostringstream ss;
    for(size_t i = 0; i < size; ++i)
    {
        ss << (*this)[i] ? 1 : 0;
    }
    return ss.str();
}


/**
 * Finds the position of the first set bit in the bitset.
 */
template<size_t size>
size_t StaticBitset<size>::find_first_set()
{
    for(size_t i = 0; i < num_segments; ++i)
    {
        if (m_segments[i] != 0)
        {
            Segment s = (m_segments[i] & ~(m_segments[i]-1));
            int pos = 0;
            while (s >>= 1) ++pos;
            return i * segment_bitwidth + pos;
        }
    }

    std::ostringstream ss;
    ss << "No set bits in the bitset, cannot find first set bit";
    throw OutOfRangeException(ss.str().c_str());
}

/**
 * Finds the position of the first unset bit in the bitset.
 */
template<size_t size>
size_t StaticBitset<size>::find_first_unset()
{
    for(size_t i = 0; i < num_segments; ++i)
    {
        if (m_segments[i] == 0) return i * segment_bitwidth;
        Segment s = ~m_segments[i];
        if (s != 0)
        {
            s = (s & ~(s-1));
            int pos = 0;
            while (s >>= 1) ++pos;
            if (i == num_segments-1 && last_segment_bits > 0 && pos > last_segment_bits-1) break;
            return i * segment_bitwidth + pos;
        }
    }
    
    std::ostringstream ss;
    ss << "No unset bits in the bitset, cannot find first unset bit";
    throw OutOfRangeException(ss.str().c_str());
}

/**
 * Returns a list of the indices of the set bits.
 */
template<size_t size>
std::vector<size_t> StaticBitset<size>::get_set_bits()
{
    std::vector<size_t> out;
    for(size_t i = 0; i < num_segments; ++i)
    {
        Segment s = m_segments[i];
        size_t seg_offset = i * segment_bitwidth;
        if (s != 0)
        {
            int pos = 0;
            do
            {
                if (s & BIT(0)) out.push_back(pos + seg_offset);
                ++pos;
            } while(s >>= 1);
        }
    }

    return std::move(out);
}

/**
 * Returns a list of the indices of the unset bits.
 */
template<size_t size>
std::vector<size_t> StaticBitset<size>::get_unset_bits()
{
    std::vector<size_t> out;
    for(size_t i = 0; i < num_segments; ++i)
    {
        Segment s = m_segments[i];
        size_t seg_offset = i * segment_bitwidth;
        if (s != 0)
        {
            int pos = 0;
            do
            {
                if (!(s & BIT(0))) out.push_back(pos + seg_offset);
                ++pos;
            } while(s >>= 1);

            if (i == num_segments-1 && last_segment_bits != 0)
            {
                if(pos < last_segment_bits-1)
                {
                    while(pos < last_segment_bits)
                    {
                        out.push_back(pos + seg_offset);
                        ++pos;
                    }
                }
            }
            else if(pos != segment_bitwidth-1)
            {
                while(pos < segment_bitwidth)
                {
                    out.push_back(pos + seg_offset);
                    ++pos;
                }
            }
        }
    }

    return std::move(out);
}


}