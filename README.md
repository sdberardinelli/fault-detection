fault-detection
===============

client config:
-------------
ip=(server ip)

port=(server port)

fault=(true or false)

distribution=(uniform,normal,gamma,beta,none) distribution=none ^ faul=true => param1,param2 are indices of fault

parameters=(param1,param2,probability)




server config:
-------------
distribution=(uniform,normal,gamma,beta,none) distrubution used for checking qurorm

parameters=(param1,param2,probability)

distribution=(uniform,normal,gamma,beta,none) distrubtion used for sending clients jobs

parameters=(param1,param2,probability)

quorum=(0 to 1) ratio for number of clients agree on an answer

client=(0 to 1) ratio for number of clients receive a job

timeout=(n) number of seconds to wait before the next job is sent/processed

trials=(n) number of trails, a trail is defined as a server checking the quorom and it is incorrect

experiment=(n) name/index of the experiement





output (for each trial)
-------------
1. # of clients connected.
2. # of jobs sent out.
2. # of incorrect answers accepted.
3. # of times server checked the answer
4. # of times the quorum agreed
5. # of times the quorum disagreed
6. # of times the quorum was correct
