// Die messages werden auf dem Server gespeichert man kann 
// nicht im client messages ablesen die man selbst nicht geschickt hat. 
// Nur der eigentliche Sender einer Nachricht kann sie Lesen und löschen 
// (der Fehler der im untericht beschprochen wurde)

// twmailer von STEFAN WERNER

// um das programm zu starten ----------
// enter in terminal:

make clean
make all

// ----------------------------------
// To run Server:

./bin/server <PORT> <local Directory>

// Example:

./bin/server 1234 data

// data is automaticaly createn in make all if another directory is needed it needs to be created

// ----------------------------------
// To run client:

./bin/client <ipAdresse> <PORT>

// Example:

./bin/client localhost 1234

// localhost is detected in code and parsed to 127.0.0.1 if specific IP adress is needed it can be inputet

// ----------------------------------

// the first thing asked by client is LOGIN

// valid LOGIN username: fh technikum user id zb: if23b001 (001 needs to be changed to correct id)

// not valid LOGIN username: zB max etc...

// password is hidden from user until completly entered and enter being pressed then only in ****** seen

// in client SEND sends msg to server
// in client READ reades from server
// in client LIST lists all entries from server
// in client DELL deletes a specific message with given id
