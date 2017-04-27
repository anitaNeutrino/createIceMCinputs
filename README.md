# createIceMCInputs
Code to create inputs to icemc using data from the ANITA flights.


Prerequisites are ROOT and the anitaBuildTool.

To prepare the installation type:
$ cmake .

To compile:
$ make

Before running the code check the path to your anita flight data.


To create icemc GPS file run:
Usage 1: ./createIceMCgps [run]
Usage 2: ./createIceMCgps [firstRun] [lastRun]

To create icemc Surf file run:
Usage 1: ./createIceMCSurfFromGps [run]
Usage 2: ./createIceMCSurfFromGps [firstRun] [lastRun]

To create icemc Turf file run:
Usage 1: ./createIceMCTurfFromGps [run]
Usage 2: ./createIceMCTurfFromGps [firstRun] [lastRun]

