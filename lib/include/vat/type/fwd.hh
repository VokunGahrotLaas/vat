#pragma once

// STL
#include <memory>

namespace vat::type
{

class Type;
class TypeType;
class Number;
class Bool;
class Unit;
class Fn;
class Never;
class Name;

using SharedType = std::shared_ptr<Type>;
using SharedConstType = std::shared_ptr<Type const>;

} // namespace vat::type
