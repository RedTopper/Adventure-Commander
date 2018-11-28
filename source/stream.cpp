#include "stream.hpp"

using namespace std;

ostream& operator<<(ostream& out, const Stream& d) {
	return d.dump(out);
}

istream& operator>>(istream& in, Stream& d) {
	return d.read(in);
}