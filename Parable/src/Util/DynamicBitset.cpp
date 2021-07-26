#include "DynamicBitset.h"

#include "pblpch.h"

#include "Core/Base.h"

#include "Exception/LogicExceptions.h"
#include "Exception/MemoryExceptions.h"

#include "Memory/Allocator.h"
#include "Memory/Allocator.tpp"


namespace Parable::Util
{

/**
 * Constructs a bitset which holds size bits, and allocates space for them with allocator
 * 
 * @param size the number of bits to store
 * @param allocator Parable::Allocator to allocate space for the bits
 * 
 * @throws Parable::AllocationFailedException if the allocation for bits fails
 */
DynamicBitset::DynamicBitset(size_t size, Allocator& allocator) :
                                                m_size(size),
                                                m_allocator(allocator)
{
    m_num_segments = size / segment_bitwidth;
    m_last_segment_bits = size - (m_num_segments * segment_bitwidth);
    if (m_last_segment_bits > 0) ++m_num_segments;

    m_segments = allocator.allocate_array<Segment>(m_num_segments);

    if (m_segments != nullptr)
    {
        throw Parable::AllocationFailedException("Failed to allocate space for bitset.");
    }

    reset_all();
}
                                        
DynamicBitset::~DynamicBitset()
{
    m_allocator.deallocate_array<Segment>(m_segments);
}

// element access

/**
 * Return the value of a single bit.
 *
 * @param bit the position of the bit to check
 */
bool DynamicBitset::operator[](size_t bit)
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
bool DynamicBitset::at(size_t bit)
{
    if (bit >= m_size)
    {
        std::ostringstream ss;
        ss << "Cannot get bit " << bit << " from a bitset of size " << m_size;
        throw OutOfRangeException(ss.str().c_str());
    }

    return (*this)[bit];
}

/**
 * Check if all the bits are set.
 */
bool DynamicBitset::all() const
{
    for(size_t i = 0; i < m_num_segments-1; ++i)
    {
        if (~m_segments[i] != 0) return false;
    }

    // for last segment, check only the used bits
    if (m_last_segment_bits > 0)
    {
        for(size_t i = 0; i < m_last_segment_bits; ++i)
        {
            if (!(*this)[i]) return false;
        }
    }

    return true;
}

/**
 * Check if at least one of the bits are set.
 */
bool DynamicBitset::any() const
{
    for(size_t i = 0; i < m_num_segments; ++i)
    {
        if (m_segments[i] != 0) return true;
    }

    // for last segment, check only the used bits
    if (m_last_segment_bits > 0)
    {
        for(size_t i = 0; i < m_last_segment_bits; ++i)
        {
            if ((*this)[i]) return true;
        }
    }

    return false;
}

/**
 * Check if none of the bits are set.
 */
bool DynamicBitset::none() const
{
    return !any();
}

/**
 * Return the number of bits that are set.
 */
size_t DynamicBitset::count() const
{
    size_t count = 0;
    for(size_t i = 0; i < m_num_segments; ++i)
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
 * 
 * @throws Parable::IncompatibleObjectException if the bitsets have different sizes
 */
bool DynamicBitset::operator==(const DynamicBitset& rhs) const
{
    if (m_size != rhs.get_size())
    {
        std::ostringstream ss;
        ss << "Cannot compare a bitset of size " << m_size << " with a bitset of size ";
        throw IncompatibleObjectException(ss.str().c_str());
    }

    for(size_t i = 0; i < m_num_segments; ++i)
    {
        if (m_segments[i] != rhs.m_segments[i]) return false;
    }
    return true;
}

/**
 * Returns false if the values held by 2 equal size bitsets are equal
 * 
 * @throws Parable::IncompatibleObjectException if the bitsets have different sizes
 */
bool DynamicBitset::operator!=(const DynamicBitset& rhs) const
{
    if (m_size != rhs.get_size())
    {
        std::ostringstream ss;
        ss << "Cannot compare a bitset of size " << m_size << " with a bitset of size ";
        throw IncompatibleObjectException(ss.str().c_str());
    }

    return !((*this) == rhs);
}

// modifiers

/**
 * Perfoms bitwise AND on 2 bitsets of the same size.
 * 
 * @param other the other bitset to AND with
 * 
 * @throws Parable::IncompatibleObjectException if the bitsets have different sizes
 */
DynamicBitset& DynamicBitset::operator&=(const DynamicBitset& other)
{
    if (m_size != other.get_size())
    {
        std::ostringstream ss;
        ss << "Cannot compare a bitset of size " << m_size << " with a bitset of size ";
        throw IncompatibleObjectException(ss.str().c_str());
    }

    for(size_t i = 0; i < m_num_segments; ++i)
    {
        m_segments[i] &= other.m_segments[i];
    }

    return (*this);
}

/**
 * Perfoms bitwise OR on 2 bitsets of the same size.
 * 
 * @param other the other bitset to OR with
 * 
 * @throws Parable::IncompatibleObjectException if the bitsets have different sizes
 */
DynamicBitset& DynamicBitset::operator|=(const DynamicBitset& other)
{
    if (m_size != other.get_size())
    {
        std::ostringstream ss;
        ss << "Cannot compare a bitset of size " << m_size << " with a bitset of size ";
        throw IncompatibleObjectException(ss.str().c_str());
    }

    for(size_t i = 0; i < m_num_segments; ++i)
    {
        m_segments[i] |= other.m_segments[i];
    }

    return (*this);
}

/**
 * Perfoms bitwise XOR on 2 bitsets of the same size.
 * 
 * @param other the other bitset to XOR with
 * 
 * @throws Parable::IncompatibleObjectException if the bitsets have different sizes
 */
DynamicBitset& DynamicBitset::operator^=(const DynamicBitset& other)
{
    if (m_size != other.get_size())
    {
        std::ostringstream ss;
        ss << "Cannot compare a bitset of size " << m_size << " with a bitset of size ";
        throw IncompatibleObjectException(ss.str().c_str());
    }

    for(size_t i = 0; i < m_num_segments; ++i)
    {
        m_segments[i] ^= other.m_segments[i];
    }

    return (*this);
}

/**
 * Inverts all the bits in the bitset.
 */
void DynamicBitset::flip_all()
{
    for(size_t i = 0; i < m_num_segments; ++i)
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
void DynamicBitset::flip(size_t bit)
{
    if (bit >= m_size)
    {
        std::ostringstream ss;
        ss << "Cannot get bit " << bit << " from a bitset of size " << m_size;
        throw OutOfRangeException(ss.str().c_str());
    }

    size_t segment = bit / segment_bitwidth;
    size_t pos = bit - (segment * segment_bitwidth);

    m_segments[segment] ^= BIT(pos);
}

/**
 * Sets all the bits in the bitset.
 */
void DynamicBitset::set_all()
{
    for(size_t i = 0; i < m_num_segments; ++i)
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
void DynamicBitset::set(size_t bit)
{
    if (bit >= m_size)
    {
        std::ostringstream ss;
        ss << "Cannot get bit " << bit << " from a bitset of size " << m_size;
        throw OutOfRangeException(ss.str().c_str());
    }

    size_t segment = bit / segment_bitwidth;
    size_t pos = bit - (segment * segment_bitwidth);

    m_segments[segment] |= BIT(pos);
}

/**
 * Reset all the bits in the bitset.
 */
void DynamicBitset::reset_all()
{
    for(size_t i = 0; i < m_num_segments; ++i)
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
void DynamicBitset::reset(size_t bit)
{
    if (bit >= m_size)
    {
        std::ostringstream ss;
        ss << "Cannot get bit " << bit << " from a bitset of size " << m_size;
        throw OutOfRangeException(ss.str().c_str());
    }

    size_t segment = bit / segment_bitwidth;
    size_t pos = bit - (segment * segment_bitwidth);

    m_segments[segment] &= ~BIT(pos);
}

/**
 * Outputs a string containing the bit pattern in the bitset.
 */
std::string DynamicBitset::to_string()
{
    std::ostringstream ss;
    for(size_t i = 0; i < m_size; ++i)
    {
        ss << (*this)[i] ? 1 : 0;
    }
    return ss.str();
}


}