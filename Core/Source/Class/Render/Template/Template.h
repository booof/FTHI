// Map Template
template <class First, class Second> class template_map
{
private:

	// Define Map Template
	typedef typename std::map<First, Second> Tmap;
	Tmap temp_map;

public:

	// Define Iterator
	typedef typename Tmap::const_iterator const_iterator;

	// Define Begin and End Functions
	const_iterator begin() const { return temp_map.begin(); }
	const_iterator end() const { return temp_map.end(); }
};