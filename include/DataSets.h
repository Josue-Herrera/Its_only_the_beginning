#ifdef _WIN32
#define TOML_WINDOWS_COMPAT 0
#endif
#define TOML_EXCEPTIONS 0

#include <..\toml.hpp>
#include <random>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>

namespace dsm {
    namespace fs = std::filesystem;
    using version_t = std::array<unsigned int, 3>;
    enum class v : std::size_t { major=0, minor, patch };

    template <typename T>
    struct range_generator {
        T first;
        T last;
        struct iterator {
            using iterator_category = std::input_iterator_tag;
            using value_type = T;
            T value;
            iterator& operator++() { ++value; return *this; }
            iterator operator++(int) = delete;
            bool operator==(iterator const& other) const { return value == other.value; }
            bool operator!=(iterator const& other) const { return !(*this == other); }
            T const& operator*()  const { return value; }
            T const* operator->() const { return std::addressof(value); }
        };
        iterator begin() { return{ first }; }
        iterator end() { return{ last }; }
    };

    template<class T >
    range_generator<T> range(T last) {
        return { {} , last };
    }

    template<class T>
    range_generator<T> range(T first, T last) {
        return { first , last };
    }

    class UUID
    {
    public:
        static UUID create() {
            UUID uuid{};
            std::mt19937 engine { std::random_device{}() };
            std::uniform_int_distribution<int> distribution{ 0, 256 }; //Limits of the interval

            for (auto index : range(16) ) uuid._data[index] = static_cast<unsigned char>( distribution(engine) );
    
            uuid._data[6] = ((uuid._data[6] & 0x0f) | 0x40); // Version 4
            uuid._data[8] = ((uuid._data[8] & 0x3f) | 0x80); // Variant is 10

            return uuid;
        }

        // Returns UUID as formatted string
        std::string to_string()  {
            // Formats to "0065e7d7-418c-4da4-b4d6-b54b6cf7466a"
            char buffer[256]{ 0 };
            std::snprintf(buffer, 255,
                "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                _data[0], _data[1], _data[2], _data[3],
                _data[4], _data[5],
                _data[6], _data[7],
                _data[8], _data[9],
                _data[10], _data[11], _data[12], _data[13], _data[14], _data[15]);
            return { buffer };
        }

    private:
        UUID() = default;
        unsigned char _data[16];
    };

#ifdef _WIN32
    const static std::string separator = "\\";
#else
    const static std::string separator = "/";
#endif      

	class DataSets {

    struct data_set {
        unsigned int index;
        std::string_view key;
        std::string_view description;
        std::string_view source_path;
        version_t   version;

    };
    
    public:
        static bool create() {
            // Get root
            std::string data_library_dir { "D:\\workspace\\datasets" };
            data_library_dir.append(separator).append("data_library");
            const fs::path directory{ data_library_dir };
        
            if (!fs::exists(directory)) {
                fs::create_directory(directory);
                std::ofstream of(data_library_dir.append(separator).append("data_library.toml"));

                auto data_library_toml_default = toml::table{ {
                    {"data_library", "test1" }
                } };

                of << data_library_toml_default;
                of.close();
                return true;
            } else {
                return false;
                // this directoy exists. must warn!
            }
        }

        bool add_project(data_set const & data_set) {
          
        }

    private:
        DataSets() = default;

        toml::table datasets_;
	};
}
