#include <iostream>

#if defined(_MSC_VER)
#include "windows.h" // test no inadvertant macro expansions
#endif
#include <jsoncons/json.hpp>
#include <jsoncons/json_encoder.hpp>
#include <jsoncons_ext/bson/bson_cursor.hpp>
#include <jsoncons_ext/bson/bson.hpp>
//#include <catch/catch.hpp>
#include <sstream>
#include <vector>
#include <utility>
#include <ctime>
#include <fstream>

using namespace jsoncons;
using namespace std;

void CHECK_LOG(bool bRight)
{
	if (bRight) {
		cout << "check is ok" << endl;
	}
	else {
		cout << "check is wrong" << endl;
	}
}

void CHECK_LOG_STR(bool bRight, const char* str)
{
	if (str) {
		if (bRight) {
			cout <<"[right]\t"<< "check " << str << " is ok" << endl;
		}
		else {
			cout << "[wrong]\t"<<"check "<<str<<" is wrong" << endl;
		}
	}
	else {
		CHECK_LOG(bRight);
	}
	
}

#define __CHECK_LOG__(A) CHECK_LOG_STR(A, #A)

#define SECTION(sec) cout<< sec << endl;

void testbsoncursor()
{
	ojson j = ojson::parse(R"(
    {
       "application": "hiking",
       "reputons": [
       {
           "rater": "HikingAsylum",
           "assertion": "advanced",
           "rated": "Marilyn C",
           "rating": 0.90
         }
       ]
    }
    )");

	std::vector<uint8_t> data;
	bson::encode_bson(j, data);

	{
		bson::bson_bytes_cursor cursor(data);

		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::begin_object);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::key);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::string_value);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::key);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::begin_array);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::begin_object);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::key);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::string_value);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::key);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::string_value);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::key);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::string_value);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::key);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::double_value);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::end_object);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::end_array);
		cursor.next();
		__CHECK_LOG__(cursor.current().event_type() == staj_event_type::end_object);
		cursor.next();
		__CHECK_LOG__(cursor.done());
	}
}

void test_equal(const std::vector<uint8_t>& v, const std::vector<uint8_t>& expected)
{
	__CHECK_LOG__(v.size() == expected.size());

	for (std::size_t i = 0; i < v.size(); ++i)
	{
		__CHECK_LOG__(v[i] == expected[i]);
	}
}

