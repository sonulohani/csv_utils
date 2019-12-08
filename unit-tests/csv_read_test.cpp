#include "gtest/gtest.h"

#include "csv.h"
#include <iostream>

TEST(CsvRead, TestOpenInCtor)
{
    // Opening file which is not in existence
    io::Csv<std::string> csv("abc.csv", std::ios_base::in, true);
    EXPECT_FALSE(csv.isOpened());

    io::Csv<std::string> csv1(std::string("100_Sales_Records.csv"), std::ios_base::in, true);
    EXPECT_TRUE(csv1.isOpened());
}

TEST(CsvRead, TestOpen)
{
    // Opening file which is not in existence
    io::Csv<std::string> csv;
    EXPECT_FALSE(csv.open("abc.csv", std::ios_base::in, true));
    EXPECT_FALSE(csv.isOpened());

    EXPECT_TRUE(csv.open(std::string("100_Sales_Records.csv"), std::ios_base::in, true));
    EXPECT_TRUE(csv.isOpened());
}

TEST(CsvRead, TestHeader)
{
    io::Csv<std::string> csv1(std::string("100_Sales_Records.csv"), std::ios_base::in, true);
    auto header1 = csv1.getHeader();
    ASSERT_EQ(1, static_cast<int>(header1.size()));
    EXPECT_STREQ("Region", header1[0].c_str());

    io::Csv<std::string, std::string> csv2(std::string("100_Sales_Records.csv"), std::ios_base::in, true);
    auto header2 = csv2.getHeader();
    ASSERT_EQ(2, static_cast<int>(header2.size()));
    EXPECT_STREQ("Region", header2[0].c_str());
    EXPECT_STREQ("Country", header2[1].c_str());

    io::Csv<std::string, std::string, std::string, std::string, std::string> csv3(std::string("100_Sales_Records.csv"),
                                                                                  std::ios_base::in, true);
    auto header3 = csv3.getHeader();
    ASSERT_EQ(5, static_cast<int>(header3.size()));
    EXPECT_STREQ("Region", header3[0].c_str());
    EXPECT_STREQ("Country", header3[1].c_str());
    EXPECT_STREQ("Item Type", header3[2].c_str());
    EXPECT_STREQ("Sales Channel", header3[3].c_str());
    EXPECT_STREQ("Order Priority", header3[4].c_str());
}

TEST(CsvRead, TestHeaderBeyondRange)
{
    io::Csv<std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string,
            std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string,
            std::string>
        csv(std::string("100_Sales_Records.csv"), std::ios_base::in, true);

    auto header = csv.getHeader();
    ASSERT_EQ(17, static_cast<int>(header.size()));
    EXPECT_STREQ("Total Cost", header[12].c_str());
    EXPECT_STREQ("Total Profit", header[13].c_str());
    EXPECT_TRUE(header[14].empty());
    EXPECT_TRUE(header[15].empty());
    EXPECT_TRUE(header[16].empty());
}

TEST(CsvRead, TestSingleValue)
{
    io::Csv<std::string> csv(std::string("100_Sales_Records.csv"), std::ios_base::in, true);
    std::string region;
    csv.readValues(region);
    EXPECT_STREQ("Australia and Oceania", region.c_str());
}

TEST(CsvRead, TestMultipleValue)
{
    io::Csv<std::string, std::string, std::string> csv(std::string("100_Sales_Records.csv"), std::ios_base::in, true);
    std::string region, country, item_type;
    csv.readValues(region, country, item_type);
    EXPECT_STREQ("Australia and Oceania", region.c_str());
    EXPECT_STREQ("Tuvalu", country.c_str());
    EXPECT_STREQ("Baby Food", item_type.c_str());
    csv.readValues(region, country, item_type);
    EXPECT_STREQ("Central America and the Caribbean", region.c_str());
    EXPECT_STREQ("Grenada", country.c_str());
    EXPECT_STREQ("Cereal", item_type.c_str());
}

// TEST(CsvRead, TestHeaderWhenFileNameIsInvalid)
//{
//	io::Csv<std::string> csv(std::string("abc.csv"),
//			std::ios_base::in, true);
//
//	EXPECT_THROW(csv.get_header(), io::exception::FileNotOpenedException);
//}