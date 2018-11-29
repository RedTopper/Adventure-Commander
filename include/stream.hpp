#ifndef ADVENTURE_COMMANDER_STREAM_HPP
#define ADVENTURE_COMMANDER_STREAM_HPP

#include <iosfwd>

class Stream {
protected:
	unsigned int keywords = 0;

public:
	virtual std::ostream& dump(std::ostream& out) const = 0;
	virtual std::istream& read(std::istream& in) = 0;
	virtual unsigned int getRequired() const = 0;
	bool isValid() const {
		//keywords contains all required bits, other bits don't care
		unsigned int required = getRequired();
		return (keywords & required) == required;
	}
};

std::ostream& operator<<(std::ostream& out, const Stream& s);
std::istream& operator>>(std::istream& in, Stream& s);

#endif
