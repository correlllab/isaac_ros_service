// https://www.boost.org/doc/libs/1_53_0/doc/html/lockfree/examples.html

/* https://stackoverflow.com/a/16276359

Typically, polling a lock-free queue works best when the queue nearly always has entries, 
a blocking queue works best when the queue is nearly always empty.

The downside of blocking queues is latency, typically of the order of 2-20 uS, due to kernel signaling. 
This can be mitigated by designing the system so that the work done by the consumer threads on each queued item takes much longer than this interval.

The downside of non-blocking queues is the waste of CPU and memory bandwidth while polling an empty queue. 
This can be mitigated by designing the system so that the queue is rarely empty.

As already hinted at by commenters, a non-blocking queue is a very bad idea on single-CPU systems.
*/

#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <iostream>
#include <boost/atomic.hpp>

boost::atomic_int producer_count(0);
boost::atomic_int consumer_count(0);

boost::lockfree::queue<int> queue(128);

const int iterations = 10000000;
const int producer_thread_count = 4;
const int consumer_thread_count = 4;

void producer(void)
{
    for (int i = 0; i != iterations; ++i) {
        int value = ++producer_count;
        while (!queue.push(value))
            ;
    }
}

boost::atomic<bool> done (false);
void consumer(void)
{
    int value;
    while (!done) {
        while (queue.pop(value))
            ++consumer_count;
    }

    while (queue.pop(value))
        ++consumer_count;
}

int main(int argc, char* argv[])
{
    using namespace std;
    cout << "boost::lockfree::queue is ";
    if (!queue.is_lock_free())
        cout << "not ";
    cout << "lockfree" << endl;

    boost::thread_group producer_threads, consumer_threads;

    for (int i = 0; i != producer_thread_count; ++i)
        producer_threads.create_thread(producer);

    for (int i = 0; i != consumer_thread_count; ++i)
        consumer_threads.create_thread(consumer);

    producer_threads.join_all();
    done = true;

    consumer_threads.join_all();

    cout << "produced " << producer_count << " objects." << endl;
    cout << "consumed " << consumer_count << " objects." << endl;
}