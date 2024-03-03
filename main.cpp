#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // For system("clear")
#include <termios.h>
#include <unistd.h>
#include "editor.hpp"

GapBuffer::GapBuffer(int initialSize) : bufferSize(initialSize), gapStart(0), gapEnd(initialSize - 1)
{
    buffer = new char[bufferSize];
    std::fill(buffer, buffer + bufferSize, ' '); // Initialize buffer with spaces
}

GapBuffer::~GapBuffer()
{
    delete[] buffer;
}

void GapBuffer::moveGapTo(int position)
{
    if (position < 0 || position >= bufferSize)
    {
        std::cerr << "Invalid position." << std::endl;
        return;
    }

    int gapSize = gapEnd - gapStart + 1;
    if (position < gapStart)
    {
        int shift = gapStart - position;
        std::copy(buffer + position, buffer + gapStart, buffer + gapEnd - shift + 1);
        gapStart = position;
        gapEnd = gapStart + gapSize - 1;
    }
    else if (position > gapEnd)
    {
        int shift = position - gapEnd;
        std::copy(buffer + gapEnd + 1, buffer + position + 1, buffer + gapStart);
        gapStart = position - gapSize + 1;
        gapEnd = position;
    }
}

void GapBuffer::insertChar(char c)
{
    if (gapStart == gapEnd)
    {
        resizeBuffer();
    }
    buffer[gapStart++] = c;
}

void GapBuffer::deleteChar()
{
    if (gapStart > 0)
    {
        --gapStart;
    }
}

std::string GapBuffer::toString() const
{
    return std::string(buffer, buffer + gapStart) + std::string(buffer + gapEnd + 1, buffer + bufferSize);
}

void GapBuffer::resizeBuffer()
{
    int newBufferSize = bufferSize * 2;
    char *newBuffer = new char[newBufferSize];

    std::copy(buffer, buffer + gapStart, newBuffer);
    std::copy(buffer + gapEnd + 1, buffer + bufferSize, newBuffer + newBufferSize - (bufferSize - gapStart));

    gapEnd = newBufferSize - (bufferSize - gapEnd);
    bufferSize = newBufferSize;

    delete[] buffer;
    buffer = newBuffer;
}

void disableRawMode()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ECHO | ICANON;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

void enableRawMode()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    const std::string filename = argv[1];

    // Check if file exists
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "File not found: " << filename << std::endl;
        return 1;
    }

    // Read file content into a GapBuffer
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    GapBuffer editor(content.size() * 2); // Initial size is double the content size
    for (char c : content)
    {
        editor.insertChar(c);
    }

    // Clear the terminal window
    system("clear");

    // Enable raw mode
    enableRawMode();

    // Editing loop
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q')
    {
        if (c == 'i')
        { // Enter insert mode
            disableRawMode();
            std::string newContent;
            std::getline(std::cin, newContent);
            for (char ch : newContent)
            {
                editor.insertChar(ch);
            }
            enableRawMode();
        }
        else if (c == 'x')
        { // Delete character
            editor.deleteChar();
        }
        else if (c == ':')
        { // Command mode (save and exit)
            disableRawMode();
            std::ofstream outfile(filename);
            if (outfile.is_open())
            {
                outfile << editor.toString();
                outfile.close();
                std::cout << "Changes saved to file. Exiting...\n";
            }
            else
            {
                std::cerr << "Unable to save changes to file.\n";
            }
            break;
        }
        system("clear");
        std::cout << editor.toString() << std::endl;
    }

    // Disable raw mode
    disableRawMode();

    return 0;
}