void check_equal(const std::vector<uint8_t>& v, const std::vector<uint8_t>& expected)
{
	test_equal(v, expected);
	try
	{
		json j = bson::decode_bson<json>(v);
		std::vector<uint8_t> u;
		bson::encode_bson(j, u);
		test_equal(v,u);
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

void testbsonencoder()
{
	//SECTION("array")
	{
		std::vector<uint8_t> v;
		bson::bson_bytes_encoder encoder(v);

		encoder.begin_array();
		encoder.int64_value((std::numeric_limits<int64_t>::max)());
		encoder.uint64_value((uint64_t)(std::numeric_limits<int64_t>::max)());
		encoder.double_value((std::numeric_limits<double>::max)());
		encoder.bool_value(true);
		encoder.bool_value(false);
		encoder.null_value();
		encoder.string_value("Pussy cat");
		std::vector<uint8_t> purr = { 'h','i','s','s' };
		encoder.byte_string_value(purr); // default subtype is user defined
		// encoder.byte_string_value(purr, 0x80);
		encoder.end_array();
		encoder.flush();

		std::vector<uint8_t> bson = { 0x4e,0x00,0x00,0x00,
									 0x12, // int64
									 0x30, // '0'
									 0x00, // terminator
									 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,
									 0x12, // int64
									 0x31, // '1'
									 0x00, // terminator
									 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,
									 0x01, // double
									 0x32, // '2'
									 0x00, // terminator
									 0xff,0xff,0xff,0xff,0xff,0xff,0xef,0x7f,
									 0x08, // bool
									 0x33, // '3'
									 0x00, // terminator
									 0x01,
									 0x08, // bool
									 0x34, // '4'
									 0x00, // terminator
									 0x00,
									 0x0a, // null
									 0x35, // '5'
									 0x00, // terminator
									 0x02, // string
									 0x36, // '6'
									 0x00, // terminator
									 0x0a,0x00,0x00,0x00, // string length
									 'P','u','s','s','y',' ','c','a','t',
									 0x00, // terminator
									 0x05, // binary
									 0x37, // '7'
									 0x00, // terminator
									 0x04,0x00,0x00,0x00, // byte string length
									 0x80, // subtype
									 'h','i','s','s',
									 0x00 // terminator
		};

		check_equal(v, bson);

	}

	//SECTION("object")
	{
		std::vector<uint8_t> v;
		bson::bson_bytes_encoder encoder(v);

		encoder.begin_object();
		encoder.key("0");
		encoder.int64_value((std::numeric_limits<int64_t>::max)());
		encoder.key("1");
		encoder.uint64_value((uint64_t)(std::numeric_limits<int64_t>::max)());
		encoder.key("2");
		encoder.double_value((std::numeric_limits<double>::max)());
		encoder.key("3");
		encoder.bool_value(true);
		encoder.key("4");
		encoder.bool_value(false);
		encoder.key("5");
		encoder.null_value();
		encoder.key("6");
		encoder.string_value("Pussy cat");
		encoder.key("7");
		std::vector<uint8_t> hiss = { 'h','i','s','s' };
		encoder.byte_string_value(hiss);
		encoder.end_object();
		encoder.flush();

		std::vector<uint8_t> bson = { 0x4e,0x00,0x00,0x00,
									 0x12, // int64
									 0x30, // '0'
									 0x00, // terminator
									 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,
									 0x12, // int64
									 0x31, // '1'
									 0x00, // terminator
									 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,
									 0x01, // double
									 0x32, // '2'
									 0x00, // terminator
									 0xff,0xff,0xff,0xff,0xff,0xff,0xef,0x7f,
									 0x08, // bool
									 0x33, // '3'
									 0x00, // terminator
									 0x01,
									 0x08, // bool
									 0x34, // '4'
									 0x00, // terminator
									 0x00,
									 0x0a, // null
									 0x35, // '5'
									 0x00, // terminator
									 0x02, // string
									 0x36, // '6'
									 0x00, // terminator
									 0x0a,0x00,0x00,0x00, // string length
									 'P','u','s','s','y',' ','c','a','t',
									 0x00, // terminator
									 0x05, // binary
									 0x37, // '7'
									 0x00, // terminator
									 0x04,0x00,0x00,0x00, // byte string length
									 0x80, // default subtype
									 'h','i','s','s',
									 0x00 // terminator
		};
		check_equal(v, bson);
	}

	//SECTION("outer object")
	{
		std::vector<uint8_t> v;
		bson::bson_bytes_encoder encoder(v);

		encoder.begin_object();
		encoder.key("a");
		encoder.begin_object();
		encoder.key("0");
		encoder.int64_value((std::numeric_limits<int64_t>::max)());
		encoder.end_object();
		encoder.end_object();
		encoder.flush();

		std::vector<uint8_t> bson = { 0x18,0x00,0x00,0x00,
									 0x03, // object
									 'a',
									 0x00,
									 0x10,0x00,0x00,0x00,
									 0x12, // int64
									 0x30, // '0'
									 0x00, // terminator
									 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,
									 0x00, // terminator
									 0x00 // terminator
		};
		check_equal(v, bson);
	}

	//SECTION("outer array")
	{
		std::vector<uint8_t> v;
		bson::bson_bytes_encoder encoder(v);

		encoder.begin_array();
		encoder.begin_object();
		encoder.key("0");
		encoder.int64_value((std::numeric_limits<int64_t>::max)());
		encoder.end_object();
		encoder.end_array();
		encoder.flush();

		std::vector<uint8_t> bson = { 0x18,0x00,0x00,0x00,
									 0x03, // object
									 '0',
									 0x00,
									 0x10,0x00,0x00,0x00,
									 0x12, // int64
									 0x30, // '0'
									 0x00, // terminator
									 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,
									 0x00, // terminator
									 0x00 // terminator
		};
		check_equal(v, bson);
	}

	{
		std::vector<uint8_t> v;
		bson::bson_bytes_encoder encoder(v);

		encoder.begin_object();
		encoder.key("null");
		encoder.null_value();
		encoder.end_object();
		encoder.flush();

		try
		{
			json result = bson::decode_bson<json>(v);
		}
		catch(const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}
}

void check_decode_bson(const std::vector<uint8_t>& v, const json& expected)
{
	json result = bson::decode_bson<json>(v);
	__CHECK_LOG__(result == expected);

	std::string s;
	for (auto c : v)
	{
		s.push_back(c);
	}
	std::istringstream is(s);
	json j2 = bson::decode_bson<json>(is);
	__CHECK_LOG__(j2 == expected);
}

void testbsonreader()
{
	cout << "bson reader" << endl;
	check_decode_bson({ 0x16,0x00,0x00,0x00, // total document size
					  0x02, // string
					  'h','e','l','l','o', 0x00, // field name 
					  0x06,0x00,0x00,0x00, // size of value
					  'w','o','r','l','d',0x00, // field value and null terminator
					  0x00 // end of document
		}, json::parse("{\"hello\":\"world\"}"));
}

std::vector<char> read_bytes(const std::string& filename)
{
	std::vector<char> bytes;
	std::string in_file = "./input/bson/test34.bson";
	std::ifstream is(filename, std::ifstream::binary);
	is.seekg(0, is.end);
	std::streamoff length = is.tellg();
	is.seekg(0, is.beg);
	bytes.resize(length);
	is.read(bytes.data(), length);
	is.close();
	return bytes;
}

void testbsonsuite()
{
	cout << "bson suite" << endl;

	SECTION("int32")
	{
		std::string in_file = "./input/bson/test33.bson";
		std::vector<char> bytes = read_bytes(in_file);

		std::vector<char> bytes2;
		ojson j(json_object_arg);
		j.try_emplace("a", -123);
		j.try_emplace("c", 0);
		j.try_emplace("b", 123);
		bson::encode_bson(j, bytes2);
		__CHECK_LOG__(bytes2 == bytes);

		auto j2 = bson::decode_bson<ojson>(bytes);
		__CHECK_LOG__(j2 == j);
	}
	SECTION("int64")
	{
		std::string in_file = "./input/bson/test34.bson";
		std::vector<char> bytes = read_bytes(in_file);

		bytes_source source(bytes);

		uint8_t buf[sizeof(int64_t)];

		source.read(buf, sizeof(int32_t));
		auto doc_size = jsoncons::detail::little_to_native<int32_t>(buf, sizeof(buf));
		__CHECK_LOG__(doc_size == 16);
		auto result = source.get_character();
		__CHECK_LOG__(result.value() == 0x12); // 64-bit integer
		std::string s;
		for (result = source.get_character();
			result && result.value() != 0; result = source.get_character())
		{
			s.push_back(result.value());
		}
		__CHECK_LOG__(s == std::string("a"));
		source.read(buf, sizeof(int64_t));
		auto val = jsoncons::detail::little_to_native<int64_t>(buf, sizeof(int64_t));
		__CHECK_LOG__(val == 100000000000000ULL);
		result = source.get_character();
		__CHECK_LOG__(result.value() == 0);
		__CHECK_LOG__(source.eof());

		std::vector<char> bytes2;
		std::map<std::string, int64_t> m{ {"a", val} };
		bson::encode_bson(m, bytes2);
		__CHECK_LOG__(bytes2 == bytes);

		auto m2 = bson::decode_bson<std::map<std::string, int64_t>>(bytes);
		__CHECK_LOG__(m2 == m);
	}
	SECTION("double")
	{
		std::string in_file = "./input/bson/test20.bson";
		std::vector<char> bytes = read_bytes(in_file);

		std::vector<char> bytes2;
		std::map<std::string, double> m = { {"double", 123.4567} };
		bson::encode_bson(m, bytes2);
		__CHECK_LOG__(bytes2 == bytes);

		auto m2 = bson::decode_bson<std::map<std::string, double>>(bytes);
		__CHECK_LOG__(m2 == m);
	}
	SECTION("bool")
	{
		std::string in_file = "./input/bson/test19.bson";
		std::vector<char> bytes = read_bytes(in_file);

		std::vector<char> bytes2;
		std::map<std::string, bool> m = { {"bool", true} };
		bson::encode_bson(m, bytes2);
		__CHECK_LOG__(bytes2 == bytes);

		auto m2 = bson::decode_bson<std::map<std::string, bool>>(bytes);
		__CHECK_LOG__(m2 == m);
	}
	SECTION("array")
	{
		std::string in_file = "./input/bson/test23.bson";
		std::vector<char> bytes = read_bytes(in_file);

		std::vector<char> bytes2;
		ojson a(json_array_arg);
		a.push_back("hello");
		a.push_back("world");

		ojson b;
		b["array"] = std::move(a);

		bson::encode_bson(b, bytes2);
		__CHECK_LOG__(bytes2 == bytes);

		auto b2 = bson::decode_bson<ojson>(bytes);
		__CHECK_LOG__(b2 == b);
	}
	SECTION("binary")
	{
		std::string in_file = "./input/bson/test24.bson";
		std::vector<char> bytes = read_bytes(in_file);

		std::vector<char> bytes2;
		std::vector<uint8_t> bstr = { '1', '2', '3', '4' };

		json b;
		b.try_emplace("binary", byte_string_arg, bstr, 0x80);

		bson::encode_bson(b, bytes2);
		//std::cout << byte_string_view(bytes2) << "\n\n";
		//std::cout << byte_string_view(bytes) << "\n\n";
		__CHECK_LOG__(bytes2 == bytes);

		auto b2 = bson::decode_bson<json>(bytes);
		__CHECK_LOG__(b2 == b);
	}
	SECTION("binary (jsoncons default)")
	{
		std::string in_file = "./input/bson/test24.bson";
		std::vector<char> bytes = read_bytes(in_file);

		std::vector<char> bytes2;
		std::vector<uint8_t> bstr = { '1', '2', '3', '4' };

		json b;
		b.try_emplace("binary", byte_string_arg, bstr); // default subtype is user defined

		bson::encode_bson(b, bytes2);
		//std::cout << byte_string_view(bytes2) << "\n\n";
		//std::cout << byte_string_view(bytes) << "\n\n";
		__CHECK_LOG__(bytes2 == bytes);

		auto b2 = bson::decode_bson<json>(bytes);
		__CHECK_LOG__(b2 == b);
	}
	SECTION("null")
	{
		std::string in_file = "./input/bson/test18.bson";
		std::vector<char> bytes = read_bytes(in_file);

		json j;
		j.try_emplace("hello", null_type());

		std::vector<char> bytes2;
		bson::encode_bson(j, bytes2);
		//std::cout << byte_string_view(bytes2) << "\n\n";
		//std::cout << byte_string_view(bytes) << "\n\n";
		__CHECK_LOG__(bytes2 == bytes);

		auto b2 = bson::decode_bson<json>(bytes);
		__CHECK_LOG__(b2 == j);
	}
	SECTION("utf8")
	{
		std::string in_file = "./input/bson/test11.bson";
		std::vector<char> bytes = read_bytes(in_file);

		json j;
		j.try_emplace("hello", "world");

		std::vector<char> bytes2;
		bson::encode_bson(j, bytes2);
		//std::cout << byte_string_view(bytes2) << "\n\n";
		//std::cout << byte_string_view(bytes) << "\n\n";
		__CHECK_LOG__(bytes2 == bytes);

		auto b2 = bson::decode_bson<json>(bytes);
		__CHECK_LOG__(b2 == j);
	}
	SECTION("document")
	{
		std::string in_file = "./input/bson/test21.bson";
		std::vector<char> bytes = read_bytes(in_file);

		json b;
		b.try_emplace("document", json());

		std::vector<char> bytes2;
		bson::encode_bson(b, bytes2);
		//std::cout << byte_string_view(bytes2) << "\n\n";
		//std::cout << byte_string_view(bytes) << "\n\n";
		__CHECK_LOG__(bytes2 == bytes);

		auto b2 = bson::decode_bson<json>(bytes);
		__CHECK_LOG__(b2 == b);
	}
}

class MyIterator
{
	const uint8_t* p_;
public:
	using iterator_category = std::input_iterator_tag;
	using value_type = uint8_t;
	using difference_type = std::ptrdiff_t;
	using pointer = const uint8_t*;
	using reference = const uint8_t&;

	MyIterator(const uint8_t* p)
		: p_(p)
	{
	}

	reference operator*() const
	{
		return *p_;
	}

	pointer operator->() const
	{
		return p_;
	}

	MyIterator& operator++()
	{
		++p_;
		return *this;
	}

	MyIterator operator++(int)
	{
		MyIterator temp(*this);
		++*this;
		return temp;
	}

	bool operator!=(const MyIterator& rhs) const
	{
		return p_ != rhs.p_;
	}
};

void testbsonencodedecode()
{
	cout << "bson encode decode"<<endl;

	std::vector<uint8_t> input = { 0x27,0x00,0x00,0x00, // Total number of bytes comprising the document (40 bytes) 
			0x02, // URF-8 string
				0x48,0x65,0x6c,0x6c,0x6f, // Hello
				0x00, // trailing byte 
			0x06,0x00,0x00,0x00, // Number bytes in string (including trailing byte)
				0x57,0x6f,0x72,0x6c,0x64, // World
				0x00, // trailing byte
			0x05, // binary
				0x44,0x61,0x74,0x61, // Data
				0x00, // trailing byte
			0x06,0x00,0x00,0x00, // number of bytes
				0x80, // subtype
				0x66,0x6f,0x6f,0x62,0x61,0x72,
			0x00 };

	SECTION("from bytes")
	{
		ojson j = bson::decode_bson<ojson>(input);

		std::vector<uint8_t> buffer;
		bson::encode_bson(j, buffer);
		__CHECK_LOG__(buffer == input);
	}

	SECTION("from stream")
	{
		std::string s(reinterpret_cast<const char*>(input.data()), input.size());
		std::stringstream is(std::move(s));

		ojson j = bson::decode_bson<ojson>(is);

		std::vector<uint8_t> buffer;
		bson::encode_bson(j, buffer);
		__CHECK_LOG__(buffer == input);
	}

	SECTION("from iterator source")
	{
		ojson j = bson::decode_bson<ojson>(input.begin(), input.end());

		std::vector<uint8_t> buffer;
		bson::encode_bson(j, buffer);
		__CHECK_LOG__(buffer == input);
	}

	SECTION("from custom iterator source")
	{
		MyIterator it(input.data());
		MyIterator end(input.data() + input.size());

		ojson j = bson::decode_bson<ojson>(it, end);

		std::vector<uint8_t> buffer;
		bson::encode_bson(j, buffer);
		__CHECK_LOG__(buffer == input);
	}
}

int main()
{
	testbsoncursor();

	testbsonencoder();
	testbsonreader();
	testbsonsuite();
	system("pause");
	return 0;
}