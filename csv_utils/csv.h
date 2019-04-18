#ifndef CSV_H
#define CSV_H

#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <vector>
#include <exception>
#include <sstream>

namespace {
	template<const int N>
	std::array<std::string, N> split_by_delimiter(const std::string& str, char delimiter) {
		std::array<std::string, N> str_array = {};
		std::istringstream iss(str);
		std::string temp;
		int index = 0;
		while (std::getline(iss, temp, delimiter) && index < N) {
			str_array[index++] = temp;
		}
		return str_array;
	}
}

namespace io {
	namespace exception {
		class FileNotOpenedException : public std::exception {
		public:
			virtual const char* what() const noexcept override {
				return "File is not opened";
			}
		};
	}

	template<typename T>
	class StringToBasicType{
	public:
		T operator()(const std::string &str) const {
			std::stringstream stream;
			stream << str;
			T castedOutput;
			stream >> castedOutput;
			return castedOutput;
		}
	};

	template<typename... Args>
	class Csv {
	public:
		explicit Csv() = default;

		explicit Csv(std::string& file_name, std::ios_base::openmode mode) throw(exception::FileNotOpenedException) {
			m_stream.open(file_name.c_str(), mode);
			if (m_stream.is_open()) {
				if ((mode & std::ios::in) == std::ios::in) {
					std::string header_line;
					std::getline(m_stream, header_line);
					m_headers = split_by_delimiter<sizeof...(Args)>(header_line, ',');
				}
			}
			else {
				throw exception::FileNotOpenedException();
			}
		}

		std::array<std::string, sizeof...(Args)> getHeaders() noexcept {
			return m_headers;
		}

		template<typename T, typename... Arguments>
		void get_values(T &first, Arguments& ... args) {
			std::string line;
			std::getline(m_stream, line);
			get_values_expanded<T, Arguments...>(first, args..., split_by_delimiter<sizeof...(Arguments) + 1>(line, ','));
		}

	private:
		template<typename T, const int N = 1>
		void get_values_expanded(T& arg, const std::array<std::string, N>& arr) {
			StringToBasicType<T> stbt;
			arg = stbt(arr[N - 1]);
		}

		template<typename T, typename... Arguments, const int N = sizeof...(Arguments)>
		void get_values_expanded(T& first, Arguments& ... args, const std::array<std::string, N>& arr) {
			StringToBasicType<T> stbt;
			first = stbt(arr[N - 2]);
			get_values_expanded(args..., arr);
		}

	private:
		std::fstream m_stream;
		std::array<std::string, sizeof...(Args)> m_headers;
	};
}

#endif // !CSV_H
