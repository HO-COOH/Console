#include "TextVideo.h"
#include "Console.h"
#include "ConsoleEngine.h"
#include <string>
#include <iostream>
#include <iterator>

const std::string currentDir = "../../../../test/";

void VideoTestSingle(std::string const& file)
{
    ConsoleEngine engine{ console };
    engine.setWcharMode();
    /*Play a single video*/
    /*Or use multithreading to play multiple videos at once*/

    auto frame = engine.add<Video>();
    frame.load(currentDir + file);
    /*windows cmd doesn't like .. so I copied it to the build directory*/
    system(file.c_str());
    frame.play(30);

    //TEST only
    //frame.drawColorWall();
}

void VideoTestDual(std::string const& file1, std::string const& file2)
{
    console.printConsoleInfo();
    ConsoleEngine engine{ console };
    engine.setWcharMode();
    /*Play a single video*/
    /*Or use multithreading to play multiple videos at once*/
    auto frame = engine.add<RectangleArea>();

    auto [videoFrameLeft, videoFrameRight] = frame.divide(RectangleArea::Divisor::Vertical, 0);
    auto v_left = videoFrameLeft.add<Video>(engine);
    auto v_right = videoFrameRight.add<Video>(engine);

    //v_UpLeft.load("test.mp4");
    //v_UpRight.load("test.mp4");
    //v_DownLeft.load("test.mp4");
    //v_DownRight.load("test.mp4");
    v_left.load(currentDir + file1);
    v_right.load(currentDir + file2);
    VideoEngine videos{ std::vector{&v_left, &v_right} };
}

void VideoTestQuad(std::string const& file1, std::string const& file2, std::string const& file3, std::string const& file4)
{
    ConsoleEngine engine{ console };
    engine.setWcharMode();
    /*Play a single video*/
    /*Or use multithreading to play multiple videos at once*/
    auto frame = engine.add<RectangleArea>();

    auto [videoFrameLeft, videoFrameRight] = frame.divide(RectangleArea::Divisor::Vertical, 0);
    auto [upLeft, downLeft] = videoFrameLeft.divide(RectangleArea::Divisor::Horizontal, 0);
    auto [upRight, downRight] = videoFrameRight.divide(RectangleArea::Divisor::Horizontal, 0);

    auto v_UpLeft = upLeft.add<Video>(engine);
    auto v_DownLeft = downLeft.add<Video>(engine);
    auto v_UpRight = upRight.add<Video>(engine);
    auto v_DownRight = downRight.add<Video>(engine);

    //v_UpLeft.load("test.mp4");
    //v_UpRight.load("test.mp4");
    //v_DownLeft.load("test.mp4");
    //v_DownRight.load("test.mp4");
    v_UpLeft.load(currentDir + file1);
    v_UpRight.load(currentDir + file2);
    v_DownLeft.load(currentDir + file3);
    v_DownRight.load(currentDir + file4);
    VideoEngine videos{ std::vector{&v_UpLeft, &v_DownLeft, &v_UpRight, &v_DownRight} };
}

void PictureTest()
{
    const std::string file = "test.jpg";
    Console console;
    ConsoleEngine engine{ console };
    engine.setWcharMode();

    auto frame = engine.add<PictureEngine>();
    frame.load(currentDir + file);
}

inline void printUsage()
{
#ifdef WINDOWS
    system("cd");
#endif
    std::cout << R"(
    ConsoleVideo: A simple console video player, supports playing 1 / 2 / 4 video files simultaneously
        Usage :
    ConsoleVideo file1[file2 file3 file4]

        If one file is followed, play it under full - screen mode
        If two files are followed, play them under split - screen mode, one left, one right
        If four files are followed, play them under quad - rangle mode.

        Any other form of invoking are not supported at the time being!
    )";
}

int main(int argc, char** argv)
{
    const auto argCount = argc - 1;
    if(argc==2)
        system(argv[1]);
    switch (argCount)
    {
    case 1:
        VideoTestSingle(argv[1]);
        break;
    case 2:
        VideoTestDual(argv[1], argv[2]);
        break;
    case 4:
        VideoTestQuad(argv[1], argv[2], argv[3], argv[4]);
        break;
    default:
        printUsage();
        return 1;
    }
}