#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <sstream>
#include <iostream>
#include <list>
#include <json-c/json.h>
#include <common.h>

class Option {

public:
	/* subset of json_type's */
	typedef enum {
		type_boolean = 1,
		type_double,
		type_int,
		type_object,
		type_array,
		type_string
	} type_t;

	Option(const char *key, struct json_object *jso);
	Option(const char *key, const char *value);
	Option(const char *key, int value);
	Option(const char *key, double value);
	Option(const char *key, bool value);

	Option(const Option &); // copy constructor to refcount jso properly

	virtual ~Option();

	const std::string key() const { return _key; }
	operator const char *() const;
	operator int() const;
	operator double() const;
	operator bool() const;
	operator struct json_object*() const;

	type_t type() const { return _type; }

	const std::string toString() const {
		std::ostringstream oss;
		oss << "Option <"<< key() << ">=<type="<< type()<< ">=<val=";
		switch(type()) {
				case type_boolean: oss<< (bool)(*this);break;
				case type_double:  oss<< (double)(*this); break;
				case type_int:     oss<< (int)(*this); break;
				case type_string:  oss<< (const char*)(*this); break;
				case type_object: oss<< "json object"; break; // TODO add operators
				case type_array: oss<< "json array"; break; // TODO add operators
		}
		oss << ">";
		return oss.str();
	}


private:
	Option(const char *key);

	Option & operator= (const Option &); // assignment op.

	std::string _key;
	type_t _type;

	std::string _value_string;

	union {
		struct json_object *jso; // if the passed object was an array/object the array/object is kept here
		const char *string;
		int integer;
		double floating;
		int boolean:1;
	} value;

};

inline
std::ostream & operator << (std::ostream &s, const Option &l) {
	s<< l.toString();
	return s;
}


class OptionList { //: public List<Option> {

public:
	typedef std::list<Option>::iterator iterator;
	typedef std::list<Option>::const_iterator const_iterator;

	static const Option& lookup(std::list<Option> const &options, const std::string &key);
	static const char *lookup_string(std::list<Option> const &options, const char *key);
	static const char *lookup_string_tolower(std::list<Option> const &options, const char *key);
	static int    lookup_int(std::list<Option> const &options, const char *key);
	static bool   lookup_bool(std::list<Option> const &options, const char *key);
	static double lookup_double(std::list<Option> const &options, const char *key);
	static struct json_object *lookup_json_array(std::list<Option> const &options, const char *key);
	static struct json_object *lookup_json_object(std::list<Option> const &options, const char *key);
	static void dump(std::list<Option> const &options);

	static void parse();

protected:

};


template <typename T, T (*L)(const std::list<Option> &, const char *)>
T lookup_mandatory(const std::list<Option> &olist, const std::string &o, const std::string &errorcontext) {
	T v;
	try {
		v = L(olist, o.c_str());
	} catch (vz::VZException &e) {
			print(log_error, "Missing mandatory option: %s", errorcontext.c_str(), o.c_str());
			throw vz::OptionNotFoundException(e.reason());
	}
	return v;
}

template <typename T, T (*L)(const std::list<Option> &, const char *)>
T lookup_optional(const std::list<Option> &olist, const std::string &o, const T &def) {
	T v;
	try {
		v = L(olist, o.c_str());
	} catch (vz::VZException &e) {
		return def;
	}
	return v;
}


#endif /* _OPTIONS_H_ */
