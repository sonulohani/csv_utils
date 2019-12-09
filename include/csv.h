#ifndef CSV_H
#define CSV_H

#include <array>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
template<const int N> std::array<std::string, N> splitByDelimiter(const std::string &str, char delimiter)
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

template<typename T> class CastString
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

template<> class CastString<std::string>
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
     * @param mode Mode in which file to be opened.
     * @param fileHasHeader True if file has header otherwise false
     */
    explicit Csv(const std::string &fileName, OpenMode mode = OpenMode::Read, bool fileHasHeader = true)
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
     * @param mode Mode in which file to be opened. For mode description head over
     * to:- https://en.cppreference.com/w/cpp/io/ios_base/openmode
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
        auto values = splitByDelimiter<sizeof...(Args) + 1>(line, ',');
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

  private:
    void readHeadersInternal()
    {
        std::string header_line;
        std::getline(m_fileStream, header_line);
        m_csvHeader = splitByDelimiter<sizeof...(Args) + 1>(header_line, ',');
    }

    template<const int N, typename Type> void readValuesInternal(Type &arg, const std::array<std::string, N> &arr)
    {
        CastString<Type> cs;
        arg = cs(arr[N - 1]);
    }

    template<const int N, typename Type, typename... Arguments>
    void readValuesInternal(Type &first, Arguments &... args, const std::array<std::string, N> &arr)
    {
        CastString<Type> cs;
        first = cs(arr[N - sizeof...(Arguments) - 1]);
        readValuesInternal<N, Arguments...>(args..., arr);
    }

  private:
    std::fstream m_fileStream;
    std::array<std::string, sizeof...(Args) + 1> m_csvHeader;
    OpenMode m_openmode;
};
} // namespace io

#endif // !CSV_H
