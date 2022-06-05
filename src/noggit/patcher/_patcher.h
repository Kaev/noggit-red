/*
Now we are using .db as the active copy of blizzard db files, we need to add a patcher that saves
the current database version into the working directory. For WOTLK upon save/Manual trigger
we take .db file and export to dbc and inject into .mpq

can create a new patch e.g Patch-n.mpq, add all dbcs and map data, and move patch to WOTLK working directory and all should be good.


Directories


*Patch
*------client
*----------- Patch-n.mpq
*------server
*----------- trinityCore_gameobject.sql
*----------- DBC
*------------- *DBC Files*

*/