#ifndef FLAKELIB_ASSOCIATION_LIST_OBJECT_HPP_INCLUDED
#define FLAKELIB_ASSOCIATION_LIST_OBJECT_HPP_INCLUDED

#include <flakelib/association_list/element.hpp>
#include <fcppt/container/out_of_range.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/next_prior.hpp>
#include <boost/spirit/home/phoenix/bind.hpp>
#include <boost/spirit/home/phoenix/core.hpp>
#include <boost/spirit/home/phoenix/operator.hpp>
#include <algorithm>
#include <memory>
#include <vector>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace association_list
{
/**
\brief This class implements an association list, or a list of key-value pairs
\tparam Key Any type (doesn't have to be ordered, but equality comparable)
\tparam Value Any type
\tparam A The allocator type

\todo
Implement <code>cbegin, cend, crbegin, crend</code> (I was too lazy for this at
the time of this writing)

\section association_list_motivation Motivation

A short introduction to association lists can be found here:

http://en.wikipedia.org/wiki/Association_list

Note that the <code>association_list</code> does <em>not</em> directly export
the <code>Key</code> and <code>Value</code> template parameters. This is
because the stdlib reserves the typedef <code>value_type</code> for the
container's value type, which is association_list::element for an
<code>association_list</code>, not <code>Value</code>. To prevent confusion
when using <code>key_type</code> and <code>value_type</code>, there is just
<code>value_type</code> (pointing to <code>element</code>!).

\section association_list_comparison Comparison

Here are the differences to other common containers:

<table>
<tr>
<th>
Container
</th>
<th>
Differences to association lists
</th>
</tr>
<tr>
<td><code>std::%map</code></td>
<td>
<ul>
<li>Has logarithmic search time (an association list has linear search time)</li>
<li><code>Key</code> has to be ordered</li>
<li>The key sequence is reordered (according to the key ordering)</li>
<li>No duplicate entries</li>
<li>Iteration is over pairs which have only public <code>first</code> and <code>second</code> public members.</li>
<li>Keys are <code>const</code></li>
</ul>
</td>
</tr>
<tr>
<td><code>std::%multimap</code></td>
<td>
<ul>
<li>All of the differences of the <code>std::%map</code>, except multiple entries are allowed</li>
</ul>
</td>
</tr>
<tr>
<td><code>std::%vector<std::%pair<Key,Value>></code></td>
<td>
<ul>
<li>Grabbing a single element involves an explicit search operation (ugly, inconvenient)</li>
</ul>
</td>
</tr>
<tr>
<td><code>boost::%unordered_map</code></td>
<td>
<ul>
<li>Has constant search time (an association list has linear search time)</li>
<li><code>Key</code> has to be hashable</li>
<li>The key sequence is reordered (according to the hashing)</li>
<li>No duplicate entries</li>
<li>Iteration is over pairs which have only public <code>first</code> and <code>second</code> public members.</li>
<li>Keys are <code>const</code></li>
</ul>
</td>
</tr>
</table>
*/
template
<
	typename Key,
	typename Value,
	typename A = std::allocator< association_list::element<Key,Value> >
>
class object
{
public:
	/**
	\brief A type that represents the data type stored in an association_list.
	*/
	typedef association_list::element<Key,Value> value_type;
	/**
	\brief The internal sequence type (public for exposition and convenience)
	*/
	typedef std::vector<value_type,A> sequence_type;
	/**
	\brief A type that represents the allocator class for the association_list object.
	*/
	typedef typename sequence_type::allocator_type allocator_type;
	/**
	\brief A type that counts the number of elements in an association_list.
	*/
	typedef typename sequence_type::size_type size_type;
	/**
	\brief A type that provides the difference between the addresses of two elements in an association_list.
	*/
	typedef typename sequence_type::difference_type difference_type;
	/**
	\brief A type that provides a pointer to an element in an association_list.
	*/
	typedef typename sequence_type::pointer pointer;
	/**
	\brief A type that provides a pointer to a <code>const</code> element in an association_list.
	*/
	typedef typename sequence_type::const_pointer const_pointer;
	/**
	\brief A type that provides a reference to an element stored in an association_list.
	*/
	typedef typename sequence_type::reference reference;
	/**
	\brief A type that provides a reference to a <code>const</code> element stored in an association_list for reading and performing <code>const</code> operations.
	*/
	typedef typename sequence_type::const_reference const_reference;
	/**
	\brief A type that provides a random-access iterator that can read or modify any element in an association_list.
	*/
	typedef typename sequence_type::iterator iterator;
	/**
	\brief A type that provides a random-access iterator that can read a <code>const</code> element in an association_list.
	*/
	typedef typename sequence_type::const_iterator const_iterator;
	/**
	\brief A type that provides a random-access iterator that can read or modify any element in a reversed vector.
	*/
	typedef std::reverse_iterator<iterator> reverse_iterator;
	/**
	\brief A type that provides a random-access iterator that can read any <code>const</code> element in the association_list.
	*/
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	/**
	\brief Returns a random-access iterator to the first element in the container.
	*/
	iterator
	begin()
	{
		return sequence_.begin();
	}

	/**
	\brief Returns a random-access iterator to the first element in the container.
	*/
	const_iterator
	begin() const
	{
		return sequence_.begin();
	}

	/**
	\brief Returns a random-access iterator that points just beyond the end of the association_list.
	*/
	iterator
	end()
	{
		return sequence_.end();
	}

	/**
	\brief Returns a random-access iterator that points just beyond the end of the association_list.
	*/
	const_iterator
	end() const
	{
		return sequence_.end();
	}

	/**
	\brief Returns an iterator to the first element in a reversed vector.
	*/
	reverse_iterator
	rbegin()
	{
		return sequence_.rbegin();
	}

	/**
	\brief Returns an iterator to the first element in a reversed vector.
	*/
	const_reverse_iterator
	rbegin() const
	{
		return sequence_.rbegin();
	}

	/**
	\brief Returns an iterator to the end of a reversed vector.
	*/
	reverse_iterator
	rend()
	{
		return sequence_.rend();
	}

	/**
	\brief Returns an iterator to the end of a reversed vector.
	*/
	const_reverse_iterator
	rend() const
	{
		return sequence_.rend();
	}

	/**
	\brief Add an element to the end of the association_list.
	\param t The element to add
	*/
	void
	push_back(
		value_type const &t)
	{
		sequence_.push_back(
			t);
	}

	/**
	\brief Inserts an element into the vector at a specified position.
	\param position An iterator to the element <em>after</em> the new element
	\param t The new element
	*/
	iterator
	insert(
		iterator const position,
		value_type const &t)
	{
		sequence_.insert(
			position,
			t);
	}

	/**
	\brief Search for duplicate entries and remove all but the first one of each duplicate.

	This operation is relatively expensive.
	*/
	void
	prune()
	{
		iterator new_end =
			this->end();

		for(iterator it = this->begin(); it != new_end; ++it)
		{
			new_end =
				std::remove_if(
					boost::next(
						it),
					new_end,
					boost::phoenix::bind(
						static_cast<Key const &(association_list::element<Key,Value>::*)()>(
							&association_list::element<Key,Value>::key)) == *it);
		}

		sequence_.erase(
			new_end,
			sequence_.end());
	}

	/**
	\brief Erase all pairs which have key \p k
	*/
	void
	erase(
		Key const &k)
	{
		sequence_.erase(
			std::remove_if(
				this->begin(),
				this->end(),
				boost::phoenix::bind(
					static_cast<Key const &(association_list::element<Key,Value>::*)()>(
						&association_list::element<Key,Value>::key)) == k),
			sequence_.end());
	}

	/**
	\brief Erase all elements from the list
	*/
	void
	clear()
	{
		sequence_.clear();
	}

	/**
	\brief Conveniently access an element using a key.
	\throws fcppt::container::out_of_range If the key doesn't exist
	*/
	/*
	Value const &
	operator[](
		Key const &k) const
	{
		const_iterator const it =
			std::find_if(
				this->begin(),
				this->end(),
				boost::phoenix::bind(
					static_cast<Key const &(association_list::element<Key,Value>::*)() const>(
						&association_list::element<Key,Value>::key),
					boost::phoenix::arg_names::arg1) == k);

		if(it == this->end())
			throw fcppt::container::out_of_range();

		return it->value();
	}
	*/

	/**
	\brief Conveniently access an element using a key.
	\throws fcppt::container::out_of_range If the key doesn't exist
	*/
	Value &
	operator[](
		Key const &k)
	{
		iterator const it =
			std::find_if(
				this->begin(),
				this->end(),
				boost::phoenix::bind(
					static_cast<Key &(association_list::element<Key,Value>::*)()>(
						&association_list::element<Key,Value>::key),
					boost::phoenix::arg_names::arg1) == k);

		if(it == this->end())
			throw fcppt::container::out_of_range();

		return it->value();
	}
private:
	sequence_type sequence_;
};
}
}

#endif
