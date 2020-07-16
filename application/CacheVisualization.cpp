#include "Console.h"
#include "ConsoleEngine.h"
#include "Shape.h"
#include <array>
#include <chrono>



constexpr auto readColor = static_cast<WORD>(Color::GREEN);
constexpr auto cacheColor = static_cast<WORD>(Color::RED);
constexpr auto cacheHitTime = 1;
constexpr auto cacheMissTime = 100;
constexpr auto cacheSize=8;
class Matrix :public RectangleArea
{
    
    std::vector<int> data;
    ConsoleEngine& m_engine;
    std::array<TextBox,3> m_textbox;
    int totalRead = 0;
    int cacheMiss = 0;
    CHAR_INFO* prev;
    void printInfo()
    {
        m_textbox[0].setText(std::string{ "Total Read: " } + std::to_string(totalRead));
        m_textbox[1].setText(std::string{ "Cache Miss: " } + std::to_string(cacheMiss));
        m_textbox[2].setText(std::string{ "Hit Rate: " } + std::to_string(static_cast<int>(100.0f * (static_cast<float>(totalRead - cacheMiss) / totalRead))) + "%");
    }
public:
    Matrix(BufferObject& buffer, short width, short height, short starting_row, short starting_col, ConsoleEngine& eng, int cacheSize=8) 
        :   RectangleArea(buffer, width, height, starting_row, starting_col),
            data(width*height),
            m_engine(eng),
            m_textbox{
            TextBox{buffer, 25, 1, starting_row + 12, starting_col },
            TextBox{buffer, 25, 1, starting_row + 13, starting_col },
            TextBox{buffer, 25, 1, starting_row + 14, starting_col } }
    {
        fill(static_cast<wchar_t>(Shade::Full), Color::WHITE);
        prev = &at(0, 0);
    }

    int& operator()(int row, int col)
    {
        ++totalRead;
        if (at(row, col).Attributes == cacheColor)//cache hit
        {
            prev->Attributes = cacheColor;
            std::this_thread::sleep_for(std::chrono::milliseconds{ cacheHitTime });
        }
        else //cache miss
        {
            fill(static_cast<wchar_t>(Shade::Full), Color::WHITE);
            /*draw cached area*/
            auto x = row;
            auto y = col;
            for(auto i=0; i<cacheSize; ++i)
            {
                at(x, y).Attributes = cacheColor;
                if(++y == width)
                {
                    y = 0;
                    if (++x == height)
                        break;
                }
            }
            ++cacheMiss;
            std::this_thread::sleep_for(std::chrono::milliseconds{ cacheMissTime });
        }
        prev = &at(row, col);
        at(row, col).Attributes = readColor | FOREGROUND_INTENSITY;
        printInfo();
        m_engine.draw();
        return data[row * width + col];
    }

    int rows() const { return height; }
    int columns() const { return width; }
    int getTotalReads() const { return totalRead; }
    int getCacheMiss() const { return cacheMiss; }
};

static void MatrixMulNaive(Matrix& a, Matrix& b, Matrix& result)
{
    for (auto i = 0; i < result.rows(); ++i)
    {
        for (auto j = 0; j < result.columns(); ++j)
        {
            int sum = 0;
            for (auto k = 0; k < a.columns(); ++k)
                sum += a(i, k) * b(k, j);
            result(i, j) = sum;
        }
    }
}

static void MatrixTransposeMul(Matrix& a, Matrix& b, Matrix& b_T, Matrix& result)
{
    /*transform matrix b -> b_T*/
    for (auto i = 0; i < b.rows(); ++i)
    {
        for (auto j = 0; j < b.columns(); ++j)
            b(i, j) = b_T(j, i);
    }

    /*do multiplication*/
    for (auto i = 0; i < result.rows(); ++i)
    {
        for (auto j = 0; j < result.columns(); ++j)
        {
            int sum = 0;
            for (auto k = 0; k < a.columns(); ++k)
                sum += a(i, k) * b_T(j, k);
            result(i, j) = sum;
        }
    }
}


int main()
{
    puts("Select:");
    puts("\t1. Naive matrix multiplication");
    puts("\t2. Transposed matrix multiplication");

    int selection{};
    std::cin >> selection;

    console.clear();
    ConsoleEngine eng{ console };
    eng.setWcharMode();
    switch (selection)
    {
    case 1:
    {    
        
        auto m1 = eng.add<Matrix>(10, 10, 0, 0, eng);
        auto m2 = eng.add<Matrix>(10, 10, 30, 0, eng);
        auto result = eng.add<Matrix>(10, 10, 0, 15, eng);


        eng.draw();

        MatrixMulNaive(m1, m2, result);
        break;
    }
    case 2:
    {
        auto m1 = eng.add<Matrix>(10, 10, 0, 0, eng);
        auto m2 = eng.add<Matrix>(10, 10, 30, 0, eng);
        auto m2_T = eng.add<Matrix>(10, 10, 60, 0, eng);
        auto result = eng.add<Matrix>(10, 10, 0, 15, eng);
        MatrixTransposeMul(m1, m2, m2_T, result);
        break;
    }
    default:
        break;
    }
}
    