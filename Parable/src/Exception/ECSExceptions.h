#pragma once

#include "Exception.h"

namespace Parable::ECS
{


/**
 * Thrown when attempting an operation in a ComponentManager for a type it does not manage.
 */
class IncorrectManagerException : public Exception
{
public:
    using Exception::Exception;
};

/**
 * Thrown when attempting an operation for an entity which does not exist.
 */
class NullEntityException : public Exception
{
public:
    using Exception::Exception;
};

/**
 * Thrown when trying to create an ECS without configuring the builder fully.
 */
class ECSBuilderException : public Exception
{
public:
    using Exception::Exception;
};


}