#ifndef ADVENTURE_COMMANDER_STREAM_HPP
#define ADVENTURE_COMMANDER_STREAM_HPP

#include <iosfwd>

using namespace std;

class Stream {
protected:
	int keywords = 0;

public:
	virtual ostream& dump(ostream& out) const = 0;
	virtual istream& read(istream& in) = 0;
	virtual int getRequired() const = 0;
	bool isValid() const {
		//keywords contains all required bits, other bits don't care
		return (keywords & getRequired()) == getRequired();
	}
};

ostream& operator<<(ostream& out, const Stream& s);
istream& operator>>(istream& in, Stream& s);

#endif
