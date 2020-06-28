# Examples

### Parse and decode

[Parse JSON from a string](#A1)  
[Parse JSON from a file](#A2)  
[Parse JSON from an iterator range](#A10)  
[Parse numbers without loosing precision](#A8)  
[Validate JSON without incurring parse exceptions](#A3)  
[How to allow comments? How not to?](#A4)  
[Set a maximum nesting depth](#A5)  
[Prevent the alphabetic sort of the outputted JSON, retaining the original insertion order](#A6)  
[Decode a JSON text using stateful result and work allocators](#A9)  

### Encode

[Encode a json value to a string](#B1)  
[Encode a json value to a stream](#B2)  
[Escape all non-ascii characters](#B3)  
[Replace the representation of NaN, Inf and -Inf when serializing. And when reading in again.](#B4)

### Stream

[Write some JSON (push)](#I6)  
[Read some JSON (pull)](#I1)  
[Filter the event stream](#I2)  
[Pull nested objects into a basic_json](#I3)  
[Iterate over basic_json items](#I4)  
[Iterate over strongly typed items](#I5)  

### Decode JSON to C++ data structures, encode C++ data structures to JSON

[Serialize with the C++ member names of the class](#G2)  
[Serialize with provided names using the `_NAME_` macros](#G3)  
[Mapping to C++ data structures with and without defaults allowed](#G4)  
[Specialize json_type_traits explicitly](#G1)  
[Serialize non-mandatory std::optional values using the convenience macros](#G5)  
[An example with std::shared_ptr and std::unique_ptr](#G6)  
[Serialize a templated class with the `_TPL_` macros](#G7)  
[An example using JSONCONS_ENUM_TRAITS and JSONCONS_ALL_CTOR_GETTER_TRAITS](#G8)  
[Serialize a polymorphic type based on the presence of members](#G9)  
[Ensuring type selection is possible](#G10)  
[Convert JSON numbers to/from boost multiprecision numbers](#G11)

### Construct

[Construct a json object](#C1)  
[Construct a json array](#C2)  
[Insert a new value in an array at a specific position](#C3)  
[Create arrays of arrays of arrays of ...](#C4)  
[Merge two json objects](#C5)  
[Construct a json byte string](#C6)  

### Access

[Use string_view to access the actual memory that's being used to hold a string](#E1)  
[Given a string in a json object that represents a decimal number, assign it to a double](#E2)  
[Retrieve a big integer that's been parsed as a string](#E3)  
[Look up a key, if found, return the value converted to type T, otherwise, return a default value of type T](#E4)  
[Retrieve a value in a hierarchy of JSON objects](#E5)  
[Retrieve a json value as a byte string](#E6)

### Iterate

[Iterate over a json array](#D1)  
[Iterate over a json object](#D2)  

### Flatten and unflatten

[Flatten a json object with numberish keys to JSON Pointer/value pairs](#H1)  
[Flatten a json object to JSONPath/value pairs](#H2)  

### Search and Replace

[Search for and repace an object member key](#F1)  
[Search for and replace a value](#F2)  

### Parse and decode

<div id="A1"/> 

#### Parse JSON from a string

```
std::string s = R"({"first":1,"second":2,"fourth":3,"fifth":4})";    

json j = json::parse(s);
```

or

```c++
using namespace jsoncons::literals;

json j = R"(
{
    "StartDate" : "2017-03-01",
    "MaturityDate" : "2020-12-30"          
}
)"_json;
```

See [basic_json::parse](ref/json/parse.md). 

<div id="A2"/> 

#### Parse JSON from a file

```
std::ifstream is("myfile.json");    

json j = json::parse(is);
```

See [basic_json::parse](ref/json/parse.md). 

<div id="A10"/> 

#### Parse JSON from an iterator range

```c++
#include <jsoncons/json.hpp>

class MyIterator
{
    const char* p_;
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = char;
    using difference_type = std::ptrdiff_t;
    using pointer = const char*; 
    using reference = const char&;

    MyIterator(const char* p)
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

int main()
{
    char source[] = {'[','\"', 'f','o','o','\"',',','\"', 'b','a','r','\"',']'};

    MyIterator first(source);
    MyIterator last(source + sizeof(source));

    json j = json::parse(first, last);

    std::cout << j << "\n\n";
}
```

Output:

```json
["foo","bar"]
```

See [basic_json::parse](ref/json/parse.md). 

<div id="A8"/> 

#### Parse numbers without loosing precision

By default, jsoncons parses a number with an exponent or fractional part
into a double precision floating point number. If you wish, you can
keep the number as a string with semantic tagging `bigdec`, 
using the `lossless_number` option. You can then put it into a `float`, 
`double`, a boost multiprecision number, or whatever other type you want. 

```c++
#include <jsoncons/json.hpp>

int main()
{
    std::string s = R"(
    {
        "a" : 12.00,
        "b" : 1.23456789012345678901234567890
    }
    )";

    // Default
    json j = json::parse(s);

    std::cout.precision(15);

    // Access as string
    std::cout << "(1) a: " << j["a"].as<std::string>() << ", b: " << j["b"].as<std::string>() << "\n"; 
    // Access as double
    std::cout << "(2) a: " << j["a"].as<double>() << ", b: " << j["b"].as<double>() << "\n\n"; 

    // Using lossless_number option
    json_options options;
    options.lossless_number(true);

    json j2 = json::parse(s, options);
    // Access as string
    std::cout << "(3) a: " << j2["a"].as<std::string>() << ", b: " << j2["b"].as<std::string>() << "\n";
    // Access as double
    std::cout << "(4) a: " << j2["a"].as<double>() << ", b: " << j2["b"].as<double>() << "\n\n"; 
}
```
Output:
```
(1) a: 12.0, b: 1.2345678901234567
(2) a: 12, b: 1.23456789012346

(3) a: 12.00, b: 1.23456789012345678901234567890
(4) a: 12, b: 1.23456789012346
```

<div id="A3"/> 

#### Validate JSON without incurring parse exceptions
```c++
std::string s = R"(
{
    "StartDate" : "2017-03-01",
    "MaturityDate" "2020-12-30"          
}
)";

json_reader reader(s);

// or,
// std::stringstream is(s);
// json_reader reader(is);

std::error_code ec;
reader.read(ec);

if (ec)
{
    std::cout << ec.message() 
              << " on line " << reader.line()
              << " and column " << reader.column()
              << std::endl;
}
```
Output:
```
Expected name separator ':' on line 4 and column 20
```

<div id="A4"/> 

#### How to allow comments? How not to?

jsoncons, by default, accepts and ignores C-style comments

```c++
std::string s = R"(
{
    // Single line comments
    /*
        Multi line comments 
    */
}
)";

// Default
json j = json::parse(s);
std::cout << "(1) " << j << std::endl;

// Strict
try
{
    json j = json::parse(s, strict_json_parsing());
}
catch (const ser_error& e)
{
    std::cout << "(2) " << e.what() << std::endl;
}
```
Output:
```
(1) {}
(2) Illegal comment at line 3 and column 10
```

<div id="A5"/> 

#### Set a maximum nesting depth

Like this,
```c++
std::string s = "[[[[[[[[[[[[[[[[[[[[[\"Too deep\"]]]]]]]]]]]]]]]]]]]]]";
try
{
    json_options options;
    options.max_nesting_depth(20);
    json j = json::parse(s, options);
}
catch (const ser_error& e)
{
     std::cout << e.what() << std::endl;
}
```
Output:
```
Maximum JSON depth exceeded at line 1 and column 21
```

<div id="A6"/> 

#### Prevent the alphabetic sort of the outputted JSON, retaining the original insertion order

Use `ojson` instead of `json` (or `wojson` instead of `wjson`) to retain the original insertion order. 

```c++
ojson j = ojson::parse(R"(
{
    "street_number" : "100",
    "street_name" : "Queen St W",
    "city" : "Toronto",
    "country" : "Canada"
}
)");
std::cout << "(1)\n" << pretty_print(j) << std::endl;

// Insert "postal_code" at end
j.insert_or_assign("postal_code", "M5H 2N2");
std::cout << "(2)\n" << pretty_print(j) << std::endl;

// Insert "province" before "country"
auto it = j.find("country");
j.insert_or_assign(it,"province","Ontario");
std::cout << "(3)\n" << pretty_print(j) << std::endl;
```
Output:
```
(1)
{
    "street_number": "100",
    "street_name": "Queen St W",
    "city": "Toronto",
    "country": "Canada"
}
(2)
{
    "street_number": "100",
    "street_name": "Queen St W",
    "city": "Toronto",
    "country": "Canada",
    "postal_code": "M5H 2N2"
}
(3)
{
    "street_number": "100",
    "street_name": "Queen St W",
    "city": "Toronto",
    "province": "Ontario",
    "country": "Canada",
    "postal_code": "M5H 2N2"
}
```

<div id="A9"/> 

### Decode a JSON text using stateful result and work allocators

```c++
// Given allocator my_alloc with a single-argument constructor my_alloc(int),
// use my_alloc(1) to allocate basic_json memory, my_alloc(2) to allocate
// working memory used by json_decoder, and my_alloc(3) to allocate
// working memory used by basic_json_reader. 

using my_json = basic_json<char,sorted_policy,my_alloc>;

std::ifstream is("book_catalog.json");
json_decoder<my_json,my_alloc> decoder(my_alloc(1),my_alloc(2));

basic_json_reader<char,stream_source<char>,my_alloc> reader(is, decoder, my_alloc(3));
reader.read();

my_json j = decoder.get_result();
std::cout << pretty_print(j) << "\n";
```

### Encode

<div id="B1"/>

#### Encode a json value to a string

```
std::string s;

j.dump(s); // compressed

j.dump(s, indenting::indent); // pretty print
```

<div id="B2"/>

#### Encode a json value to a stream

```
j.dump(std::cout); // compressed

j.dump(std::cout, indenting::indent); // pretty print
```
or
```
std::cout << j << std::endl; // compressed

std::cout << pretty_print(j) << std::endl; // pretty print
```

<div id="B3"/>

#### Escape all non-ascii characters

```
json_options options;
options.escape_all_non_ascii(true);

j.dump(std::cout, options); // compressed

j.dump(std::cout, options, indenting::indent); // pretty print
```
or
```
std::cout << print(j, options) << std::endl; // compressed

std::cout << pretty_print(j, options) << std::endl; // pretty print
```

<div id="B4"/>

#### Replace the representation of NaN, Inf and -Inf when serializing. And when reading in again.

Set the serializing options for `nan` and `inf` to distinct string values.

```c++
json j;
j["field1"] = std::sqrt(-1.0);
j["field2"] = 1.79e308 * 1000;
j["field3"] = -1.79e308 * 1000;

json_options options;
options.nan_to_str("NaN")
       .inf_to_str("Inf"); 

std::ostringstream os;
os << pretty_print(j, options);

std::cout << "(1)\n" << os.str() << std::endl;

json j2 = json::parse(os.str(),options);

std::cout << "\n(2) " << j2["field1"].as<double>() << std::endl;
std::cout << "(3) " << j2["field2"].as<double>() << std::endl;
std::cout << "(4) " << j2["field3"].as<double>() << std::endl;

std::cout << "\n(5)\n" << pretty_print(j2,options) << std::endl;
```

Output:
```json
(1)
{
    "field1": "NaN",
    "field2": "Inf",
    "field3": "-Inf"
}

(2) nan
(3) inf
(4) -inf

(5)
{
    "field1": "NaN",
    "field2": "Inf",
    "field3": "-Inf"
}
```

### Stream

<div id="I6"/> 

#### Write some JSON (push)

```c++
#include <jsoncons/json_cursor.hpp>
#include <jsoncons/json_encoder.hpp>
#include <fstream>
#include <cassert>

int main()
{
    std::ofstream os("./output/book_catalog.json", 
                     std::ios_base::out | std::ios_base::trunc);
    assert(os);

    compact_json_stream_encoder encoder(os); // no indent

    encoder.begin_array();
    encoder.begin_object();
    encoder.key("author");
    encoder.string_value("Haruki Murakami");
    encoder.key("title");
    encoder.string_value("Hard-Boiled Wonderland and the End of the World");
    encoder.key("price");
    encoder.double_value(18.9);
    encoder.end_object();
    encoder.begin_object();
    encoder.key("author");
    encoder.string_value("Graham Greene");
    encoder.key("title");
    encoder.string_value("The Comedians");
    encoder.key("price");
    encoder.double_value(15.74);
    encoder.end_object();
    encoder.end_array();
    encoder.flush();

    os.close();

    // Read the JSON and write it prettified to std::cout
    json_stream_encoder writer(std::cout); // indent

    std::ifstream is("./output/book_catalog.json");
    assert(is);

    json_reader reader(is, writer);
    reader.read();
    std::cout << "\n\n";
}
```
Output:
```
[
    {
        "author": "Haruki Murakami",
        "title": "Hard-Boiled Wonderland and the End of the World",
        "price": 18.9
    },
    {
        "author": "Graham Greene",
        "title": "The Comedians",
        "price": 15.74
    }
]
```

<div id="I1"/> 

#### Read some JSON (pull)

A typical pull parsing application will repeatedly process the `current()` 
event and call `next()` to advance to the next event, until `done()` 
returns `true`.

```c++
#include <jsoncons/json_cursor.hpp>
#include <fstream>

int main()
{
    std::ifstream is("./output/book_catalog.json");

    json_cursor cursor(is);

    for (; !cursor.done(); cursor.next())
    {
        const auto& event = cursor.current();
        switch (event.event_type())
        {
            case staj_event_type::begin_array:
                std::cout << event.event_type() << " " << "\n";
                break;
            case staj_event_type::end_array:
                std::cout << event.event_type() << " " << "\n";
                break;
            case staj_event_type::begin_object:
                std::cout << event.event_type() << " " << "\n";
                break;
            case staj_event_type::end_object:
                std::cout << event.event_type() << " " << "\n";
                break;
            case staj_event_type::key:
                // Or std::string_view, if supported
                std::cout << event.event_type() << ": " << event.get<jsoncons::string_view>() << "\n";
                break;
            case staj_event_type::string_value:
                // Or std::string_view, if supported
                std::cout << event.event_type() << ": " << event.get<jsoncons::string_view>() << "\n";
                break;
            case staj_event_type::null_value:
                std::cout << event.event_type() << "\n";
                break;
            case staj_event_type::bool_value:
                std::cout << event.event_type() << ": " << std::boolalpha << event.get<bool>() << "\n";
                break;
            case staj_event_type::int64_value:
                std::cout << event.event_type() << ": " << event.get<int64_t>() << "\n";
                break;
            case staj_event_type::uint64_value:
                std::cout << event.event_type() << ": " << event.get<uint64_t>() << "\n";
                break;
            case staj_event_type::double_value:
                std::cout << event.event_type() << ": " << event.get<double>() << "\n";
                break;
            default:
                std::cout << "Unhandled event type: " << event.event_type() << " " << "\n";
                break;
        }
    }
}

```
Output:
```
begin_array
begin_object
key: author
string_value: Haruki Murakami
key: title
string_value: Hard-Boiled Wonderland and the End of the World
key: price
double_value: 18.9
end_object
begin_object
key: author
string_value: Graham Greene
key: title
string_value: The Comedians
key: price
double_value: 15.74
end_object
end_array
```

<div id="I2"/> 

#### Filter the event stream

You can apply a filter to a cursor using the pipe syntax (e.g., `cursor | filter1 | filter2 | ...`)

```c++

#include <jsoncons/json_cursor.hpp>
#include <fstream>

// Filter out all events except names of authors

int main()
{
    bool author_next = false;
    auto filter = [&](const staj_event& event, const ser_context&) -> bool
    {
        if (event.event_type() == staj_event_type::key &&
            event.get<jsoncons::string_view>() == "author")
        {
            author_next = true;
            return false;
        }
        if (author_next)
        {
            author_next = false;
            return true;
        }
        return false;
    };

    std::ifstream is("./output/book_catalog.json");

    json_cursor cursor(is);
    auto filtered_c = cursor | filter;

    for (; !filtered_c.done(); filtered_c.next())
    {
        const auto& event = filtered_c.current();
        switch (event.event_type())
        {
            case staj_event_type::string_value:
                std::cout << event.get<jsoncons::string_view>() << "\n";
                break;
            default:
                std::cout << "Unhandled event type: " << event.event_type() << " " << "\n";
                break;
        }
    }
}
```
Output:
```
Haruki Murakami
Graham Greene
```

<div id="I3"/> 

#### Pull nested objects into a basic_json

When positioned on a `begin_object` event, 
the `read_to` function can pull a complete object representing
the events from `begin_object` to `end_object`, 
and when positioned on a `begin_array` event, a complete array
representing the events from `begin_array` ro `end_array`.

```c++
#include <jsoncons/json.hpp> // json_decoder and json
#include <fstream>

int main()
{
    std::ifstream is("./output/book_catalog.json");

    json_cursor cursor(is);

    json_decoder<json> decoder;
    for (; !cursor.done(); cursor.next())
    {
        const auto& event = cursor.current();
        switch (event.event_type())
        {
            case staj_event_type::begin_array:
            {
                std::cout << event.event_type() << " " << "\n";
                break;
            }
            case staj_event_type::end_array:
            {
                std::cout << event.event_type() << " " << "\n";
                break;
            }
            case staj_event_type::begin_object:
            {
                std::cout << event.event_type() << " " << "\n";
                cursor.read_to(decoder);
                json j = decoder.get_result();
                std::cout << pretty_print(j) << "\n";
                break;
            }
            default:
            {
                std::cout << "Unhandled event type: " << event.event_type() << " " << "\n";
                break;
            }
        }
    }
}
```
Output:
```
begin_array
begin_object
{
    "author": "Haruki Murakami",
    "price": 18.9,
    "title": "Hard-Boiled Wonderland and the End of the World"
}
begin_object
{
    "author": "Graham Greene",
    "price": 15.74,
    "title": "The Comedians"
}
end_array
```

See [basic_json_cursor](ref/basic_json_cursor.md) 

<div id="I4"/> 

#### Iterate over basic_json items

```c++
#include <jsoncons/json.hpp> 
#include <fstream>

int main()
{
    std::ifstream is("./output/book_catalog.json");

    json_cursor cursor(is);

    auto view = staj_array<json>(cursor);
    for (const auto& j : view)
    {
        std::cout << pretty_print(j) << "\n";
    }
}
```
Output:
```
{
    "author": "Haruki Murakami",
    "price": 18.9,
    "title": "Hard-Boiled Wonderland and the End of the World"
}
{
    "author": "Graham Greene",
    "price": 15.74,
    "title": "The Comedians"
}
```

<div id="I5"/> 

#### Iterate over strongly typed items

```c++
#include <jsoncons/json.hpp> 
#include <fstream>

namespace ns {

    struct book
    {
        std::string author;
        std::string title;
        double price;
    };

} // namespace ns

JSONCONS_ALL_MEMBER_TRAITS(ns::book,author,title,price)

int main()
{
    std::ifstream is("./output/book_catalog.json");

    json_cursor cursor(is);

    auto view = staj_array<ns::book>(cursor);
    for (const auto& book : view)
    {
        std::cout << book.author << ", " << book.title << "\n";
    }
}
```
Output:
```
Haruki Murakami, Hard-Boiled Wonderland and the End of the World
Graham Greene, The Comedians
```

See [basic_json_cursor](ref/basic_json_cursor.md) 

<div id="G0"/>

### Decode JSON to C++ data structures, encode C++ data structures to JSON

<div id="G2"/>

#### Serialize with the C++ member names of the class

```c++
#include <jsoncons/json.hpp>

namespace ns {

    enum class BookCategory {fiction,biography};

    inline
    std::ostream& operator<<(std::ostream& os, const BookCategory& category)
    {
        switch (category)
        {
            case BookCategory::fiction: os << "fiction, "; break;
            case BookCategory::biography: os << "biography, "; break;
        }
        return os;
    }

    // #1 Class with public member data and default constructor   
    struct Book1
    {
        BookCategory category;
        std::string author;
        std::string title;
        double price;
    };

    // #2 Class with private member data and default constructor   
    class Book2
    {
        BookCategory category;
        std::string author;
        std::string title;
        double price;
        Book2() = default;

        JSONCONS_TYPE_TRAITS_FRIEND
    public:
        BookCategory get_category() const {return category;}

        const std::string& get_author() const {return author;}

        const std::string& get_title() const{return title;}

        double get_price() const{return price;}
    };

    // #3 Class with getters and initializing constructor
    class Book3
    {
        BookCategory category_;
        std::string author_;
        std::string title_;
        double price_;
    public:
        Book3(BookCategory category,
              const std::string& author,
              const std::string& title,
              double price)
            : category_(category), author_(author), title_(title), price_(price)
        {
        }

        BookCategory category() const {return category_;}

        const std::string& author() const{return author_;}

        const std::string& title() const{return title_;}

        double price() const{return price_;}
    };

    // #4 Class with getters and setters
    class Book4
    {
        BookCategory category_;
        std::string author_;
        std::string title_;
        double price_;
    public:
        Book4()
            : price_(0)
        {
        }

        Book4(BookCategory category,
              const std::string& author,
              const std::string& title,
              double price)
            : category_(category), author_(author), title_(title), price_(price)
        {
        }

        BookCategory get_category() const
        {
            return category_;
        }

        void set_category(BookCategory value)
        {
            category_ = value;
        }

        const std::string& get_author() const
        {
            return author_;
        }

        void set_author(const std::string& value)
        {
            author_ = value;
        }

        const std::string& get_title() const
        {
            return title_;
        }

        void set_title(const std::string& value)
        {
            title_ = value;
        }

        double get_price() const
        {
            return price_;
        }

        void set_price(double value)
        {
            price_ = value;
        }
    };

} // namespace ns

// Declare the traits at global scope
JSONCONS_ENUM_TRAITS(ns::BookCategory,fiction,biography)

JSONCONS_ALL_MEMBER_TRAITS(ns::Book1,category,author,title,price)
JSONCONS_ALL_MEMBER_TRAITS(ns::Book2,category,author,title,price)
JSONCONS_ALL_CTOR_GETTER_TRAITS(ns::Book3,category,author,title,price)
JSONCONS_ALL_GETTER_SETTER_TRAITS(ns::Book4,get_,set_,category,author,title,price)

using namespace jsoncons; // for convenience

int main()
{
    const std::string input = R"(
    [
        {
            "category" : "fiction",
            "author" : "Haruki Murakami",
            "title" : "Kafka on the Shore",
            "price" : 25.17
        },
        {
            "category" : "biography",
            "author" : "Robert A. Caro",
            "title" : "The Path to Power: The Years of Lyndon Johnson I",
            "price" : 16.99
        }
    ]
    )";

    std::cout << "(1)\n\n";
    auto books1 = decode_json<std::vector<ns::Book1>>(input);
    for (const auto& item : books1)
    {
        std::cout << item.category << ", "
                  << item.author << ", " 
                  << item.title << ", " 
                  << item.price << "\n";
    }
    std::cout << "\n";
    encode_json(books1, std::cout, indenting::indent);
    std::cout << "\n\n";

    std::cout << "(2)\n\n";
    auto books2 = decode_json<std::vector<ns::Book2>>(input);
    for (const auto& item : books2)
    {
        std::cout << item.get_category() << ", "
                  << item.get_author() << ", " 
                  << item.get_title() << ", " 
                  << item.get_price() << "\n";
    }
    std::cout << "\n";
    encode_json(books2, std::cout, indenting::indent);
    std::cout << "\n\n";

    std::cout << "(3)\n\n";
    auto books3 = decode_json<std::vector<ns::Book3>>(input);
    for (const auto& item : books3)
    {
        std::cout << item.category() << ", "
                  << item.author() << ", " 
                  << item.title() << ", " 
                  << item.price() << "\n";
    }
    std::cout << "\n";
    encode_json(books3, std::cout, indenting::indent);
    std::cout << "\n\n";

    std::cout << "(4)\n\n";
    auto books4 = decode_json<std::vector<ns::Book4>>(input);
    for (const auto& item : books4)
    {
        std::cout << item.get_category() << ", "
                  << item.get_author() << ", " 
                  << item.get_title() << ", " 
                  << item.get_price() << "\n";
    }
    std::cout << "\n";
    encode_json(books4, std::cout, indenting::indent);
    std::cout << "\n\n";
}
```
Output:
```
(1)

fiction, Haruki Murakami, Kafka on the Shore, 25.170000
biography, Robert A. Caro, The Path to Power: The Years of Lyndon Johnson I, 16.990000

[
    {
        "author": "Haruki Murakami",
        "category": "fiction",
        "price": 25.17,
        "title": "Kafka on the Shore"
    },
    {
        "author": "Robert A. Caro",
        "category": "biography",
        "price": 16.99,
        "title": "The Path to Power: The Years of Lyndon Johnson I"
    }
]
```

The output for (2), (3) and (4) is the same.

<div id="G3"/>

#### Serialize with provided names using the `_NAME_` macros

```c++
#include <jsoncons/json.hpp>

namespace ns {

    enum class BookCategory {fiction,biography};

    inline
    std::ostream& operator<<(std::ostream& os, const BookCategory& category)
    {
        switch (category)
        {
            case BookCategory::fiction: os << "fiction, "; break;
            case BookCategory::biography: os << "biography, "; break;
        }
        return os;
    }

    // #1 Class with public member data and default constructor   
    struct Book1
    {
        BookCategory category;
        std::string author;
        std::string title;
        double price;
    };

    // #2 Class with private member data and default constructor   
    class Book2
    {
        BookCategory category_;
        std::string author_;
        std::string title_;
        double price_;
        Book2() = default;

        JSONCONS_TYPE_TRAITS_FRIEND
    public:
        BookCategory category() const {return category_;}

        const std::string& author() const {return author_;}

        const std::string& title() const{return title_;}

        double price() const{return price_;}
    };

    // #3 Class with getters and initializing constructor
    class Book3
    {
        BookCategory category_;
        std::string author_;
        std::string title_;
        double price_;
    public:
        Book3(BookCategory category,
              const std::string& author,
              const std::string& title,
              double price)
            : category_(category), author_(author), title_(title), price_(price)
        {
        }

        BookCategory category() const {return category_;}

        const std::string& author() const{return author_;}

        const std::string& title() const{return title_;}

        double price() const{return price_;}
    };

    // #4 Class with getters, setters and default constructor
    class Book4
    {
        BookCategory category_;
        std::string author_;
        std::string title_;
        double price_;

    public:
        BookCategory getCategory() const {return category_;}
        void setCategory(const BookCategory& value) {category_ = value;}

        const std::string& getAuthor() const {return author_;}
        void setAuthor(const std::string& value) {author_ = value;}

        const std::string& getTitle() const {return title_;}
        void setTitle(const std::string& value) {title_ = value;}

        double getPrice() const {return price_;}
        void setPrice(double value) {price_ = value;}
    };

} // namespace ns

// Declare the traits at global scope
JSONCONS_ENUM_NAME_TRAITS(ns::BookCategory,(fiction,"Fiction"),(biography,"Biography"))

JSONCONS_ALL_MEMBER_NAME_TRAITS(ns::Book1,(category,"Category"),(author,"Author"),
                                          (title,"Title"),(price,"Price"))
JSONCONS_ALL_MEMBER_NAME_TRAITS(ns::Book2,(category_,"Category"),(author_,"Author"),
                                          (title_,"Title"),(price_,"Price"))
JSONCONS_ALL_CTOR_GETTER_NAME_TRAITS(ns::Book3,(category,"Category"),(author,"Author"),
                                               (title,"Title"),(price,"Price"))
JSONCONS_ALL_GETTER_SETTER_NAME_TRAITS(ns::Book4,(getCategory,setCategory,"Category"),
                                                 (getAuthor,setAuthor,"Author"),
                                                 (getTitle,setTitle,"Title"),
                                                 (getPrice,setPrice,"Price"))

using namespace jsoncons; // for convenience

int main()
{
    const std::string input = R"(
    [
        {
            "Category" : "Fiction",
            "Author" : "Haruki Murakami",
            "Title" : "Kafka on the Shore",
            "Price" : 25.17
        },
        {
            "Category" : "Biography",
            "Author" : "Robert A. Caro",
            "Title" : "The Path to Power: The Years of Lyndon Johnson I",
            "Price" : 16.99
        }
    ]
    )";

    std::cout << "(1)\n\n";
    auto books1 = decode_json<std::vector<ns::Book1>>(input);
    for (const auto& item : books1)
    {
        std::cout << item.category << ", "
                  << item.author << ", " 
                  << item.title << ", " 
                  << item.price << "\n";
    }
    std::cout << "\n";
    encode_json(books1, std::cout, indenting::indent);
    std::cout << "\n\n";

    std::cout << "(2)\n\n";
    auto books2 = decode_json<std::vector<ns::Book2>>(input);
    for (const auto& item : books2)
    {
        std::cout << item.category() << ", "
                  << item.author() << ", " 
                  << item.title() << ", " 
                  << item.price() << "\n";
    }
    std::cout << "\n";
    encode_json(books2, std::cout, indenting::indent);
    std::cout << "\n\n";

    std::cout << "(3)\n\n";
    auto books3 = decode_json<std::vector<ns::Book3>>(input);
    for (const auto& item : books3)
    {
        std::cout << item.category() << ", "
                  << item.author() << ", " 
                  << item.title() << ", " 
                  << item.price() << "\n";
    }
    std::cout << "\n";
    encode_json(books3, std::cout, indenting::indent);
    std::cout << "\n\n";

    std::cout << "(4)\n\n";
    auto books4 = decode_json<std::vector<ns::Book4>>(input);
    for (const auto& item : books4)
    {
        std::cout << item.getCategory() << ", "
                  << item.getAuthor() << ", " 
                  << item.getTitle() << ", " 
                  << item.getPrice() << "\n";
    }
    std::cout << "\n";
    encode_json(books4, std::cout, indenting::indent);
    std::cout << "\n\n";
}
```
Output:
```
(1)

fiction, Haruki Murakami, Kafka on the Shore, 25.170000
biography, Robert A. Caro, The Path to Power: The Years of Lyndon Johnson I, 16.990000

[
    {
        "Author": "Haruki Murakami",
        "Category": "Fiction",
        "Price": 25.17,
        "Title": "Kafka on the Shore"
    },
    {
        "Author": "Robert A. Caro",
        "Category": "Biography",
        "Price": 16.99,
        "Title": "The Path to Power: The Years of Lyndon Johnson I"
    }
]
```

The output for (2), (3) and (4) is the same.

<div id="G4"/>

#### Mapping to C++ data structures with and without defaults allowed

`JSONCONS_N_MEMBER_TRAITS` and `JSONCONS_ALL_MEMBER_TRAITS` both generate
the code to specialize `json_type_traits` from member data. The difference is that `JSONCONS_N_MEMBER_TRAITS`
does not require all member names to be present in the JSON data, while `JSONCONS_ALL_MEMBER_TRAITS` does.
More generaly, the qualifier _N_ in the macro name indicates that only a specified number of members
must be present in the JSON.

```c++
#include <iostream>
#include <jsoncons/json.hpp>
#include <vector>
#include <string>

namespace ns {

    class Person
    {
    public:
        Person(const std::string& name, const std::string& surname,
               const std::string& ssn, unsigned int age)
           : name(name), surname(surname), ssn(ssn), age(age) { }

    private:
        // Make json_type_traits specializations friends to give accesses to private members
        JSONCONS_TYPE_TRAITS_FRIEND

        Person() : age(0) {}

        std::string name;
        std::string surname;
        std::string ssn;
        unsigned int age;
    };

} // namespace ns

// Declare the traits. Specify which data members need to be serialized, and how many are mandatory.
JSONCONS_N_MEMBER_TRAITS(ns::Person, 2, name, surname, ssn, age)

int main()
{
    try
    {
        // Incomplete JSON data: field ssn missing
        std::string data = R"({"name":"Rod","surname":"Bro","age":30})";
        auto person = jsoncons::decode_json<ns::Person>(data);

        std::string s;
        jsoncons::encode_json(person, s, indenting::indent);
        std::cout << s << "\n";
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << "";
    }
}
```
Output:
```
{
    "age": 30,
    "name": "Rod",
    "ssn": "",
    "surname": "Bro"
}
```

If all members of the JSON data must be present, use
```
JSONCONS_ALL_MEMBER_TRAITS(ns::Person, name, surname, ssn, age)
```
instead. This will cause an exception to be thrown with the message
```
Key 'ssn' not found
```

<div id="G1"/>

#### Specialize json_type_traits explicitly

jsoncons supports conversion between JSON text and C++ data structures. The functions [decode_json](ref/decode_json.md) 
and [encode_json](ref/encode_json.md) convert JSON formatted strings or streams to C++ data structures and back. 
Decode and encode work for all C++ classes that have 
[json_type_traits](ref/json_type_traits.md) 
defined. The standard library containers are already supported, 
and your own types will be supported too if you specialize `json_type_traits`
in the `jsoncons` namespace. 


```c++
#include <iostream>
#include <jsoncons/json.hpp>
#include <vector>
#include <string>

namespace ns {
    struct book
    {
        std::string author;
        std::string title;
        double price;
    };
} // namespace ns

namespace jsoncons {

    template<class Json>
    struct json_type_traits<Json, ns::book>
    {
        using allocator_type = typename Json::allocator_type;

        static bool is(const Json& j) noexcept
        {
            return j.is_object() && j.contains("author") && 
                   j.contains("title") && j.contains("price");
        }
        static ns::book as(const Json& j)
        {
            ns::book val;
            val.author = j.at("author").template as<std::string>();
            val.title = j.at("title").template as<std::string>();
            val.price = j.at("price").template as<double>();
            return val;
        }
        static Json to_json(const ns::book& val, 
                            allocator_type allocator=allocator_type())
        {
            Json j(allocator);
            j.try_emplace("author", val.author);
            j.try_emplace("title", val.title);
            j.try_emplace("price", val.price);
            return j;
        }
    };
} // namespace jsoncons
```

To save typing and enhance readability, the jsoncons library defines macros, 
so you could also write

```c++
JSONCONS_ALL_MEMBER_TRAITS(ns::book, author, title, price)
```

which expands to the code above.

```c++
using namespace jsoncons; // for convenience

int main()
{
    const std::string s = R"(
    [
        {
            "author" : "Haruki Murakami",
            "title" : "Kafka on the Shore",
            "price" : 25.17
        },
        {
            "author" : "Charles Bukowski",
            "title" : "Pulp",
            "price" : 22.48
        }
    ]
    )";

    std::vector<ns::book> book_list = decode_json<std::vector<ns::book>>(s);

    std::cout << "(1)\n";
    for (const auto& item : book_list)
    {
        std::cout << item.author << ", " 
                  << item.title << ", " 
                  << item.price << "\n";
    }

    std::cout << "\n(2)\n";
    encode_json(book_list, std::cout, indenting::indent);
    std::cout << "\n\n";
}
```
Output:
```
(1)
Haruki Murakami, Kafka on the Shore, 25.17
Charles Bukowski, Pulp, 22.48

(2)
[
    {
        "author": "Haruki Murakami",
        "price": 25.17,
        "title": "Kafka on the Shore"
    },
    {
        "author": "Charles Bukowski",
        "price": 22.48,
        "title": "Pulp"
    }
]
```

<div id="G5"/>

#### Serialize non-mandatory std::optional values using the convenience macros

The jsoncons library includes a [json_type_traits](ref/json_type_traits.md) specialization for 
`jsoncons::optional<T>` if `T` is also specialized. `jsoncons::optional<T>` is aliased to 
[std::optional<T>](https://en.cppreference.com/w/cpp/utility/optional) if 
jsoncons detects the presence of C++17, or if `JSONCONS_HAS_STD_OPTIONAL` is defined.
An empty `jsoncons::optional<T>` value correspond to JSON null.

This example assumes C++17 language support (otherwise substitute `jsoncons::optional`.)

Macro names include qualifiers `_ALL_` or `_N_` to indicate that the generated traits require all
members be present in the JSON, or a specified number be present. For non-mandatory members, the generated 
traits `to_json` function will exclude altogether empty values for `std::optional`.

```c++
#include <cassert>
#include <jsoncons/json.hpp>

namespace ns
{
    class MetaDataReplyTest 
    {
    public:
        MetaDataReplyTest()
            : description()
        {
        }
        const std::string& GetStatus() const 
        {
            return status;
        }
        const std::string& GetPayload() const 
        {
            return payload;
        }
        const std::optional<std::string>& GetDescription() const 
        {
            return description;
        }
    private:
        JSONCONS_TYPE_TRAITS_FRIEND;
        std::string status;
        std::string payload;
        std::optional<std::string> description;
    };
}

JSONCONS_N_MEMBER_TRAITS(ns::MetaDataReplyTest, 2, status, payload, description)

using namespace jsoncons;

int main()
{
    std::string input1 = R"({
      "status": "OK",
      "payload": "Modified",
      "description": "TEST"
    })";
    std::string input2 = R"({
      "status": "OK",
      "payload": "Modified"
    })";

    auto val1 = decode_json<ns::MetaDataReplyTest>(input1);
    assert(val1.GetStatus() == "OK");
    assert(val1.GetPayload() == "Modified");
    assert(val1.GetDescription());
    assert(val1.GetDescription() == "TEST");

    auto val2 = decode_json<ns::MetaDataReplyTest>(input2);
    assert(val2.GetStatus() == "OK");
    assert(val2.GetPayload() == "Modified");
    assert(!val2.GetDescription());

    std::string output1;
    std::string output2;

    encode_json(val2,output2,indenting::indent);
    encode_json(val1,output1,indenting::indent);

    std::cout << "(1)\n";
    std::cout << output1 << "\n\n";

    std::cout << "(2)\n";
    std::cout << output2 << "\n\n";
}
```
Output:
```
(1)
{
    "description": "TEST",
    "payload": "Modified",
    "status": "OK"
}

(2)
{
    "payload": "Modified",
    "status": "OK"
}
```

<div id="G6"/>

#### An example with std::shared_ptr and std::unique_ptr

The jsoncons library includes [json_type_traits](ref/json_type_traits.md) specializations for 
`std::shared_ptr<T>` and `std::unique_ptr<T>` if `T` is not a [polymorphic class](https://en.cppreference.com/w/cpp/language/object#Polymorphic_objects), 
i.e., does not have any virtual functions, and if `T` is also specialized. Empty `std::shared_ptr<T>` and `std::unique_ptr<T>` values correspond to JSON null.
In addition, users can implement `json_type_traits` for `std::shared_ptr` and `std::unique_ptr`
with polymorphic classes using the convenience macro `JSONCONS_POLYMORPHIC_TRAITS`, or by specializing `json_type_traits` explicitly.

The convenience macros whose names include the qualifier `_N_` do not require all members to be present in the JSON.
For these, the generated traits `to_json` function will exclude altogether empty values 
for `std::shared_ptr` and `std::unique_ptr`.

```c++
namespace ns {

    struct smart_pointer_test
    {
        std::shared_ptr<std::string> field1;
        std::unique_ptr<std::string> field2;
        std::shared_ptr<std::string> field3;
        std::unique_ptr<std::string> field4;
        std::shared_ptr<std::string> field5;
        std::unique_ptr<std::string> field6;
        std::shared_ptr<std::string> field7;
        std::unique_ptr<std::string> field8;
    };

} // namespace ns

// Declare the traits, first 4 members mandatory, last 4 non-mandatory
JSONCONS_N_MEMBER_TRAITS(ns::smart_pointer_test,4,field1,field2,field3,field4,field5,field6,field7,field8)

int main()
{
    ns::smart_pointer_test val;
    val.field1 = std::make_shared<std::string>("Field 1"); 
    val.field2 = jsoncons::make_unique<std::string>("Field 2"); 
    val.field3 = std::shared_ptr<std::string>(nullptr);
    val.field4 = std::unique_ptr<std::string>(nullptr);
    val.field5 = std::make_shared<std::string>("Field 5"); 
    val.field6 = jsoncons::make_unique<std::string>("Field 6"); 
    val.field7 = std::shared_ptr<std::string>(nullptr);
    val.field8 = std::unique_ptr<std::string>(nullptr);

    std::string buf;
    encode_json(val, buf, indenting::indent);

    std::cout << buf << "\n";

    auto other = decode_json<ns::smart_pointer_test>(buf);

    assert(*other.field1 == *val.field1);
    assert(*other.field2 == *val.field2);
    assert(!other.field3);
    assert(!other.field4);
    assert(*other.field5 == *val.field5);
    assert(*other.field6 == *val.field6);
    assert(!other.field7);
    assert(!other.field8);
}
```
Output:
```
{
    "field1": "Field 1",
    "field2": "Field 2",
    "field3": null,
    "field4": null,
    "field5": "Field 5",
    "field6": "Field 6"
}
```

<div id="G7"/>

#### Serialize a templated class with the `_TPL_` macros

```c++
#include <cassert>
#include <jsoncons/json.hpp>

namespace ns {
    template <typename T1, typename T2>
    struct TemplatedStruct
    {
          T1 aT1;
          T2 aT2;

          friend bool operator==(const TemplatedStruct& lhs, const TemplatedStruct& rhs)
          {
              return lhs.aT1 == rhs.aT1 && lhs.aT2 == rhs.aT2;  
          }

          friend bool operator!=(const TemplatedStruct& lhs, const TemplatedStruct& rhs)
          {
              return !(lhs == rhs);
          }
    };

} // namespace ns

// Declare the traits. Specify the number of template parameters and which data members need to be serialized.
JSONCONS_TPL_ALL_MEMBER_TRAITS(2,ns::TemplatedStruct,aT1,aT2)

using namespace jsoncons; // for convenience

int main()
{
    using value_type = ns::TemplatedStruct<int,std::wstring>;

    value_type val{1, L"sss"};

    std::wstring s;
    encode_json(val, s);

    auto val2 = decode_json<value_type>(s);
    assert(val2 == val);
}
```

<div id="G8"/>

#### An example using JSONCONS_ENUM_TRAITS and JSONCONS_ALL_CTOR_GETTER_TRAITS

This example makes use of the convenience macros `JSONCONS_ENUM_TRAITS`
and `JSONCONS_ALL_CTOR_GETTER_TRAITS` to specialize the 
[json_type_traits](ref/json_type_traits.md) for the enum type
`ns::hiking_experience` and the classes `ns::hiking_reputon` and 
`ns::hiking_reputation`.
The macro `JSONCONS_ENUM_TRAITS` generates the code from
the enum values, and the macro `JSONCONS_ALL_CTOR_GETTER_TRAITS` 
generates the code from the get functions and a constructor. 
These macro declarations must be placed outside any namespace blocks.

```c++
#include <cassert>
#include <iostream>
#include <jsoncons/json.hpp>

namespace ns {
    enum class hiking_experience {beginner,intermediate,advanced};

    class hiking_reputon
    {
        std::string rater_;
        hiking_experience assertion_;
        std::string rated_;
        double rating_;
    public:
        hiking_reputon(const std::string& rater,
                       hiking_experience assertion,
                       const std::string& rated,
                       double rating)
            : rater_(rater), assertion_(assertion), rated_(rated), rating_(rating)
        {
        }

        const std::string& rater() const {return rater_;}
        hiking_experience assertion() const {return assertion_;}
        const std::string& rated() const {return rated_;}
        double rating() const {return rating_;}

        friend bool operator==(const hiking_reputon& lhs, const hiking_reputon& rhs)
        {
            return lhs.rater_ == rhs.rater_ && lhs.assertion_ == rhs.assertion_ && 
                   lhs.rated_ == rhs.rated_ && lhs.rating_ == rhs.rating_;
        }

        friend bool operator!=(const hiking_reputon& lhs, const hiking_reputon& rhs)
        {
            return !(lhs == rhs);
        };
    };

    class hiking_reputation
    {
        std::string application_;
        std::vector<hiking_reputon> reputons_;
    public:
        hiking_reputation(const std::string& application, 
                          const std::vector<hiking_reputon>& reputons)
            : application_(application), 
              reputons_(reputons)
        {}

        const std::string& application() const { return application_;}
        const std::vector<hiking_reputon>& reputons() const { return reputons_;}

        friend bool operator==(const hiking_reputation& lhs, const hiking_reputation& rhs)
        {
            return (lhs.application_ == rhs.application_) && (lhs.reputons_ == rhs.reputons_);
        }

        friend bool operator!=(const hiking_reputation& lhs, const hiking_reputation& rhs)
        {
            return !(lhs == rhs);
        };
    };

} // namespace ns

// Declare the traits. Specify which data members need to be serialized.
JSONCONS_ENUM_TRAITS(ns::hiking_experience, beginner, intermediate, advanced)
JSONCONS_ALL_CTOR_GETTER_TRAITS(ns::hiking_reputon, rater, assertion, rated, rating)
JSONCONS_ALL_CTOR_GETTER_TRAITS(ns::hiking_reputation, application, reputons)

using namespace jsoncons; // for convenience

int main()
{
    ns::hiking_reputation val("hiking", { ns::hiking_reputon{"HikingAsylum",ns::hiking_experience::advanced,"Marilyn C",0.90} });

    std::string s;
    encode_json(val, s, indenting::indent);
    std::cout << s << "\n";

    auto val2 = decode_json<ns::hiking_reputation>(s);

    assert(val2 == val);
}
```
Output:
```
{
    "application": "hiking",
    "reputons": [
        {
            "assertion": "advanced",
            "rated": "Marilyn C",
            "rater": "HikingAsylum",
            "rating": 0.9
        }
    ]
}
```

<div id="G9"/>

#### Serialize a polymorphic type based on the presence of members

This example uses the convenience macro `JSONCONS_N_CTOR_GETTER_TRAITS`
to generate the [json_type_traits](ref/json_type_traits.md) boilerplate for the `HourlyEmployee` and `CommissionedEmployee` 
derived classes, and `JSONCONS_POLYMORPHIC_TRAITS` to generate the `json_type_traits` boilerplate
for `std::shared_ptr<Employee>` and `std::unique_ptr<Employee>`. The type selection strategy is based
on the presence of mandatory members, in particular, to the `firstName`, `lastName`, and `wage` members of an
`HourlyEmployee`, and to the `firstName`, `lastName`, `baseSalary`, and `commission` members of a `CommissionedEmployee`.
Non-mandatory members are not considered for the purpose of type selection.

```c++
#include <cassert>
#include <iostream>
#include <vector>
#include <jsoncons/json.hpp>

using namespace jsoncons;

namespace ns {

    class Employee
    {
        std::string firstName_;
        std::string lastName_;
    public:
        Employee(const std::string& firstName, const std::string& lastName)
            : firstName_(firstName), lastName_(lastName)
        {
        }
        virtual ~Employee() noexcept = default;

        virtual double calculatePay() const = 0;

        const std::string& firstName() const {return firstName_;}
        const std::string& lastName() const {return lastName_;}
    };

    class HourlyEmployee : public Employee
    {
        double wage_;
        unsigned hours_;
    public:
        HourlyEmployee(const std::string& firstName, const std::string& lastName, 
                       double wage, unsigned hours)
            : Employee(firstName, lastName), 
              wage_(wage), hours_(hours)
        {
        }

        double wage() const {return wage_;}

        unsigned hours() const {return hours_;}

        double calculatePay() const override
        {
            return wage_*hours_;
        }
    };

    class CommissionedEmployee : public Employee
    {
        double baseSalary_;
        double commission_;
        unsigned sales_;
    public:
        CommissionedEmployee(const std::string& firstName, const std::string& lastName, 
                             double baseSalary, double commission, unsigned sales)
            : Employee(firstName, lastName), 
              baseSalary_(baseSalary), commission_(commission), sales_(sales)
        {
        }

        double baseSalary() const
        {
            return baseSalary_;
        }

        double commission() const
        {
            return commission_;
        }

        unsigned sales() const
        {
            return sales_;
        }

        double calculatePay() const override
        {
            return baseSalary_ + commission_*sales_;
        }
    };

} // namespace ns

JSONCONS_N_CTOR_GETTER_TRAITS(ns::HourlyEmployee, 3, firstName, lastName, wage, hours)
JSONCONS_N_CTOR_GETTER_TRAITS(ns::CommissionedEmployee, 4, firstName, lastName, baseSalary, commission, sales)
JSONCONS_POLYMORPHIC_TRAITS(ns::Employee, ns::HourlyEmployee, ns::CommissionedEmployee)

int main()
{
    std::string input = R"(
[
    {
        "firstName": "John",
        "hours": 1000,
        "lastName": "Smith",
        "wage": 40.0
    },
    {
        "baseSalary": 30000.0,
        "commission": 0.25,
        "firstName": "Jane",
        "lastName": "Doe",
        "sales": 1000
    }
]
    )"; 

    auto v = decode_json<std::vector<std::unique_ptr<ns::Employee>>>(input);

    std::cout << "(1)\n";
    for (const auto& p : v)
    {
        std::cout << p->firstName() << " " << p->lastName() << ", " << p->calculatePay() << "\n";
    }

    std::cout << "\n(2)\n";
    encode_json(v, std::cout, indenting::indent);

    std::cout << "\n\n(3)\n";
    json j(v);
    std::cout << pretty_print(j) << "\n\n";
}
```
Output:
```
(1)
John Smith, 40000
Jane Doe, 30250

(2)
[
    {
        "firstName": "John",
        "hours": 1000,
        "lastName": "Smith",
        "wage": 40.0
    },
    {
        "baseSalary": 30000.0,
        "commission": 0.25,
        "firstName": "Jane",
        "lastName": "Doe",
        "sales": 1000
    }
]

(3)
[
    {
        "firstName": "John",
        "hours": 1000,
        "lastName": "Smith",
        "wage": 40.0
    },
    {
        "baseSalary": 30000.0,
        "commission": 0.25,
        "firstName": "Jane",
        "lastName": "Doe",
        "sales": 1000
    }
]
```

<div id="G10"/>

#### Ensuring type selection is possible

When deserializing a polymorphic type, jsoncons needs to know how
to convert a json value to the proper derived class. In the Employee
example above, the type selection strategy is based
on the presence of mandatory members in the derived classes. If
derived classes cannot be distinguished in this way, 
you can introduce extra members. The convenience
macros `JSONCONS_N_MEMBER_TRAITS`, `JSONCONS_ALL_MEMBER_TRAITS`,
`JSONCONS_TPL_N_MEMBER_TRAITS`, `JSONCONS_TPL_ALL_MEMBER_TRAITS`,
`JSONCONS_N_MEMBER_NAME_TRAITS`, `JSONCONS_ALL_MEMBER_NAME_TRAITS`,
`JSONCONS_TPL_N_MEMBER_NAME_TRAITS`, and `JSONCONS_TPL_ALL_MEMBER_NAME_TRAITS`
allow you to have `const` or `static const` data members that are serialized and that 
particpate in the type selection strategy during deserialization. 

```c++
namespace ns {

class Foo
{
public:
    virtual ~Foo() noexcept = default;
};

class Bar : public Foo
{
    static const bool bar = true;
    JSONCONS_TYPE_TRAITS_FRIEND
};

class Baz : public Foo 
{
    static const bool baz = true;
    JSONCONS_TYPE_TRAITS_FRIEND
};

} // ns

JSONCONS_N_MEMBER_TRAITS(ns::Bar,1,bar)
JSONCONS_N_MEMBER_TRAITS(ns::Baz,1,baz)
JSONCONS_POLYMORPHIC_TRAITS(ns::Foo, ns::Bar, ns::Baz)

int main()
{
    std::vector<std::unique_ptr<ns::Foo>> u;
    u.emplace_back(new ns::Bar());
    u.emplace_back(new ns::Baz());

    std::string buffer;
    encode_json(u, buffer);
    std::cout << "(1)\n" << buffer << "\n\n";

    auto v = decode_json<std::vector<std::unique_ptr<ns::Foo>>>(buffer);

    std::cout << "(2)\n";
    for (const auto& ptr : v)
    {
        if (dynamic_cast<ns::Bar*>(ptr.get()))
        {
            std::cout << "A bar\n";
        }
        else if (dynamic_cast<ns::Baz*>(ptr.get()))
        {
            std::cout << "A baz\n";
        } 
    }
}
```

Output:
```
(1)
[{"bar":true},{"baz":true}]

(2)
A bar
A baz
```

<div id="G11"/>

#### Convert JSON numbers to/from boost multiprecision numbers

```
#include <jsoncons/json.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

namespace jsoncons 
{
    template <class Json, class Backend>
    struct json_type_traits<Json,boost::multiprecision::number<Backend>>
    {
        using multiprecision_type = boost::multiprecision::number<Backend>;

        static bool is(const Json& val) noexcept
        {
            if (!(val.is_string() && val.semantic_tag() == semantic_tag::bigdec))
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        static multiprecision_type as(const Json& val)
        {
            return multiprecision_type(val.template as<std::string>());
        }

        static Json to_json(multiprecision_type val)
        {
            return Json(val.str(), semantic_tag::bigdec);
        }
    };
}

int main()
{
    typedef boost::multiprecision::number<boost::multiprecision::cpp_dec_float_50> multiprecision_type;

    std::string s = "[100000000000000000000000000000000.1234]";
    json_options options;
    options.lossless_number(true);
    json j = json::parse(s, options);

    multiprecision_type x = j[0].as<multiprecision_type>();

    std::cout << "(1) " << std::setprecision(std::numeric_limits<multiprecision_type>::max_digits10)
        << x << "\n";

    json j2(json_array_arg, {x});
    std::cout << "(2) " << j2[0].as<std::string>() << "\n";
}
```
Output:
```
(1) 100000000000000000000000000000000.1234
(2) 100000000000000000000000000000000.1234
```

### Construct

<div id="C1"/>

#### Construct a json object

Start with an empty json object and insert some name-value pairs,

```c++
json image_sizing;
image_sizing.insert_or_assign("Resize To Fit",true);  // a boolean 
image_sizing.insert_or_assign("Resize Unit", "pixels");  // a string
image_sizing.insert_or_assign("Resize What", "long_edge");  // a string
image_sizing.insert_or_assign("Dimension 1",9.84);  // a double
image_sizing.insert_or_assign("Dimension 2",json::null());  // a null value
```

or use an object initializer-list,

```c++
json file_settings( json_object_arg,{
        {"Image Format", "JPEG"},
        {"Color Space", "sRGB"},
        {"Limit File Size", true},
        {"Limit File Size To", 10000}
    });
};
```

<div id="C2"/>

#### Construct a json array

```c++
json color_spaces(json_array_arg); // an empty array
color_spaces.push_back("sRGB");
color_spaces.push_back("AdobeRGB");
color_spaces.push_back("ProPhoto RGB");
```

or use an array initializer-list,
```c++
json image_formats(json_array_arg, {"JPEG","PSD","TIFF","DNG"});
```

<div id="C3"/>

#### Insert a new value in an array at a specific position

```c++
json cities(json_array_arg); // an empty array
cities.push_back("Toronto");  
cities.push_back("Vancouver");
// Insert "Montreal" at beginning of array
cities.insert(cities.array_range().begin(),"Montreal");  

std::cout << cities << std::endl;
```
Output:
```
["Montreal","Toronto","Vancouver"]
```

<div id="C4"/>

#### Create arrays of arrays of arrays of ...

Like this:

```c++
json j = json::make_array<3>(4, 3, 2, 0.0);
double val = 1.0;
for (std::size_t i = 0; i < a.size(); ++i)
{
    for (std::size_t j = 0; j < j[i].size(); ++j)
    {
        for (std::size_t k = 0; k < j[i][j].size(); ++k)
        {
            j[i][j][k] = val;
            val += 1.0;
        }
    }
}
std::cout << pretty_print(j) << std::endl;
```
Output:
```json
[
    [
        [1.0,2.0],
        [3.0,4.0],
        [5.0,6.0]
    ],
    [
        [7.0,8.0],
        [9.0,10.0],
        [11.0,12.0]
    ],
    [
        [13.0,14.0],
        [15.0,16.0],
        [17.0,18.0]
    ],
    [
        [19.0,20.0],
        [21.0,22.0],
        [23.0,24.0]
    ]
]
```

<div id="C5"/>

#### Merge two json objects

[json::merge](ref/json/merge.md) inserts another json object's key-value pairs into a json object,
unless they already exist with an equivalent key.

[json::merge_or_update](ref/json/merge_or_update.md) inserts another json object's key-value pairs 
into a json object, or assigns them if they already exist.

The `merge` and `merge_or_update` functions perform only a one-level-deep shallow merge,
not a deep merge of nested objects.

```c++
json another = json::parse(R"(
{
    "a" : "2",
    "c" : [4,5,6]
}
)");

json j = json::parse(R"(
{
    "a" : "1",
    "b" : [1,2,3]
}
)");

j.merge(std::move(another));
std::cout << pretty_print(j) << std::endl;
```
Output:
```json
{
    "a": "1",
    "b": [1,2,3],
    "c": [4,5,6]
}
```

<div id="C6"/>

#### Construct a json byte string

```c++
#include <jsoncons/json.hpp>

namespace jc=jsoncons;

int main()
{
    std::vector<uint8_t> bytes = {'H','e','l','l','o'};

    // default suggested encoding (base64url)
    json j1(byte_string_arg, bytes);
    std::cout << "(1) "<< j1 << "\n\n";

    // base64 suggested encoding
    json j2(byte_string_arg, bytes, semantic_tag::base64);
    std::cout << "(2) "<< j2 << "\n\n";

    // base16 suggested encoding
    json j3(byte_string_arg, bytes, semantic_tag::base16);
    std::cout << "(3) "<< j3 << "\n\n";
}
```

Output:
```
(1) "SGVsbG8"

(2) "SGVsbG8="

(3) "48656C6C6F"
```

### Iterate

<div id="D1"/>

#### Iterate over a json array

```c++
json j(json_array_arg, {1,2,3,4});

for (auto val : j.array_range())
{
    std::cout << val << std::endl;
}
```

<div id="D2"/>

#### Iterate over a json object

```c++
json j(json_object_arg, {
    {"author", "Haruki Murakami"},
    {"title", "Kafka on the Shore"},
    {"price", 25.17}
});

for (const auto& member : j.object_range())
{
    std::cout << member.key() << "=" 
              << member.value() << std::endl;
}
```

### Access

<div id="E1"/>

#### Use string_view to access the actual memory that's being used to hold a string

You can use `j.as<jsoncons::string_view>()`, e.g.
```c++
json j = json::parse("\"Hello World\"");
auto sv = j.as<jsoncons::string_view>();
```
`jsoncons::string_view` supports the member functions of `std::string_view`, including `data()` and `size()`. 

If your compiler supports `std::string_view`, you can also use `j.as<std::string_view>()`.

<div id="E2"/>

#### Given a string in a json object that represents a decimal number, assign it to a double

```c++
json j(json_objectarg, {
    {"price", "25.17"}
});

double price = j["price"].as<double>();
```

<div id="E3"/>
 
#### Retrieve a big integer that's been parsed as a string

If an integer exceeds the range of an `int64_t` or `uint64_t`, jsoncons parses it as a string 
with semantic tagging `bigint`.

```c++
#include <jsoncons/json.hpp>
#include <iostream>
#include <iomanip>

using jsoncons::json;

int main()
{
    std::string input = "-18446744073709551617";

    json j = json::parse(input);

    // Access as string
    std::string s = j.as<std::string>();
    std::cout << "(1) " << s << "\n\n";

    // Access as double
    double d = j.as<double>();
    std::cout << "(2) " << std::setprecision(17) << d << "\n\n";

    // Access as jsoncons::bigint
    jsoncons::bigint bn = j.as<jsoncons::bigint>();
    std::cout << "(3) " << bn << "\n\n";

    // If your compiler supports extended integral types for which std::numeric_limits is specialized 
#if (defined(__GNUC__) || defined(__clang__)) && (!defined(__ALL_ANSI__) && defined(_GLIBCXX_USE_INT128))
    __int128 i = j.as<__int128>();
    std::cout << "(4) " << i << "\n\n";
#endif
}
```
Output:
```
(1) -18446744073709551617

(2) -1.8446744073709552e+19

(3) -18446744073709551617

(4) -18446744073709551617
```

<div id="E4"/>

#### Look up a key, if found, return the value converted to type T, otherwise, return a default value of type T.
 
```c++
json j(json_object_arg, {{"price", "25.17"}});

double price = j.get_value_or<double>("price", 25.00); // returns 25.17

double sale_price = j.get_value_or<double>("sale_price", 22.0); // returns 22.0
```

<div id="E5"/>
 
#### Retrieve a value in a hierarchy of JSON objects

```c++
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>
#include <jsoncons_ext/jsonpath/jsonpath.hpp>

int main()
{
    json j = json::parse(R"(
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

    // Using index or `at` accessors
    std::string result1 = j["reputons"][0]["rated"].as<std::string>();
    std::cout << "(1) " << result1 << std::endl;
    std::string result2 = j.at("reputons").at(0).at("rated").as<std::string>();
    std::cout << "(2) " << result2 << std::endl;

    // Using JSON Pointer
    std::string result3 = jsonpointer::get(j, "/reputons/0/rated").as<std::string>();
    std::cout << "(3) " << result3 << std::endl;

    // Using JSONPath
    json result4 = jsonpath::json_query(j, "$.reputons.0.rated");
    if (result4.size() > 0)
    {
        std::cout << "(4) " << result4[0].as<std::string>() << std::endl;
    }
    json result5 = jsonpath::json_query(j, "$..0.rated");
    if (result5.size() > 0)
    {
        std::cout << "(5) " << result5[0].as<std::string>() << std::endl;
    }
}
```

<div id="E6"/>
 
#### Retrieve a json value as a byte string

```c++
#include <jsoncons/json.hpp>

namespace jc=jsoncons;

int main()
{
    json j;
    j["ByteString"] = json(byte_string_arg, std::vector<uint8_t>{ 'H','e','l','l','o' });
    j["EncodedByteString"] = json("SGVsbG8=", semantic_tag::base64);

    std::cout << "(1)\n";
    std::cout << pretty_print(j) << "\n\n";

    // Retrieve a byte string as a std::vector<uint8_t>
    std::vector<uint8_t> v = j["ByteString"].as<std::vector<uint8_t>>();

    // Retrieve a byte string from a text string containing base64 character values
    byte_string bytes2 = j["EncodedByteString"].as<byte_string>();
    std::cout << "(2) " << bytes2 << "\n\n";

    // Retrieve a byte string view  to access the memory that's holding the byte string
    byte_string_view bsv3 = j["ByteString"].as<byte_string_view>();
    std::cout << "(3) " << bsv3 << "\n\n";

    // Can't retrieve a byte string view of a text string 
    try
    {
        byte_string_view bsv4 = j["EncodedByteString"].as<byte_string_view>();
    }
    catch (const std::exception& e)
    {
        std::cout << "(4) "<< e.what() << "\n\n";
    }
}
```
Output:
```
(1)
{
    "ByteString": "SGVsbG8",
    "EncodedByteString": "SGVsbG8="
}

(2) 48 65 6c 6c 6f

(3) 48 65 6c 6c 6f

(4) Not a byte string
```

### Search and Replace
 
<div id="F1"/>

#### Search for and repace an object member key

You can rename object members with the built in filter [rename_object_key_filter](ref/rename_object_key_filter.md)

```c++
#include <sstream>
#include <jsoncons/json.hpp>
#include <jsoncons/json_filter.hpp>

using namespace jsoncons;

int main()
{
    std::string s = R"({"first":1,"second":2,"fourth":3,"fifth":4})";    

    json_stream_encoder encoder(std::cout);

    // Filters can be chained
    rename_object_key_filter filter2("fifth", "fourth", encoder);
    rename_object_key_filter filter1("fourth", "third", filter2);

    // A filter can be passed to any function that takes a json_visitor ...
    std::cout << "(1) ";
    std::istringstream is(s);
    json_reader reader(is, filter1);
    reader.read();
    std::cout << std::endl;

    // or 
    std::cout << "(2) ";
    ojson j = ojson::parse(s);
    j.dump(filter1);
    std::cout << std::endl;
}
```
Output:
```json
(1) {"first":1,"second":2,"third":3,"fourth":4}
(2) {"first":1,"second":2,"third":3,"fourth":4}
```
 
<div id="F2"/>

#### Search for and replace a value

You can use [json_replace](ref/jsonpath/json_replace.md) in the `jsonpath` extension

```c++
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpath/jsonpath.hpp>

using namespace jsoncons;

int main()
{
    json j = json::parse(R"(
        { "store": {
            "book": [ 
              { "category": "reference",
                "author": "Nigel Rees",
                "title": "Sayings of the Century",
                "price": 8.95
              },
              { "category": "fiction",
                "author": "Evelyn Waugh",
                "title": "Sword of Honour",
                "price": 12.99
              },
              { "category": "fiction",
                "author": "Herman Melville",
                "title": "Moby Dick",
                "isbn": "0-553-21311-3",
                "price": 8.99
              }
            ]
          }
        }
    )");

    // Change the price of "Moby Dick" from $8.99 to $10
    jsonpath::json_replace(j,"$.store.book[?(@.isbn == '0-553-21311-3')].price",10.0);
    std::cout << pretty_print(booklist) << std::endl;
}
```
Output:
```json
{
    "store": {
        "book": [
            {
                "author": "Nigel Rees",
                "category": "reference",
                "price": 8.95,
                "title": "Sayings of the Century"
            },
            {
                "author": "Evelyn Waugh",
                "category": "fiction",
                "price": 12.99,
                "title": "Sword of Honour"
            },
            {
                "author": "Herman Melville",
                "category": "fiction",
                "isbn": "0-553-21311-3",
                "price": 10.0,
                "title": "Moby Dick"
            }
        ]
    }
}
```

### Flatten and unflatten
 
<div id="H1"/> 

#### Flatten a json object with numberish keys to JSON Pointer/value pairs

```c++
#include <iostream>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>

// for brevity
using jsoncons::json; 
namespace jsonpointer = jsoncons::jsonpointer;

int main()
{
    json input = json::parse(R"(
    {
        "discards": {
            "1000": "Record does not exist",
            "1004": "Queue limit exceeded",
            "1010": "Discarding timed-out partial msg"
        },
        "warnings": {
            "0": "Phone number missing country code",
            "1": "State code missing",
            "2": "Zip code missing"
        }
    }
    )");

    json flattened = jsonpointer::flatten(input);
    std::cout << "(1)\n" << pretty_print(flattened) << "\n";

    json unflattened1 = jsonpointer::unflatten(flattened);
    std::cout << "(2)\n" << pretty_print(unflattened1) << "\n";

    json unflattened2 = jsonpointer::unflatten(flattened,
        jsonpointer::unflatten_options::assume_object);
    std::cout << "(3)\n" << pretty_print(unflattened2) << "\n";
}
```
Output:
```
(1)
{
    "/discards/1000": "Record does not exist",
    "/discards/1004": "Queue limit exceeded",
    "/discards/1010": "Discarding timed-out partial msg",
    "/warnings/0": "Phone number missing country code",
    "/warnings/1": "State code missing",
    "/warnings/2": "Zip code missing"
}
(2)
{
    "discards": {
        "1000": "Record does not exist",
        "1004": "Queue limit exceeded",
        "1010": "Discarding timed-out partial msg"
    },
    "warnings": ["Phone number missing country code", "State code missing", "Zip code missing"]
}
(3)
{
    "discards": {
        "1000": "Record does not exist",
        "1004": "Queue limit exceeded",
        "1010": "Discarding timed-out partial msg"
    },
    "warnings": {
        "0": "Phone number missing country code",
        "1": "State code missing",
        "2": "Zip code missing"
    }
}
```
 
Note that unflattening a json object of JSON Pointer-value pairs
has no unique solution. An integer appearing in a path could be an 
array index or it could be an object key.
 
<div id="H2"/> 

#### Flatten a json object to JSONPath/value pairs

```c++
#include <iostream>
#include <cassert>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpath/jsonpath.hpp>

// for brevity
using jsoncons::json; 
namespace jsonpath = jsoncons::jsonpath;

int main()
{
    json input = json::parse(R"(
    {
       "application": "hiking",
       "reputons": [
           {
               "rater": "HikingAsylum",
               "assertion": "advanced",
               "rated": "Marilyn C",
               "rating": 0.90
            },
            {
               "rater": "HikingAsylum",
               "assertion": "intermediate",
               "rated": "Hongmin",
               "rating": 0.75
            }    
        ]
    }
    )");

    json result = jsonpath::flatten(input);

    std::cout << pretty_print(result) << "\n";

    json original = jsonpath::unflatten(result);
    assert(original == input);
}
```
Output:
```
{
    "$['application']": "hiking",
    "$['reputons'][0]['assertion']": "advanced",
    "$['reputons'][0]['rated']": "Marilyn C",
    "$['reputons'][0]['rater']": "HikingAsylum",
    "$['reputons'][0]['rating']": 0.9,
    "$['reputons'][1]['assertion']": "intermediate",
    "$['reputons'][1]['rated']": "Hongmin",
    "$['reputons'][1]['rater']": "HikingAsylum",
    "$['reputons'][1]['rating']": 0.75
}
```

