#pragma once

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define min(a, b) ((a)>(b)? (b) : (a))

class Rfifo {
private:
    unsigned char *m_buffer; /* the buffer holding the data */
    unsigned int m_size; /* the size of the allocated buffer */
    unsigned int in; /* data is added at offset (in % size) */
    unsigned int out; /* data is extracted from off. (out % size) */
    pthread_spinlock_t *lock; /* protects concurrent modifications */

public:

    Rfifo(int size = 1024)
    {
        m_size = size;
        m_buffer = new unsigned char[m_size];
        
        in = 0;
        out = 0;

        pthread_spin_init(lock, NULL);
    }

    unsigned int put(unsigned char* buf, unsigned int len)
    {
        unsigned int l;

        len = min(len, in-out);

        l = min(len, m_size - in + out);

        memcpy(m_buffer+(in & (m_size-1)), buf, l);


        memcpy(m_buffer, buf+l, len-l);

        in += len;

        return len;
    }

    unsigned int get(unsigned char* buffer, unsigned int len)
    {
        unsigned int l;
        len = min(len, in-out);
        l = min(len, m_size - (out & (m_size-1)));

        memcpy(buffer, m_buffer+(out & (m_size-1)), l);

        memcpy(buffer+l, m_buffer, len-l);

        out+=len;

        return len;
    }

    // 0 empty
    // =m_size full
    inline unsigned int len()
    {
        return in - out;
    }

};