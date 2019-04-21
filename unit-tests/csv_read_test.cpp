#include "gtest/gtest.h"

#include "csv.h"

TEST(CsvRead, TestHeader)
{
	io::Csv<std::string> csv1(std::string(R"(C:\Users\sonul\source\repos\csv_utils\unit-tests\test_data\100 Sales Records.csv)"),
					std::ios_base::in, true);
	auto header1 = csv1.get_header();
	EXPECT_EQ(1, static_cast<int>(header1.size()));

	io::Csv<std::string, std::string> csv2(std::string(R"(C:\Users\sonul\source\repos\csv_utils\unit-tests\test_data\100 Sales Records.csv)"),
		std::ios_base::in, true);
	auto header2 = csv2.get_header();
	EXPECT_EQ(2, static_cast<int>(header2.size()));

	io::Csv<std::string, std::string, std::string, std::string, std::string> csv3(std::string(R"(C:\Users\sonul\source\repos\csv_utils\unit-tests\test_data\100 Sales Records.csv)"),
		std::ios_base::in, true);
	auto header3 = csv3.get_header();
	EXPECT_EQ(5, static_cast<int>(header3.size()));
}