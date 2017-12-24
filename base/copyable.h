#ifndef ENGINE_BASE_COPYABLE_H
#define ENGINE_BASE_COPYABLE_H


/// A tag class emphasises the objects are copyable.
/// The empty base class optimization applies.
/// Any derived class of copyable should be a value type.
class copyable
{
};


#endif  // ENGINE_BASE_COPYABLE_H
