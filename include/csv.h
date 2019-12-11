#ifndef CSV_H
#define CSV_H

#include <array>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace
{
template<const int32_t N> std::array<std::string, N> splitByDelimiter(const std::string &str, char delimiter)
{
    std::array<std::string, N> splitted_str_array = {};
    std::istringstream iss(str);
    std::string temp;
    int32_t index = {};
    while (std::getline(iss, temp, delimiter) && index < N)
    {
        splitted_str_array[index++] = temp;
    }
    return splitted_str_array;
}
} // namespace

namespace utils
{

/**
 * @brief Converter class.
 * By default, all the values from csv are read in string. To convert string to other datatype
 * this class is used.
 * @tparam T Type to convert to from string.
 */
template<typename T> class Converter
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

template<> class Converter<std::string>
{
  public:
    std::string operator()(const std::string &str) const
    {
        return str;
    }
};

template<> class Converter<uint16_t>
{
  public:
    uint16_t operator()(const std::string &str) const
    {
        try
        {
            auto convertedValue = std::stoul(str);
            if (convertedValue >= std::numeric_limits<uint16_t>::min() &&
                convertedValue <= std::numeric_limits<uint16_t>::max())
            {
                return static_cast<uint16_t>(convertedValue);
            }
            else
            {
                throw std::range_error("Value conversion failed to uint16_t.");
            }
        }
        catch (const std::invalid_argument &invalidArg)
        {
            std::cerr << invalidArg.what();
            throw;
        }
    }
};

template<> class Converter<int16_t>
{
  public:
    int16_t operator()(const std::string &str) const
    {
        try
        {
            auto convertedValue = std::stoi(str);
            if (convertedValue >= std::numeric_limits<int16_t>::min() &&
                convertedValue <= std::numeric_limits<int16_t>::max())
            {
                return static_cast<int16_t>(convertedValue);
            }
            else
            {
                throw std::range_error("Value conversion failed to int16_t.");
            }
        }
        catch (const std::invalid_argument &invalidArg)
        {
            std::cerr << invalidArg.what();
            throw;
        }
    }
};

template<> class Converter<int32_t>
{
  public:
    int32_t operator()(const std::string &str) const
    {
        try
        {
            return std::stoi(str);
        }
        catch (const std::invalid_argument &invalidArg)
        {
            std::cerr << invalidArg.what();
            throw;
        }
    }
};

template<> class Converter<uint32_t>
{
  public:
    uint32_t operator()(const std::string &str) const
    {
        try
        {
            auto convertedValue = std::stoul(str);
            if (convertedValue >= std::numeric_limits<uint32_t>::min() &&
                convertedValue <= std::numeric_limits<uint32_t>::max())
            {
                return static_cast<uint32_t>(convertedValue);
            }
            else
            {
                throw std::range_error("Value conversion failed to uint32_t.");
            }
        }
        catch (const std::invalid_argument &invalidArg)
        {
            std::cerr << invalidArg.what();
            throw;
        }
    }
};

template<> class Converter<uint64_t>
{
  public:
    uint64_t operator()(const std::string &str) const
    {
        try
        {
            return std::stoul(str);
        }
        catch (const std::invalid_argument &invalidArg)
        {
            std::cerr << invalidArg.what();
            throw;
        }
    }
};

template<> class Converter<int64_t>
{
  public:
    int64_t operator()(const std::string &str) const
    {
        try
        {
            return std::stol(str);
        }
        catch (const std::invalid_argument &invalidArg)
        {
            std::cerr << invalidArg.what();
            throw;
        }
    }
};

template<> class Converter<double>
{
  public:
    double operator()(const std::string &str) const
    {
        try
        {
            return std::stod(str);
        }
        catch (const std::invalid_argument &invalidArg)
        {
            std::cerr << invalidArg.what();
            throw;
        }
    }
};

template<> class Converter<float>
{
  public:
    float operator()(const std::string &str) const
    {
        try
        {
            return std::stof(str);
        }
        catch (const std::invalid_argument &invalidArg)
        {
            std::cerr << invalidArg.what();
            throw;
        }
    }
};

} // namespace utils

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

enum class OpenMode : uint32_t
{
    Read = std::ios::in,
    Write = std::ios::out,
    Append = std::ios::app,
};

