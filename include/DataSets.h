#ifdef _WIN32
#define TOML_WINDOWS_COMPAT 0
#endif
#define TOML_EXCEPTIONS 0

#include <..\toml.hpp>
#include <format>
#include <chrono>
#include <random>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string_view>

namespace dsm {
    namespace fs = std::filesystem;
    
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
    range_generator<T> enumerate(T last) {
        return { {} , last };
    }

    template<class T>
    range_generator<T> enumerate(T first, T last) {
        return { first , last };
    }

    static const constexpr auto version = std::array<std::size_t, 3>{ 0, 1, 0 };
    auto get_version() {
        return std::format("{}.{}.{}", version[0], version[1], version[2]);
    }

    static const constexpr auto year_1900 = 1900;
    auto get_time() {
        std::time_t now = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now());
        auto tmp = std::localtime(&now);
        return  std::format("{}-{}-{} {}:{}:{}",
        tmp->tm_mon, tmp->tm_mday, tmp->tm_year + year_1900,
        tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
    }

    class UUID
    {
    public:
        static UUID create() {
            UUID uuid{};
            std::mt19937 engine { std::random_device{}() };
            std::uniform_int_distribution<int> distribution{ 0, 256 }; //Limits of the interval

            for (auto index : enumerate(16) )
                uuid._data[index] = static_cast<unsigned char>( distribution(engine) );
    
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

    using namespace std::literals::string_view_literals;

#ifdef _WIN32
    const static std::string_view separator = "\\"sv;
#else
    const static std::string_view separator = "/""sv;
#endif      

	class DataSets {

    struct data_set {
        std::size_t index;
        std::string_view key;
        std::string_view description;
        std::string_view source_path;
        std::array<std::size_t, 3> version;
    };
    
    public:
        static bool create(std::string_view && dsm_folder_path) {
            
            const fs::path directory { dsm_folder_path };
        
            if (!fs::exists(directory)) 
                fs::create_directory(directory);
            
            std::string data_lib_config { dsm_folder_path };
            data_lib_config.append(separator).append("data_library.toml"sv);

            if (!fs::exists(data_lib_config)) {
                std::ofstream of(data_lib_config);
                auto data_library_toml_default = toml::table {{ 
                    {"DataSet Management"sv,
                        toml::table {{
                            {"library_software_version"sv, get_version() },
                            {"creatation_time"sv, get_time() }
                        }}
                    },
                    {"Group Labels"sv,
                        toml::table {{
                            {"size"sv, 0 }
                        }}
                    }
                }};
                of << data_library_toml_default;
                of.close();
            }
     
            return true;
        }

    private:
        DataSets() = default;
        toml::table datasets_;
        std::vector<toml::table> group_labed_data_sets;
	};

    /*
        - user api
            at the main
            auto dsm = dsm::create();
            auto data = dsm.use_dataset(dataset_string, process_file);

            auto results = app.process_data(data)

            dsm_          
    */
}
