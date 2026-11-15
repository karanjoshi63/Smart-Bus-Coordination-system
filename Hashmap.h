#ifndef HASHMAP_H
#define HASHMAP_H

#include <cstring>

const int HASH_SIZE = 211;    // number of slots in the table (a prime number)

// our own HashMap, made with arrays (no STL)
// it saves a text key (like "A Monday") together with a number
// (the position of that section in the sections array)
class HashMap
{
private:
    char keys[HASH_SIZE][40];
    int values[HASH_SIZE];
    bool used[HASH_SIZE];       // is this slot taken or not
    int count;                  // how many keys are saved
    int collisions;             // how many times the slot was already taken while saving
    int lastProbes;             // how many slots the last search had to check

    // turns a key into a slot number between 0 and HASH_SIZE - 1
    int hashFunction(const char key[])
    {
        int h = 0;
        for(int i = 0; key[i] != '\0'; i++)
        {
            // unsigned char: a letter like 'e with accent' is a negative char and would give a negative slot
            h = (h * 31 + (unsigned char)key[i]) % HASH_SIZE;
        }
        return h;
    }

public:
    HashMap()
    {
        count = 0;
        collisions = 0;
        lastProbes = 0;

        for(int i = 0; i < HASH_SIZE; i++)
        {
            used[i] = false;
            values[i] = -1;
            keys[i][0] = '\0';
        }
    }

    // saves a key and its value, returns false if there is no place
    bool put(const char key[], int value)
    {
        if(strlen(key) >= 40)
        {
            return false;
        }

        int slot = hashFunction(key);

        for(int step = 0; step < HASH_SIZE; step++)
        {
            if(!used[slot])
            {
                strcpy(keys[slot], key);
                values[slot] = value;
                used[slot] = true;
                count++;
                return true;
            }

            if(strcmp(keys[slot], key) == 0)
            {
                values[slot] = value;       // same key again, only update the value
                return true;
            }

            collisions++;                   // this slot is taken by a different key
            slot = (slot + 1) % HASH_SIZE;  // try the next slot
        }

        return false;                       // the table is full
    }

    // finds the value of a key, returns -1 if the key is not there
    int get(const char key[])
    {
        int slot = hashFunction(key);
        lastProbes = 0;

        for(int step = 0; step < HASH_SIZE; step++)
        {
            lastProbes++;

            if(!used[slot])
            {
                return -1;                  // an empty slot means the key was never saved
            }

            if(strcmp(keys[slot], key) == 0)
            {
                return values[slot];
            }

            slot = (slot + 1) % HASH_SIZE;
        }

        return -1;
    }

    int getCount()
    {
        return count;
    }

    int getCollisions()
    {
        return collisions;
    }

    int getLastProbes()
    {
        return lastProbes;
    }
};

#endif
