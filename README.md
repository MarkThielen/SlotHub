SlotHub
==============

This is for Carrera Digital 124/132 systems with Control Unit and PC Unit (or other working cable connection). The overall goal is to show car lap times for training, qualify and race on one screen. Furhter iterations might add other functionality like 

+ adding Car and Driver names/pictures
+ store best lap times or complete races on disk or cloud 
+ make use of the fuel functionalities including the pitlane
+ pace car usage
+ make the whole system run on a rasbery pi
+ etc.

There is a lot to learn. Most information I got so far is from Slotbär (www.slotbaer.de) who has reverse engineered the Carrera protocols.


12/22/2013
At this stage there is some very simple functionality implemented which shows the lap times of the cars on the console and also the current track status information.

12/28/2013
System shows lap times of cars, some track information in a table format using ncurses. 

The car status information include 
+ Car Number
+ Last Laptime
+ Fastest Laptime
+ Diff Fastest to Last 
+ Number of Pit Stops
+ Fuel indicator

Track Status Information include
+ Status of Start Lights
+ Fuel Mode Setting (Off, On, Real)
+ Pitlane Adapter flag
+ Laptime connected flag

It works pretty well so far. There are a lot of things to optimize, especially the code and some weird behavior when the terminal window size is changed. Happy with the progress so far.


01/04/2014
Functionality as described above but have encapsulated functionalites into classes which works pretty well and makes code more readable. Also added Protocol Buffers into some status classes to be able to serialize and send the data to clients via a message queue. Looked ad 0MQ which seems like it can do the job. With this it should be possible to send TrackStatus information, Session/Race Status but also detail CarStatus/Information to connected clients. One of the next steps will be to decouple the display through a local message queue.
