#include "csv.h"
#include <gtest/gtest.h>

TEST(OpenModeTest, TestModeWithIosBase)
{
    EXPECT_EQ(static_cast<uint32_t>(io::OpenMode::Read), static_cast<uint32_t>(std::ios_base::in));
    EXPECT_EQ(static_cast<uint32_t>(io::OpenMode::Write), static_cast<uint32_t>(std::ios_base::out));
    EXPECT_EQ(static_cast<uint32_t>(io::OpenMode::Append), static_cast<uint32_t>(std::ios_base::app));
}

TEST(OpenModeTest, TestAndOperation)
{
    EXPECT_EQ(io::OpenMode::Read, (io::OpenMode::Read & io::OpenMode::Read));
    EXPECT_EQ(io::OpenMode::Write, (io::OpenMode::Write & io::OpenMode::Write));
    EXPECT_EQ(io::OpenMode::Append, (io::OpenMode::Append & io::OpenMode::Append));

    EXPECT_EQ(io::OpenMode::Read, (io::OpenMode::Read &= io::OpenMode::Read));
    EXPECT_EQ(io::OpenMode::Write, (io::OpenMode::Write &= io::OpenMode::Write));
    EXPECT_EQ(io::OpenMode::Append, (io::OpenMode::Append &= io::OpenMode::Append));
}