/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
 */
 
#ifndef _PFIELDSET_H_
#define _PFIELDSET_H_

// Class to contain a set of PFields used by one instrument

class PField;

class PFieldSet {
public:
	PFieldSet(int numfields);
	~PFieldSet();
	void		load(PField *, int index);
	PField & 	operator[](int index) const { return *_array[index]; }
	int			size() const { return _size; }
private:
	PField	**_array;
	int		_size;
};
	
#endif	// _PFIELDSET_H_