inline OpenMode operator&(const OpenMode &lhs, const OpenMode &rhs)
{
    return static_cast<OpenMode>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

inline OpenMode operator&=(const OpenMode &lhs, const OpenMode &rhs)
{
    auto result = (static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
    return static_cast<OpenMode>(result);
}

/**
 * @brief Class to write or read csv files
 *
 * @tparam T
 * @tparam Args
 */
template<typename T, typename... Args> class Csv
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
     * @param fileName Name of the file to be opened for read or write or append
     * @param mode Mode in which file to be opened
     * @param fileHasHeader True if file has header otherwise false
     */
    explicit Csv(const std::string &fileName, const OpenMode mode = OpenMode::Read, const bool fileHasHeader = true)
        : Csv(fileName, m_delimiter, mode, fileHasHeader)
    {
    }

    /**
     * @brief Constructs a new Csv object
     *
     * @param fileName Name of the file to be opened for read or write or append
     * @param delimiter Csv delimiter
     * @param mode Mode in which file to be opened
     * @param fileHasHeader True if file has header otherwise false
     */
    explicit Csv(const std::string &fileName, const char delimiter, const OpenMode mode = OpenMode::Read,
                 const bool fileHasHeader = true)
        : m_delimiter(delimiter)
    {
        open(fileName, mode, fileHasHeader);
    }

    /**
     * @brief Destroys the Csv object
     *
     */
    ~Csv()
    {
        close();
    }

    inline bool isOpened() const
    {
        return m_fileStream.is_open();
    }

    /**
     * @brief Opens the file in mode passed
     *
     * @param fileName Name of the file to be opened for read or write or append
     * @param mode Mode in which file to be opened
     * @param fileHasHeader True if file has header otherwise false
     * @return true
     * @return false
     */
    bool open(const std::string &fileName, OpenMode mode, bool fileHasHeader) noexcept
    {
        // close the previous stream if opened
        close();

        m_openmode = mode;
        m_fileStream.open(fileName.c_str(), static_cast<std::ios_base::openmode>(m_openmode));

        if (m_fileStream.is_open())
        {
            if ((m_openmode & OpenMode::Read) == OpenMode::Read && fileHasHeader)
            {
                readHeadersInternal();
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
    bool canRead() const
    {
        if ((m_openmode & OpenMode::Read) != OpenMode::Read)
            throw exception::FileNotOpenedInReadModeException();

        return !m_fileStream.eof();
    }

    /**
     * @brief Closes the file opened
     *
     */
    void close() noexcept
    {
        if (m_fileStream.is_open())
        {
            m_fileStream.close();
        }
    }

    /**
     * @brief Returns the csv headers array
     *
     * @return auto
     */
    inline std::array<std::string, sizeof...(Args) + 1> getHeader() const noexcept
    {
        if (!m_fileStream.is_open())
            throw exception::FileNotOpenedException();

        if ((m_openmode & OpenMode::Read) != OpenMode::Read)
            throw exception::FileNotOpenedInReadModeException();

        return m_csvHeader;
    }

    /**
     * @brief Reads comma seperated values from the file
     *
     * @param first
     * @param args
     */
    void readValues(T &first, Args &... args)
    {
        if ((m_openmode & OpenMode::Read) != OpenMode::Read)
            throw exception::FileNotOpenedInReadModeException();

        std::string line;
        std::getline(m_fileStream, line);
        auto values = splitByDelimiter<sizeof...(Args) + 1>(line, m_delimiter);
        first = values[0];
        if constexpr (sizeof...(Args))
            readValuesInternal<sizeof...(Args) + 1, Args...>(args..., values);
    }

    /**
     * @brief Sets the csv header to the file. Should be called at the start of
     * writing of the file
     *
     * @tparam T
     * @tparam Args
     * @param first
     * @param args
     */
    template<typename Type, typename... Arguments> void setHeader(const Type &first, const Arguments &... args)
    {
        if ((m_openmode & OpenMode::Write) != OpenMode::Write)
            throw exception::FileNotOpenedInWriteModeException();

        m_fileStream << first << ',';
        writeValues(args...);
    }

    /**
     * @brief Sets the csv header to the file. Should be called at the start of
     * writing of the file
     *
     * @tparam T
     * @param value
     */
    template<typename Type> void setHeader(const Type &value)
    {
        writeValues(value);
    }

    /**
     * @brief Writes command seperated values to the file.
     *
     * @param first
     * @param args
     */
    void writeValues(const T &first, const Args &... args)
    {
        if ((m_openmode & OpenMode::Write) != OpenMode::Write)
            throw exception::FileNotOpenedInWriteModeException();

        m_fileStream << first << ',';
        writeValues(args...);
    }

    /**
     * @brief Writes command seperated values to the file.
     *
     * @tparam T
     * @param value
     */
    template<typename Type> void writeValues(const Type &value)
    {
        m_fileStream << value << '\n';
    }

    /**
     * @brief Sets the csv delimiter
     *
     * @param delimiter
     */
    void setDelimiter(const char delimiter)
    {
        m_delimiter = delimiter;
    }

    /**
     * @brief Returns the delimiter
     *
     * @return char
     */
    char getDelimiter() const
    {
        return m_delimiter;
    }

  private:
    void readHeadersInternal()
    {
        std::string header_line;
        std::getline(m_fileStream, header_line);
        m_csvHeader = splitByDelimiter<sizeof...(Args) + 1>(header_line, m_delimiter);
    }

    template<const int32_t N, typename Type> void readValuesInternal(Type &arg, const std::array<std::string, N> &arr)
    {
        utils::Converter<Type> cs;
        arg = cs(arr[N - 1]);
    }

    template<const int32_t N, typename Type, typename... Arguments>
    void readValuesInternal(Type &first, Arguments &... args, const std::array<std::string, N> &arr)
    {
        utils::Converter<Type> cs;
        first = cs(arr[N - sizeof...(Arguments) - 1]);
        readValuesInternal<N, Arguments...>(args..., arr);
    }

  private:
    std::fstream m_fileStream;
    std::array<std::string, sizeof...(Args) + 1> m_csvHeader;
    OpenMode m_openmode;
    char m_delimiter = ',';
};
} // namespace io

#endif // !CSV_H
