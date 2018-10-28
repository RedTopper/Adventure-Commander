#ifndef ADVENTURE_COMMANDER_STREAMABLE_HPP
#define ADVENTURE_COMMANDER_STREAMABLE_HPP

#include <iosfwd>

using namespace std;

class Stream {
private:
	bool fail = false;

public:
	virtual ostream& dump(ostream& out) const = 0;
	virtual istream& read(istream& in) = 0;
	bool isFail() const {
		return fail;
	}
};

ostream& operator<<(ostream& out, const Stream& s);
istream& operator>>(istream& in, Stream& s);

#endif
