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
1. trail
2. # of clients connected.
3. # of jobs sent out.
4. # of times quorum is correct when server checks
5. # of times quorum is incorrect
6. # of times quorum is incorrect when server checks
7. # of times quorum disagrees
8. # of total messages sent
9. # of times quorum agrees
10. # of times quorum is corret
