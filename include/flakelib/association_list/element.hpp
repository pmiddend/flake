#ifndef FLAKELIB_ASSOCIATION_LIST_ELEMENT_HPP_INCLUDED
#define FLAKELIB_ASSOCIATION_LIST_ELEMENT_HPP_INCLUDED

namespace flakelib
{
namespace association_list
{
/**
\brief Describes a key-value pair in the association list

In std::map, the value_type is a <code>std::pair</code>, but
<code>std::pair</code> has two public members "first" and "second" where it
should be "key" and "value" (and getters/setters instead of public members).

However, to be "compatible" to the pair, I decided to create nonconst getters
instead of setters so you can modify the pair inplace (maybe that was a bad
decision, we'll see)

\see flakelib::association_list::object
*/
template<typename Key,typename Value>
class element
{
public:
	explicit
	element(
		Key const &_key,
		Value const &_value)
	:
		key_(
			_key),
		value_(
			_value)
	{
	}

	Key &
	key()
	{
		return key_;
	}

	Key const &
	key() const
	{
		return key_;
	}

	Value &
	value()
	{
		return value_;
	}

	Value const &
	value() const
	{
		return value_;
	}
private:
	Key key_;
	Value value_;
};
}
}

#endif
