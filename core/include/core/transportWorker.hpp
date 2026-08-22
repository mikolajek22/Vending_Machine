#pragma once

#include <thread>

#include <core/iTransport.hpp>
#include <core/iTransactionJournal.hpp>

namespace vending {

class TransportWorker {
    public:
        TransportWorker(ITransactionJournal &transactionJournal ,ITransport &transport);
        ~TransportWorker();

        /**
         * @brief Synchronization method with HTTP server
         * @return True is succeed, False if failed
         */
        bool sync();
        /* thread - start / stop */
        void start(std::chrono::milliseconds interval);
        void stop();

    private:
        bool _threadEnabled;
        std::thread _thread;
        void _thread_entry(std::chrono::milliseconds interval);
        
        std::chrono::milliseconds _interval;    // HTTP polling interval for worker
        ITransactionJournal     &_transactionJournal;
        ITransport              &_transport;

};

} /* namespace vending */