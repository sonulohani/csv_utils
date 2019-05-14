#ifndef CSV_H
#define CSV_H

#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <vector>
#include <exception>
#include <sstream>

namespace
{
template <const int N>
std::array<std::string, N> split_by_delimiter(const std::string &str, char delimiter)
{
	std::array<std::string, N> splitted_str_array = {};
	std::istringstream iss(str);
	std::string temp;
	int index = 0;
	while (std::getline(iss, temp, delimiter) && index < N)
	{
		splitted_str_array[index++] = temp;
	}
	return splitted_str_array;
}

template <typename T>
class CastString
{
  public:
	T operator()(const std::string &str) const
	{
		std::stringstream stream;
		stream << str;
		T castedOutput;
		stream >> castedOutput;
		return castedOutput;
	}
};

template <>
class CastString<std::string>
{
  public:
	std::string operator()(const std::string &str) const
	{
		return str;
	}
};
} // namespace

namespace io
{
namespace exception
{
class FileNotOpenedException : public std::exception
{
  public:
	virtual const char *what() const noexcept override
	{
		return "File is not opened";
	}
};

class FileNotOpenedInReadModeException : std::exception
{
  public:
	virtual const char *what() const noexcept override
	{
		return "File is not opened in read mode";
	}
};
class FileNotOpenedInWriteModeException : std::exception
{
  public:
	virtual const char *what() const noexcept override
	{
		return "File is not opened in write mode";
	}
};
} // namespace exception

/**
 * @brief Class to write or read csv files
 * 
 * @tparam T 
 * @tparam Args 
 */
template<typename T, typename... Args>
class Csv
{
  public:
	/**
	 * @brief Constructs a new Csv object
	 * 
	 */
	explicit Csv() = default;

	/**
	 * @brief Constructs a new Csv object
	 * 
	 * @param file_name Name of the file to be opened for read or write or append
	 * @param mode Mode in which file to be opened. For mode description head over to:- https://en.cppreference.com/w/cpp/io/ios_base/openmode
	 * @param file_has_header True if file has header otherwise false
	 */
	explicit Csv(const std::string &file_name, std::ios_base::openmode mode, bool file_has_header)
	{
		open(file_name, mode, file_has_header);
	}

	/**
	 * @brief Destroys the Csv object
	 * 
	 */
	~Csv()
	{
		close();
	}

	inline bool is_open() const
	{
		return file_stream.is_open();
	}

	/**
	 * @brief Opens the file in mode passed
	 * 
	 * @param file_name Name of the file to be opened for read or write or append
	 * @param mode Mode in which file to be opened. For mode description head over to:- https://en.cppreference.com/w/cpp/io/ios_base/openmode
	 * @param file_has_header True if file has header otherwise false
	 * @return true 
	 * @return false 
	 */
	bool open(const std::string &file_name, std::ios_base::openmode mode, bool file_has_header) noexcept
	{

		// close the previous stream if opened
		close();

		openmode = mode;
		file_stream.open(file_name.c_str(), openmode);

		if (file_stream.is_open())
		{
			if ((openmode & std::ios::in) == std::ios::in && file_has_header)
			{
				read_headers_internal();
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	/**
	 * @brief Returns if file can be read further
	 * 
	 * @return true If file can be read further
	 * @return false If file can't be read further
	 */
	bool can_read() const
	{
		if ((openmode & std::ios::in) != std::ios::in)
			throw exception::FileNotOpenedInReadModeException();

		return !file_stream.eof();
	}

	/**
	 * @brief Closes the file opened
	 * 
	 */
	void close() noexcept
	{
		if (file_stream.is_open())
		{
			file_stream.close();
		}
	}

	/**
	 * @brief Returns the csv headers array
	 * 
	 * @return auto 
	 */
	inline std::array<std::string, sizeof...(Args) + 1> get_header() const noexcept
	{
		if(!file_stream.is_open())
			throw exception::FileNotOpenedException();

		if ((openmode & std::ios::in)!= std::ios::in)
			throw exception::FileNotOpenedInReadModeException();

			return csv_header;
	}

	/**
	 * @brief Reads comma seperated values from the file
	 * 
	 * @param first 
	 * @param args 
	 */
	void read_values(T &first, Args &... args)
	{
		if ((openmode & std::ios::in) != std::ios::in)
			throw exception::FileNotOpenedInReadModeException();

		std::string line;
		std::getline(file_stream, line);
		auto values = split_by_delimiter<sizeof...(Args) + 1>(line, ',');
		first = values[0];
		if constexpr (sizeof...(Args))
			read_values_internal<sizeof...(Args) + 1, Args...>(args..., values);
	}

	/**
	 * @brief Sets the csv header to the file. Should be called at the start of writing of the file
	 * 
	 * @tparam T 
	 * @tparam Args 
	 * @param first 
	 * @param args 
	 */
	template <typename Type, typename... Arguments>
	void set_header(const Type &first, const Arguments &... args)
	{
		if ((openmode & std::ios::out) != std::ios::out)
			throw exception::FileNotOpenedInWriteModeException();

		file_stream << first << ',';
		write_values(args...);
	}

	/**
	 * @brief Sets the csv header to the file. Should be called at the start of writing of the file
	 * 
	 * @tparam T 
	 * @param value 
	 */
	template <typename Type>
	void set_header(const Type &value)
	{
		write_values(value);
	}

	/**
	 * @brief Writes command seperated values to the file.
	 * 
	 * @param first 
	 * @param args 
	 */
	void write_values(const T &first, const Args &... args)
	{
		if ((openmode & std::ios::out) != std::ios::out)
			throw exception::FileNotOpenedInWriteModeException();

		file_stream << first << ',';
		write_values(args...);
	}

	/**
	 * @brief Writes command seperated values to the file.
	 * 
	 * @tparam T 
	 * @param value 
	 */
	template <typename Type>
	void write_values(const Type &value)
	{
		file_stream << value << '\n';
	}

  private:
	void read_headers_internal()
	{
		std::string header_line;
		std::getline(file_stream, header_line);
		csv_header = split_by_delimiter<sizeof...(Args) + 1>(header_line, ',');
	}

	template <const int N, typename Type>
	void read_values_internal(Type &arg, const std::array<std::string, N> &arr)
	{
		CastString<Type> cs;
		arg = cs(arr[N - 1]);
	}

	template <const int N, typename Type, typename... Arguments>
	void read_values_internal(Type &first, Arguments &... args, const std::array<std::string, N> &arr)
	{
		CastString<Type> cs;
		first = cs(arr[N - sizeof...(Arguments) - 1]);
		read_values_internal<N, Arguments...>(args..., arr);
	}

  private:
	std::fstream file_stream;
	std::array<std::string, sizeof...(Args) + 1> csv_header;
	std::ios_base::openmode openmode;
};
} // namespace io

#endif // !CSV_H
