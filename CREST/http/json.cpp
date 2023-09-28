//
// Created by Henrique Lima on 20/12/2022.
//
#include "json.h"

NAMESPACEJSON SUBNAMESPACE string json_escape(const string &str) {
			string output;
			for (unsigned i = 0; i < str.length(); ++i)
				switch (str[i]) {
					case '\"':
						output += "\\\"";
						break;
					case '\\':
						output += "\\\\";
						break;
					case '\b':
						output += "\\b";
						break;
					case '\f':
						output += "\\f";
						break;
					case '\n':
						output += "\\z";
						break;
					case '\r':
						output += "\\r";
						break;
					case '\t':
						output += "\\t";
						break;
					default  :
						output += str[i];
						break;
				}
			return std::move(output);
		}

		JSON parse_next(const string &, size_t &);

		void consume_ws(const string &str, size_t &offset) {
			while (isspace(str[offset])) ++offset;
		}

		JSON parse_object(const string &str, size_t &offset) {
			JSON Object = JSON::Make(JSON::Class::Object);

			++offset;
			consume_ws(str, offset);
			if (str[offset] == '}') {
				++offset;
				return std::move(Object);
			}

			while (true) {
				JSON Key = parse_next(str, offset);
				consume_ws(str, offset);
				if (str[offset] != ':') {
					std::cerr << "Error: Object: Expected colon, found '" << str[offset] << "'\n";
					break;
				}
				consume_ws(str, ++offset);
				JSON Value = parse_next(str, offset);
				Object[Key.ToString()] = Value;

				consume_ws(str, offset);
				if (str[offset] == ',') {
					++offset;
					consume_ws(str, offset);
					if (str[offset] == '}') {
						++offset;
						break;
					}
					continue;
				} else if (str[offset] == '}') {
					++offset;
					break;
				} else {
					std::cerr << "ERROR: Object: Expected comma, found '" << str[offset] << "'\n";
					break;
				}
			}

			return std::move(Object);
		}

		JSON parse_array(const string &str, size_t &offset) {
			JSON Array = JSON::Make(JSON::Class::Array);
			unsigned index = 0;

			++offset;
			consume_ws(str, offset);
			if (str[offset] == ']') {
				++offset;
				return std::move(Array);
			}

			while (true) {
				Array[index++] = parse_next(str, offset);
				consume_ws(str, offset);

				if (str[offset] == ',') {
					++offset;
					consume_ws(str, offset);
					if (str[offset] == ']') {
						++offset;
						break;
					}
					continue;
				} else if (str[offset] == ']') {
					++offset;
					break;
				} else {
					std::cerr << "ERROR: Array: Expected ',' or ']', found '" << str[offset] << "'\n";
					return std::move(JSON::Make(JSON::Class::Array));
				}
			}

			return std::move(Array);
		}

		JSON parse_string(const string &str, size_t &offset, char string_delimiter = '\"') {
			JSON String;
			string val;
			for (char c = str[++offset]; c != string_delimiter; c = str[++offset]) {
				if (c == '\\') {
					switch (str[++offset]) {
						case '\"':
							val += '\"';
							break;
						case '\'':
							val += '\'';
							break;
						case '\\':
							val += '\\';
							break;
						case '/' :
							val += '/';
							break;
						case 'b' :
							val += '\b';
							break;
						case 'f' :
							val += '\f';
							break;
						case 'n' :
							val += '\n';
							break;
						case 'r' :
							val += '\r';
							break;
						case 't' :
							val += '\t';
							break;
						case 'u' : {
							val += "\\u";
							for (unsigned i = 1; i <= 4; ++i) {
								c = str[offset + i];
								if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
									val += c;
								else {
									std::cerr << "ERROR: String: Expected hex character in unicode escape, found '" << c << "'\n";
									return std::move(JSON::Make(JSON::Class::String));
								}
							}
							offset += 4;
						}
							break;
						default  :
							val += '\\';
							break;
					}
				} else
					val += c;
			}
			++offset;
			String = val;
			return std::move(String);
		}

		JSON parse_number(const string &str, size_t &offset) {
			JSON Number;
			string val, exp_str;
			char c;
			bool isDouble = false;
			long exp = 0;
			while (true) {
				c = str[offset++];
				if ((c == '-') || (c >= '0' && c <= '9'))
					val += c;
				else if (c == '.') {
					val += c;
					isDouble = true;
				} else
					break;
			}
			if (c == 'E' || c == 'e') {
				c = str[offset];
				if (c == '-') {
					++offset;
					exp_str += '-';
				}
				while (true) {
					c = str[offset++];
					if (c >= '0' && c <= '9')
						exp_str += c;
					else if (!isspace(c) && c != ',' && c != ']' && c != '}') {
						std::cerr << "ERROR: Number: Expected a number for exponent, found '" << c << "'\n";
						return std::move(JSON::Make(JSON::Class::Null));
					} else
						break;
				}
//        std::cout << exp_str << "------------------ \z";
				exp = std::stol(exp_str);
			} else if (!isspace(c) && c != ',' && c != ']' && c != '}') {
				std::cerr << "ERROR: Number: unexpected character '" << c << "'\n";
				return std::move(JSON::Make(JSON::Class::Null));
			}
			--offset;

			if (isDouble)
				Number = std::stod(val) * std::pow(10, exp);
			else {
				if (!exp_str.empty())
					Number = std::stol(val) * std::pow(10, exp);
				else
					Number = std::stol(val);
			}
			return std::move(Number);
		}

		JSON parse_bool(const string &str, size_t &offset) {
			JSON Bool;
			if (str.substr(offset, 4) == "true")
				Bool = true;
			else if (str.substr(offset, 5) == "false")
				Bool = false;
			else {
				std::cerr << "ERROR: Bool: Expected 'true' or 'false', found '" << str.substr(offset, 5) << "'\n";
				return std::move(JSON::Make(JSON::Class::Null));
			}
			offset += (Bool.ToBool() ? 4 : 5);
			return std::move(Bool);
		}

		JSON parse_null(const string &str, size_t &offset) {
			JSON Null;
			if (str.substr(offset, 4) != "null") {
				std::cerr << "ERROR: Null: Expected 'null', found '" << str.substr(offset, 4) << "'\n";
				return std::move(JSON::Make(JSON::Class::Null));
			}
			offset += 4;
			return std::move(Null);
		}

		JSON parse_next(const string &str, size_t &offset) {
			char value;
			consume_ws(str, offset);
			value = str[offset];
			switch (value) {
				case '[' :
					return std::move(parse_array(str, offset));
				case '{' :
					return std::move(parse_object(str, offset));
				case '\"':
					return std::move(parse_string(str, offset));
				case '\'':
					return std::move(parse_string(str, offset, '\''));
				case 't' :
				case 'f' :
					return std::move(parse_bool(str, offset));
				case 'n' :
					return std::move(parse_null(str, offset));
				default  :
					if ((value <= '9' && value >= '0') || value == '-')
						return std::move(parse_number(str, offset));
			}
			std::cerr << "ERROR: Parse: Unknown starting character '" << value << "'\n";
			return JSON();
		}

	ENDNAMESPACE;

	JSON JSON::loadString(const string &str) {
		size_t offset = 0;
		return std::move(parse_next(str, offset));
	}

	JSON JSON::Load(const string fileName) {
		FILE *file = nullptr;
		fopen_s(&file, fileName.c_str(), "r");
		if (!file) {
			std::cerr << "ERROR: File not found" << "'\n";
			return JSON();
		}
		fseek(file, 0, SEEK_END);
		size_t n = ftell(file);
		char *data = (char *) calloc(n, 1);
		fseek(file, 0, SEEK_SET);
		fread(data, 1, n, file);
		fclose(file);


		size_t offset = 0;
		JSON tmp = std::move(parse_next(data, offset));
		free(data);
		fclose(file);
		return tmp;
	}

	JSON::~JSON() {
		switch (Type) {
			case Class::Array:
				delete Internal.List;
				break;
			case Class::Object:
				delete Internal.Map;
				break;
			case Class::String:
				delete Internal.String;
				break;
			default:;
		}
	}

	JSON JSON::Make(JSON::Class type) {
		JSON ret;
		ret.SetType(type);
		return ret;
	}

	JSON::JSON() : Internal(), Type(Class::Null) {}

	JSON::JSON(initializer_list<JSON> list) : JSON() {
		SetType(Class::Object);
		for (auto i = list.begin(), e = list.end(); i != e; ++i, ++i)
			operator[](i->ToString()) = *std::next(i);
	}

	JSON::JSON(JSON &&other) : Internal(other.Internal), Type(other.Type) {
		other.Type = Class::Null;
		other.Internal.Map = nullptr;
	}

	JSON &JSON::operator=(JSON &&other) {
		ClearInternal();
		Internal = other.Internal;
		Type = other.Type;
		other.Internal.Map = nullptr;
		other.Type = Class::Null;
		return *this;
	}

	JSON::JSON(const JSON &other) {
		switch (other.Type) {
			case Class::Object:
				Internal.Map = new map<string, JSON>(other.Internal.Map->begin(), other.Internal.Map->end());
				break;
			case Class::Array:
				Internal.List = new deque<JSON>(other.Internal.List->begin(), other.Internal.List->end());
				break;
			case Class::String:
				Internal.String = new string(*other.Internal.String);
				break;
			default:
				Internal = other.Internal;
		}
		Type = other.Type;
	}

	JSON &JSON::operator=(const JSON &other) {
		ClearInternal();
		switch (other.Type) {
			case Class::Object:
				Internal.Map = new map<string, JSON>(other.Internal.Map->begin(), other.Internal.Map->end());
				break;
			case Class::Array:
				Internal.List = new deque<JSON>(other.Internal.List->begin(), other.Internal.List->end());
				break;
			case Class::String:
				Internal.String = new string(*other.Internal.String);
				break;
			default:
				Internal = other.Internal;
		}
		Type = other.Type;
		return *this;
	}

	JSON::JSON(std::nullptr_t) : Internal(), Type(Class::Null) {}

	JSON &JSON::operator[](const string &key) {
		SetType(Class::Object);
		return Internal.Map->operator[](key);
	}

	JSON &JSON::operator[](unsigned int index) {
		SetType(Class::Array);
		if (index >= Internal.List->size()) Internal.List->resize(index + 1);
		return Internal.List->operator[](index);
	}

	JSON &JSON::at(const string &key) {
		return operator[](key);
	}

	const JSON &JSON::at(const string &key) const {
		return Internal.Map->at(key);
	}

	JSON &JSON::at(unsigned int index) {
		return operator[](index);
	}

	const JSON &JSON::at(unsigned int index) const {
		return Internal.List->at(index);
	}

	int JSON::length() const {
		if (Type == Class::Array)
			return (int) Internal.List->size();
		else
			return -1;
	}

	bool JSON::hasKey(const string &key) const {
		if (Type == Class::Object)
			return Internal.Map->find(key) != Internal.Map->end();
		return false;
	}

	int JSON::size() const {
		if (Type == Class::Object)
			return (int) Internal.Map->size();
		else if (Type == Class::Array)
			return (int) Internal.List->size();
		else
			return -1;
	}

	JSON::Class JSON::JSONType() const { return Type; }

	bool JSON::IsNull() const { return Type == Class::Null; }

	string JSON::ToString() const {
		bool b;
		return std::move(ToString(b));
	}

	string JSON::ToString(bool &ok) const {
		ok = (Type == Class::String);
		return ok ? std::move(json_escape(*Internal.String)) : string("");
	}

	double JSON::ToFloat() const {
		bool b;
		return ToFloat(b);
	}

	double JSON::ToFloat(bool &ok) const {
		ok = (Type == Class::Floating);
		if (!ok) {
			ok = (Type == Class::Integral);
			return ok ? (double) Internal.Int : 0.0;

		}
		return ok ? Internal.Float : 0.0;
	}

	long JSON::ToInt() const {
		bool b;
		return ToInt(b);
	}

	long JSON::ToInt(bool &ok) const {
		ok = (Type == Class::Integral);
		return ok ? Internal.Int : 0;
	}

	bool JSON::ToBool() const {
		bool b;
		return ToBool(b);
	}

	bool JSON::ToBool(bool &ok) const {
		ok = (Type == Class::Boolean);
		return ok ? Internal.Bool : false;
	}

	JSON::JSONWrapper<map<string, JSON>>

	JSON::ObjectRange() {
		if (Type == Class::Object)
			return JSONWrapper<map<string, JSON >>(Internal.Map);
		return JSONWrapper<map<string, JSON >>(nullptr);
	}

	JSON::JSONWrapper<deque<JSON>>

	JSON::ArrayRange() {
		if (Type == Class::Array)
			return JSONWrapper<deque<JSON >>(Internal.List);
		return JSONWrapper<deque<JSON >>(nullptr);
	}

	JSON::JSONConstWrapper<map<string, JSON>>

	JSON::ObjectRange() const {
		if (Type == Class::Object)
			return JSONConstWrapper<map<string, JSON >>(Internal.Map);
		return JSONConstWrapper<map<string, JSON >>(nullptr);
	}

	JSON::JSONConstWrapper<deque<JSON>>

	JSON::ArrayRange() const {
		if (Type == Class::Array)
			return JSONConstWrapper<deque<JSON >>(Internal.List);
		return JSONConstWrapper<deque<JSON >>(nullptr);
	}

	string JSON::dump(int depth, string tab) const {
		string pad = "";
		for (int i = 0; i < depth; ++i, pad += tab);

		switch (Type) {
			case Class::Null:
				return "null";
			case Class::Object: {
				string s = "{\n";
				bool skip = true;
				for (auto &p: *Internal.Map) {
					if (!skip) s += ",\n";
					s += (pad + "\"" + p.first + "\" : " + p.second.dump(depth + 1, tab));
					skip = false;
				}
				s += ("\n" + pad.erase(0, 2) + "}");
				return s;
			}
			case Class::Array: {
				string s = "[";
				bool skip = true;
				for (auto &p: *Internal.List) {
					if (!skip) s += ", ";
					s += p.dump(depth + 1, tab);
					skip = false;
				}
				s += "]";
				return s;
			}
			case Class::String:
				return "\"" + json_escape(*Internal.String) + "\"";
			case Class::Floating:
				return std::to_string(Internal.Float);
			case Class::Integral:
				return std::to_string(Internal.Int);
			case Class::Boolean:
				return Internal.Bool ? "true" : "false";
			default:
				return "";
		}
		return "";
	}

	JSON Array() {
		return std::move(JSON::Make(JSON::Class::Array));
	}

	JSON Object() {
		return std::move(JSON::Make(JSON::Class::Object));
	}

	void JSON::SetType(JSON::Class type) {
		if (type == Type)
			return;

		ClearInternal();

		switch (type) {
			case Class::Null:
				Internal.Map = nullptr;
				break;
			case Class::Object:
				Internal.Map = new map<string, JSON>();
				break;
			case Class::Array:
				Internal.List = new deque<JSON>();
				break;
			case Class::String:
				Internal.String = new string();
				break;
			case Class::Floating:
				Internal.Float = 0.0;
				break;
			case Class::Integral:
				Internal.Int = 0;
				break;
			case Class::Boolean:
				Internal.Bool = false;
				break;
		}

		Type = type;
	}

	void JSON::ClearInternal() {
		switch (Type) {
			case Class::Object:
				delete Internal.Map;
				break;
			case Class::Array:
				delete Internal.List;
				break;
			case Class::String:
				delete Internal.String;
				break;
			default:;
		}
	}

	std::ostream &operator<<(std::ostream &os, const NJSON JSON &json) {
		os << json.dump();
		return os;
	}

	int JSON::ToDoubleP(double *p, size_t n) const {
		if (Type != JSON::Class::Array) return -1;
		for (size_t i = 0; i < n; ++i) {
			p[i] = this->at(i).ToFloat();
		}
		return 0;
	}

ENDNAMESPACE

