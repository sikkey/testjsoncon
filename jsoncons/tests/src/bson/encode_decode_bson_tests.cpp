// Copyright 2016 Daniel Parker
// Distributed under Boost license

#include <jsoncons/json.hpp>
#include <jsoncons_ext/bson/bson.hpp>
#include <sstream>
#include <vector>
#include <utility>
#include <ctime>
#include <limits>
#include <catch/catch.hpp>

using namespace jsoncons;

namespace 
{
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

} // namespace

TEST_CASE("encode decode bson source uint8_t")
{
    std::vector<uint8_t> input = {0x27,0x00,0x00,0x00, // Total number of bytes comprising the document (40 bytes) 
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
            0x00};

    SECTION("from bytes")
    {
        ojson j = bson::decode_bson<ojson>(input);

        std::vector<uint8_t> buffer;
        bson::encode_bson(j, buffer);
        CHECK(buffer == input);
    }

    SECTION("from stream")
    {
        std::string s(reinterpret_cast<const char*>(input.data()), input.size());
        std::stringstream is(std::move(s));

        ojson j = bson::decode_bson<ojson>(is);

        std::vector<uint8_t> buffer;
        bson::encode_bson(j, buffer);
        CHECK(buffer == input);
    }

    SECTION("from iterator source")
    {
        ojson j = bson::decode_bson<ojson>(input.begin(), input.end());

        std::vector<uint8_t> buffer;
        bson::encode_bson(j, buffer);
        CHECK(buffer == input);
    }

    SECTION("from custom iterator source")
    {
        MyIterator it(input.data());
        MyIterator end(input.data() + input.size());

        ojson j = bson::decode_bson<ojson>(it, end);

        std::vector<uint8_t> buffer;
        bson::encode_bson(j, buffer);
        CHECK(buffer == input);
    }
}


