# ChatTracker

This an implementation of a chatroom and user tracking system built in C++ as a project for my Data Structure and Algorithm's course. Its main focus was on the use of Data Structures and Algorithms when working with large amounts of data for application.

The project objective was to design a system that can track thousands of chatrooms and users that join chatrooms in an efficient manner. We were not allowed to use any of the STL implementations of data structures hat use hashing, so I had to create my own implementation of a hash table for the project. 

`ChatTracker.cpp` contains my implementation that works with the provided interface. It is well commented for ease of understanding

The testing scripts provided by the instructors are as follows: <br>
`generateTests.cpp`  : creates tests by generating a series of random users and chatrooms (no. of each specified in file) acc. to interface of `testChatTracker.cpp`  <br>
`testChatTracker.cpp`: uses `commands.txt` to test the efficiency and accuracy of `ChatTracker.cpp`. The commands are described in file  <br>

Usage:
``` 
g++ -o generateTests generateTests.cpp
echo commands.txt | ./generateTests
g++ -o testChatTracker testChatTracker.cpp ChatTracker.cpp -O2
./testChatTracker
```

`report.docx` contains additional details about this implementation
