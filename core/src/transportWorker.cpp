#include <core/transportWorker.hpp>

#include <algorithm>

#include <iostream>

namespace vending
{



TransportWorker::TransportWorker(ITransactionJournal &transactionJournal ,ITransport &transport) : 
                _transactionJournal(transactionJournal), _transport(transport)
{

}
TransportWorker::~TransportWorker()
{
    stop();
}
/* thread - start / stop */
void TransportWorker::start(std::chrono::milliseconds interval)
{
    _threadEnabled = true;
    _thread = std::thread([this, interval] { _thread_entry(interval); });
}
void TransportWorker::stop()
{
    _threadEnabled = false;
    if (_thread.joinable())
        _thread.join();
}

bool TransportWorker::sync()
{
    auto notSynced = _transactionJournal.findNotSynced();
    bool ret = true;
    for (const auto &transaction : notSynced)
    {
        if(_transport.send(transaction))
            _transactionJournal.syncDone(transaction.uuid);
        else
        {
            ret = false;
            break;
        }
            
    }
    return ret;
}
void TransportWorker::_thread_entry(std::chrono::milliseconds interval)
{
    const auto maxBackoff = std::chrono::milliseconds(5000);
    auto backoff = interval;
    while(_threadEnabled)
    {
        bool syncStatus = sync();
        if (syncStatus)
        {
            backoff = interval;
            std::cout << "Succeeded to Synchronize, current backoff: " << backoff << std::endl;
        }
            
        else
        {
            backoff = std::min(backoff * 2, maxBackoff);
            std::cout << "Succeeded to Synchronize, current backoff: " << backoff << std::endl;
        }
            

        std::this_thread::sleep_for(backoff);
    }
}


} /* namespace vending */