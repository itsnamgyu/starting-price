# starting-price
Project For Sogang University CS Course: System Programming CSE4100-01

## Usage
Make! Run!

## Testing
To test a module, define the TEST macro during compilation. For example,
```
gcc -D TEST module.c
```

## Project Structure
The file structure of this project is inspired by the [Redis Repo](https://github.com/antirez/redis/). Much thanks to all of those involved.

### Module: `dir`
Provides function to print the current directory.

### Module: `hashtable`
Provides the hashtable struct `HashTable` for storing & retrieving `Value` type values for given string keys. Includes  functions for creation, deletion, adding, finding etc.

**Make sure to free `HashTable`s after usage.**

### Module: `help`
Provides function to print the help dialog.

### Module: `history`
Provides the `History` structure type that stores previously registered commands. Includes functions to add commands to `History` and print them.

**Make sure to free `History`s after usage.**

### Module: `memory`
Provides the `Block` structure type that stores 1 megabyte of data as an array of `unsigned char`s. Includes functions for creation, deletion, memory fill, dump, reset etc.

**Make sure to free `Block`s after usage.**

## Dear Grader
Thank you for taking the time to actually read this README :) Here is a gif of a rolling Ice Bear.

![Rolling Ice Bear](https://media.giphy.com/media/7HPgM7IltO4QU/giphy.gif)

### How To Grade
```bash
make
./20161662.out # or whatever you want to do with 20161662.out
```

## Contributing
Only I contribute to this project :P
